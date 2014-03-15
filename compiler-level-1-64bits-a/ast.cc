
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

#include<iostream>
#include<fstream>

using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast(int line)
{
	report_internal_error("Should not reach, Ast : check_ast");
}

Data_Type Ast::get_data_type()
{
	report_internal_error("Should not reach, Ast : get_data_type");
}

void Ast::print_self(ostream & file_buffer)
{
	report_internal_error("Should not reach, Ast : print_self, This is meant only for Name Ast");
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	report_internal_error("Should not reach, Ast : print_value");
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	report_internal_error("Should not reach, Ast : get_value_of_evaluation");
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	report_internal_error("Should not reach, Ast : set_value_of_evaluation");
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type;
}

bool Assignment_Ast::check_ast(int line)
{
    //std::file_buffer << lhs->get_data_type() << " " << rhs->get_data_type() <<endl;
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}

void Assignment_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")\n";
}


Register_Enum Assignment_Ast::print_icode(ostream & file_buffer)
{
    Register_Enum r_reg = rhs->print_icode(file_buffer); //print iLoad stmts
    file_buffer << " store: " ;
    lhs->print_self(file_buffer); //only for NameAst
    file_buffer <<" <- " << reg_str_map[r_reg] << endl;

    cpu_gpr.free(r_reg); //free the reg containing rhs
    return zero;
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	if (result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print_ast(file_buffer);

	lhs->print_value(eval_env, file_buffer);
    file_buffer <<endl;

	return result;
}
/////////////////////////////////////////////////////////////////
Relational_Expr_Ast::Relational_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs, int temp_op, Data_Type constant_data_type)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
    rel_op = temp_op;
	node_data_type = constant_data_type;
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Relational_Expr_Ast::get_data_type()
{
	return node_data_type;
}

bool Relational_Expr_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Relational statement data type not compatible", line);
}

void Relational_Expr_Ast::print_ast(ostream & file_buffer)
{
	file_buffer <<"\n"<<COND_SPACE << "Condition: " << rel_operators_map[rel_op] <<endl;

	file_buffer << COND_NODE_SPACE"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << COND_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Register_Enum Relational_Expr_Ast::print_icode(ostream & file_buffer){
    Register_Enum lr = lhs->print_icode(file_buffer);
    Register_Enum rr = rhs->print_icode(file_buffer);
    Register_Enum dest = cpu_gpr.get_first_icode_free();

    file_buffer << " " << rel_op_instr_map[rel_op] <<": " << reg_str_map[dest] << " <- ";
    file_buffer << reg_str_map[lr] << " , " << reg_str_map[rr] <<endl;
    cpu_gpr.free(lr);
    cpu_gpr.free(rr);
    return dest;
}

int Relational_Expr_Ast::compare(int x, int y){
    //file_buffer << "operator is " << rel_op << "X is " << x << " Y is "<<y <<endl;
    if(rel_op == 0) return (x < y);
    if(rel_op == 1) return (x > y);
    if(rel_op == 2) return (x >= y);
    if(rel_op == 3) return (x <= y);
    if(rel_op == 4) return (x != y);
    if(rel_op == 5) return (x == y);
}

Eval_Result & Relational_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & rhs_result = rhs->evaluate(eval_env, file_buffer);

	if (rhs_result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	Eval_Result & lhs_result = lhs->evaluate(eval_env, file_buffer);

	if (lhs_result.is_variable_defined() == false)
		report_error("Variable should be defined to be on lhs", NOLINE);


	// Print the result
    Eval_Result & result = *new Eval_Result_Value_Int();
    int compare_result = this->compare(lhs_result.get_value(), rhs_result.get_value());
    //file_buffer << "Compare result is "<< compare_result <<endl;
    result.set_value(compare_result);

	return result;
}

//////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry)
{
	variable_name = name;
	variable_symbol_entry = var_entry;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry.get_data_type();
}

void Name_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_name;
}

Register_Enum Name_Ast::print_icode(ostream & file_buffer)
{
    Register_Enum dest = cpu_gpr.get_first_icode_free();
    file_buffer << " load:  " << reg_str_map[dest] << " <- " << variable_name << endl;
    return dest;
}

void Name_Ast::print_self(ostream & file_buffer)
{
    file_buffer << variable_name;
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result_Value * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result_Value * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer  << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		if (loc_var_val->get_result_enum() == int_result)
			file_buffer << loc_var_val->get_value() << "\n";
		else
			report_internal_error("Result type can only be int and float");
	}

	else
	{
		if (glob_var_val->get_result_enum() == int_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << glob_var_val->get_value() << "\n";
		}
		else
			report_internal_error("Result type can only be int and float");
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	if (eval_env.does_variable_exist(variable_name))
	{
		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result_Value * i;
	if (result.get_result_enum() == int_result)
	{
		i = new Eval_Result_Value_Int();
	 	i->set_value(result.get_value());
	}

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type)
{
	constant = number;
	node_data_type = constant_data_type;
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print_ast(ostream & file_buffer)
{
	file_buffer << "Num : " << constant;
}

template <class DATA_TYPE>
Register_Enum Number_Ast<DATA_TYPE>::print_icode(ostream & file_buffer)
{
    Register_Enum dest = cpu_gpr.get_first_icode_free();
    file_buffer << " iLoad: " << reg_str_map[dest] << " <- " << constant << endl;
    return dest;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(constant);

		return result;
	}
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast()
{
    node_data_type = return_data_type;
}

Return_Ast::~Return_Ast()
{}

Data_Type Return_Ast::get_data_type()
{
	return node_data_type;
}

void Return_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Return <NOTHING>\n";
}

Register_Enum Return_Ast::print_icode(ostream & file_buffer)
{
    return zero;
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Return <NOTHING>\n";
	Eval_Result & result = *new Eval_Result_Value_Int();
    result.set_result_enum(return_result); //set the type to go_to_result enum
	return result;
}
template class Number_Ast<int>;

///////////////////////////////////////////////////////////////////////////////
Goto_Ast::Goto_Ast(int succ)
{
    successor = succ;
}

Goto_Ast::~Goto_Ast()
{}

void Goto_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Goto statement:\n";
	file_buffer << AST_NODE_SPACE"Successor: "<< successor <<endl;
}

Register_Enum Goto_Ast::print_icode(ostream & file_buffer)
{
    file_buffer << " goto label" << successor <<endl;
    return zero;
}

int Goto_Ast::get_successor(){
    return successor;
}

Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
    print_ast(file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();
    result.set_result_enum(go_to_result); //set the type to go_to_result enum
    result.set_value(successor);
	file_buffer << AST_SPACE << "GOTO (BB "<< successor <<")"<<"\n";
	return result;
}

/////////////////////////////////////////////////////////////////////////////////

If_Ast::If_Ast(Ast * temp_condition, Ast * temp_goto_true, Ast* temp_goto_false)
{
	condition = temp_condition;
	goto_true = temp_goto_true;
	goto_false= temp_goto_false;
}

If_Ast::~If_Ast()
{}

void If_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "If_Else statement:";
    condition->print_ast(file_buffer);
    file_buffer <<endl; //this is required because rel_expr->print() doesn't end in newline
    file_buffer << AST_NODE_SPACE << "True Successor: "<< ((Goto_Ast*)goto_true)->get_successor() <<endl;
    file_buffer << AST_NODE_SPACE << "False Successor: "<< ((Goto_Ast*)goto_false)->get_successor() <<endl;
}
Register_Enum If_Ast::print_icode(ostream & file_buffer)
{
    Register_Enum dest = condition->print_icode(file_buffer);
    file_buffer << " bne: " << reg_str_map[dest] <<" , " << reg_str_map[zero] << " :";
    goto_true->print_icode(file_buffer);
    goto_false->print_icode(file_buffer);
    return zero;
}

Eval_Result & If_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{

	Eval_Result & result = *new Eval_Result_Value_Int();
    int successor;

    print_ast(file_buffer);

	Eval_Result & condition_result = condition->evaluate(eval_env, file_buffer);
    int cond_result_value = condition_result.get_value();
    if(cond_result_value == 1){ //true statment
        successor =((Goto_Ast*)goto_true)->get_successor();
        file_buffer << AST_SPACE << "Condition True : Goto (BB "<< successor << ")"<<endl;
    }
    else{ //false statment
        successor =((Goto_Ast*)goto_false)->get_successor();
        file_buffer << AST_SPACE << "Condition False : Goto (BB "<< successor << ")"<<endl;
    }
    
    result.set_result_enum(go_to_result); //set the type to go_to_result enum
    result.set_value(successor);

}
