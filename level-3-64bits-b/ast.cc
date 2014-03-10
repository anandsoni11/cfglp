
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

void Ast::set_data_type(Data_Type type)
{
    report_internal_error("Should not reach, Ast : set_data_type");
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

void Assignment_Ast::set_data_type(Data_Type type)
{
    node_data_type = type;
}

Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type;
}

bool Assignment_Ast::check_ast(int line)
{
	if (is_compatible(lhs->get_data_type(), rhs->get_data_type()))
	{
		node_data_type = rhs->get_data_type();
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}

void Assignment_Ast::print_ast(ostream & file_buffer)
{
    file_buffer << endl;
	file_buffer << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")\n";
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

    result.set_result_flag(normal_flag); 
	return result;
}

Eval_Result & Assignment_Ast::evaluate_silently(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	if (result.is_variable_defined() == false)
		report_error("Passing Arguments :: Variable should be defined", NOLINE);

	lhs->set_value_of_evaluation(eval_env, result);

    result.set_result_flag(normal_flag); 
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

void Relational_Expr_Ast::set_data_type(Data_Type type)
{
    node_data_type = type;
}
bool Relational_Expr_Ast::check_ast(int line)
{
	if (is_compatible(lhs->get_data_type(), rhs->get_data_type()))
	{
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

int Relational_Expr_Ast::compare(Value_Bundle x, Value_Bundle y, Result_Enum res_enum){
    //file_buffer << "operator is " << rel_op << "X is " << x << " Y is "<<y <<endl;
    if(res_enum == int_result){
        if(rel_op == 0) return (x.int_v <  y.int_v);
        if(rel_op == 1) return (x.int_v >  y.int_v);
        if(rel_op == 2) return (x.int_v >= y.int_v);
        if(rel_op == 3) return (x.int_v <= y.int_v);
        if(rel_op == 4) return (x.int_v != y.int_v);
        if(rel_op == 5) return (x.int_v == y.int_v);
    }
    else if(res_enum == float_result || res_enum == double_result){
        if(rel_op == 0) return (x.float_v <  y.float_v);
        if(rel_op == 1) return (x.float_v >  y.float_v);
        if(rel_op == 2) return (x.float_v >= y.float_v);
        if(rel_op == 3) return (x.float_v <= y.float_v);
        if(rel_op == 4) return (x.float_v != y.float_v);
        if(rel_op == 5) return (x.float_v == y.float_v);
    }
}

Eval_Result & Relational_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & lhs_result = lhs->evaluate(eval_env, file_buffer);

	if (lhs_result.is_variable_defined() == false)
		report_error("Variable should be defined to be on lhs", NOLINE);

	Eval_Result & rhs_result = rhs->evaluate(eval_env, file_buffer);

	if (rhs_result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);



    int compare_result = this->compare(lhs_result.get_value(), rhs_result.get_value(), lhs_result.get_result_enum());
    //file_buffer << "Compare result is "<< compare_result <<endl;
	// Print the result
    
    // identify the return type for this Ast(maybe typecasted) and return Eval_Result accordingly of correct type
    if(node_data_type == int_data_type){
        Eval_Result & result = *new Eval_Result_Value_Int();
        Value_Bundle bundle;
        bundle.int_v = (int) compare_result;
        result.set_value(bundle);
        result.set_result_flag(normal_flag); 
	    return result;
    }
    else if(node_data_type == float_data_type){
        Eval_Result & result = *new Eval_Result_Value_Float();
        Value_Bundle bundle;
        bundle.float_v = (float) compare_result;
        result.set_value(bundle);
        result.set_result_flag(normal_flag); 
	    return result;
    }
    else if(node_data_type == double_data_type){
        Eval_Result & result = *new Eval_Result_Value_Double();
        Value_Bundle bundle;
        bundle.float_v = (float) compare_result;
        result.set_value(bundle);
        result.set_result_flag(normal_flag); 
	    return result;
    }
}

///////////////////////////////////////////////////////////////////////////

Arithmetic_Expr_Ast::Arithmetic_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs, int temp_op)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
    arith_op = temp_op;
}

Arithmetic_Expr_Ast::~Arithmetic_Expr_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Arithmetic_Expr_Ast::get_data_type()
{
	return node_data_type;
}

void Arithmetic_Expr_Ast::set_data_type(Data_Type type)
{
    node_data_type = type;
}
bool Arithmetic_Expr_Ast::check_ast(int line)
{
	if (lhs==NULL || is_compatible(lhs->get_data_type(), rhs->get_data_type()))
	{
        node_data_type = rhs->get_data_type();
		return true;
	}
	report_error("Arithmetic statement data type not compatible", line);
}

void Arithmetic_Expr_Ast::print_ast(ostream & file_buffer)
{
	file_buffer <<"\n"<<COND_SPACE << "Arith: " << arith_operators_map[arith_op] <<endl;

    if(arith_op == 0){
        file_buffer << COND_NODE_SPACE"LHS (";
        rhs->print_ast(file_buffer);
        file_buffer << ")";
    }
    else{
        file_buffer << COND_NODE_SPACE"LHS (";
        lhs->print_ast(file_buffer);
        file_buffer << ")\n";

        file_buffer << COND_NODE_SPACE << "RHS (";
        rhs->print_ast(file_buffer);
        file_buffer << ")";
    }
}

Value_Bundle Arithmetic_Expr_Ast::calculate(Value_Bundle x, Value_Bundle y, Result_Enum res_enum){
    Value_Bundle bundle;
    if(res_enum == int_result){
        if     (arith_op == 0)   bundle.int_v = -(y.int_v);
        else if(arith_op == 1)   bundle.int_v =  x.int_v + y.int_v;
        else if(arith_op == 2)   bundle.int_v =  x.int_v - y.int_v;
        else if(arith_op == 3)   bundle.int_v =  x.int_v * y.int_v;
        else if(arith_op == 4)   bundle.int_v =  x.int_v / y.int_v;
    }
    else if(res_enum == float_result || res_enum == double_result){
        if     (arith_op == 0)   bundle.float_v = -(y.float_v);
        else if(arith_op == 1)   bundle.float_v =  x.float_v + y.float_v;
        else if(arith_op == 2)   bundle.float_v =  x.float_v - y.float_v;
        else if(arith_op == 3)   bundle.float_v =  x.float_v * y.float_v;
        else if(arith_op == 4)   bundle.float_v =  x.float_v / y.float_v;
    }
    return bundle;
}

Eval_Result & Arithmetic_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & rhs_result = rhs->evaluate(eval_env, file_buffer);

	if (rhs_result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);


    Value_Bundle calculated_result;

    Result_Enum operand_result_enum = rhs_result.get_result_enum();

    if(lhs == NULL){
        Value_Bundle dummy_lhs_res_value;
        calculated_result = this->calculate(dummy_lhs_res_value, rhs_result.get_value(), operand_result_enum);
    }
    else{
        Eval_Result & lhs_result = lhs->evaluate(eval_env, file_buffer);

        if (lhs_result.is_variable_defined() == false)
            report_error("Variable should be defined to be on lhs", NOLINE);


        calculated_result = this->calculate(lhs_result.get_value(), rhs_result.get_value(), operand_result_enum);
    }
    //Here we have the result's bundle

    if(node_data_type == int_data_type){
        Eval_Result & result = *new Eval_Result_Value_Int();
        Value_Bundle bundle;

        if     (operand_result_enum == int_result)      bundle.int_v = (int) calculated_result.int_v;
        else if(operand_result_enum == float_result)    bundle.int_v = (int) calculated_result.float_v;
        else if(operand_result_enum == double_result)   bundle.int_v = (int) calculated_result.float_v;
        result.set_value(bundle);
        result.set_result_flag(normal_flag); 
	    return result;
    }
    else if(node_data_type == float_data_type){
        Eval_Result & result = *new Eval_Result_Value_Float();
        Value_Bundle bundle;

        if     (operand_result_enum == int_result)      bundle.float_v = (float) calculated_result.int_v;
        else if(operand_result_enum == float_result)    bundle.float_v = (float) calculated_result.float_v;
        else if(operand_result_enum == double_result)   bundle.float_v = (float) calculated_result.float_v;
        result.set_value(bundle);
        result.set_result_flag(normal_flag); 
	    return result;
    }
    else if(node_data_type == double_data_type){
        Eval_Result & result = *new Eval_Result_Value_Double();
        Value_Bundle bundle;

        if     (operand_result_enum == int_result)      bundle.float_v = (double) calculated_result.int_v;
        else if(operand_result_enum == float_result)    bundle.float_v = (double) calculated_result.float_v;
        else if(operand_result_enum == double_result)   bundle.float_v = (double) calculated_result.float_v;
        result.set_value(bundle);
        result.set_result_flag(normal_flag); 
	    return result;
    }
}
///////////////////////////////////////////////////////////////////////////

Function_Call_Ast::Function_Call_Ast(string temp_name, list<Ast*> temp_args, Procedure *p)
{
    name = temp_name;
    args = temp_args;
    proc = p;
}

Function_Call_Ast::~Function_Call_Ast()
{
}

Data_Type Function_Call_Ast::get_data_type()
{
	return node_data_type;
}

void Function_Call_Ast::set_data_type(Data_Type type)
{
    node_data_type = type;
}

bool Function_Call_Ast::check_ast(int line)
{
    if(proc == NULL){
        node_data_type = int_data_type; //CHECK
        return false;
    }
    node_data_type = proc->get_return_type();
    if(! proc->validate_arg_list(args, line)) return false;

    return true;
}

void Function_Call_Ast::print_ast(ostream & file_buffer)
{
    file_buffer << endl;
	file_buffer << AST_SPACE << "FN CALL: " << name <<"(";
    list<Ast*>::iterator itr;
    for(itr=args.begin(); itr!=args.end(); itr++){
        file_buffer << endl << AST_NODE_SPACE;
        (*itr)->print_ast(file_buffer);
    }
	file_buffer << ")";
}

Eval_Result_Value * Function_Call_Ast::convert_to_value(Eval_Result & result){
	Eval_Result_Value * i;
	if (result.get_result_enum() == int_result)
	{
		i = new Eval_Result_Value_Int();
        Value_Bundle bundle = result.get_value();
	 	i->set_value(bundle);
	}
    else if (result.get_result_enum() == float_result)
	{
		i = new Eval_Result_Value_Float();
        Value_Bundle bundle = result.get_value();
	 	i->set_value(bundle);
	}
    else if (result.get_result_enum() == double_result)
	{
		i = new Eval_Result_Value_Double();
        Value_Bundle bundle = result.get_value();
	 	i->set_value(bundle);
	}
    return i;
}

Eval_Result & Function_Call_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
    list<Eval_Result_Value*> arguments;


    list<Ast*>::iterator arg_it = args.begin();
    for(; arg_it != args.end(); arg_it++){
        Eval_Result & value = (*arg_it)->evaluate(eval_env, file_buffer);
        Eval_Result_Value * i = convert_to_value(value);
        arguments.push_back(i);
    }

    Eval_Result& result = proc->evaluate(file_buffer, arguments);

    if(node_data_type == int_data_type){
        Eval_Result & new_result = *new Eval_Result_Value_Int();
        Value_Bundle bundle;

        if     (result.get_result_enum() == int_result)      bundle.int_v = (int) result.get_value().int_v;
        else if(result.get_result_enum() == float_result)    bundle.int_v = (int) result.get_value().float_v;
        else if(result.get_result_enum() == double_result)   bundle.int_v = (int) result.get_value().float_v;
        new_result.set_value(bundle);
        new_result.set_result_flag(normal_flag); 
	    return new_result;
    }
    else if(node_data_type == float_data_type){
        Eval_Result & new_result = *new Eval_Result_Value_Float();
        Value_Bundle bundle;

        if     (result.get_result_enum() == int_result)      bundle.float_v = (float) result.get_value().int_v;
        else if(result.get_result_enum() == float_result)    bundle.float_v = (float) result.get_value().float_v;
        else if(result.get_result_enum() == double_result)   bundle.float_v = (float) result.get_value().float_v;
        new_result.set_value(bundle);
        new_result.set_result_flag(normal_flag); 
	    return new_result;
    }
    else if(node_data_type == double_data_type){
        Eval_Result & new_result = *new Eval_Result_Value_Double();
        Value_Bundle bundle;

        if     (result.get_result_enum() == int_result)      bundle.float_v = (double) result.get_value().int_v;
        else if(result.get_result_enum() == float_result)    bundle.float_v = (double) result.get_value().float_v;
        else if(result.get_result_enum() == double_result)   bundle.float_v = (double) result.get_value().float_v;
        new_result.set_value(bundle);
        new_result.set_result_flag(normal_flag); 
	    return new_result;
    }
    else{//void data type
        result.set_result_flag(normal_flag); 
	    return result;
    }
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

void Name_Ast::set_data_type(Data_Type type)
{
    variable_symbol_entry.set_data_type(type);
}

void Name_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_name;
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
		if (loc_var_val->get_result_enum() == int_result || 
            loc_var_val->get_result_enum() == float_result ||
            loc_var_val->get_result_enum() == double_result)  {

            loc_var_val->print(file_buffer);
			file_buffer << "\n";
        }
		else
			report_internal_error("Result type can only be int and float");
	}

	else
	{
		if (glob_var_val->get_result_enum() == int_result || 
            glob_var_val->get_result_enum() == float_result ||
            glob_var_val->get_result_enum() == double_result)  {

			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else{
                glob_var_val->print(file_buffer);
                file_buffer << "\n";
            }
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
        Value_Bundle bundle = result.get_value();
	 	i->set_value(bundle);
	}
    else if (result.get_result_enum() == float_result)
	{
		i = new Eval_Result_Value_Float();
        Value_Bundle bundle = result.get_value();
	 	i->set_value(bundle);
	}
    else if (result.get_result_enum() == double_result)
	{
		i = new Eval_Result_Value_Double();
        Value_Bundle bundle = result.get_value();
	 	i->set_value(bundle);
	}

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
    Eval_Result &result = get_value_of_evaluation(eval_env);

    //cout << "here out" << endl;
    if(get_data_type() == int_data_type){
        Eval_Result & new_result = *new Eval_Result_Value_Int();
        Value_Bundle bundle;

        if     (result.get_result_enum() == int_result)      bundle.int_v = (int) result.get_value().int_v;
        else if(result.get_result_enum() == float_result)    bundle.int_v = (int) result.get_value().float_v;
        else if(result.get_result_enum() == double_result)   bundle.int_v = (int) result.get_value().float_v;
        new_result.set_value(bundle);
        new_result.set_result_flag(normal_flag); 
	    return new_result;
    }
    else if(get_data_type() == float_data_type){
        Eval_Result & new_result = *new Eval_Result_Value_Float();
        Value_Bundle bundle;

        if     (result.get_result_enum() == int_result)      bundle.float_v = (float) result.get_value().int_v;
        else if(result.get_result_enum() == float_result)    bundle.float_v = (float) result.get_value().float_v;
        else if(result.get_result_enum() == double_result)   bundle.float_v = (float) result.get_value().float_v;
        new_result.set_value(bundle);
        new_result.set_result_flag(normal_flag); 
	    return new_result;
    }
    else if(get_data_type() == double_data_type){
        Eval_Result & new_result = *new Eval_Result_Value_Double();
        Value_Bundle bundle;

        if     (result.get_result_enum() == int_result)      bundle.float_v = (double) result.get_value().int_v;
        else if(result.get_result_enum() == float_result)    bundle.float_v = (double) result.get_value().float_v;
        else if(result.get_result_enum() == double_result)   bundle.float_v = (double) result.get_value().float_v;
        new_result.set_value(bundle);
        new_result.set_result_flag(normal_flag); 
	    return new_result;
    }
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(Ast * expr)
{
    expression = expr;
    if(expression == NULL){
        node_data_type = void_data_type;
    }
    else{
        node_data_type = expression->get_data_type();
    }
}

Return_Ast::~Return_Ast()
{}

Data_Type Return_Ast::get_data_type()
{
	return node_data_type;
}

void Return_Ast::print_ast(ostream & file_buffer)
{
    file_buffer << endl;
	if(expression == NULL) {
        file_buffer << AST_SPACE << "RETURN <NOTHING>";
        file_buffer << endl << endl;
    }
    else{
        file_buffer << AST_SPACE << "RETURN ";
        expression->print_ast(file_buffer);
        file_buffer << endl << endl;
    }
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
    print_ast(file_buffer);

    if(expression == NULL){
        Eval_Result & result = * new Eval_Result_Value_Int();
        result.set_result_enum(void_result);
        result.set_result_flag(return_flag);
        return result;
    }
	Eval_Result &result = expression->evaluate(eval_env, file_buffer);
	//file_buffer << AST_SPACE << "Return <NOTHING>\n";
    //Set flag "RETURN"
    result.set_result_flag(return_flag); 
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

Data_Type Goto_Ast::get_data_type()
{
	return node_data_type;
}
void Goto_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Goto statement:\n";
	file_buffer << AST_NODE_SPACE"Successor: "<< successor <<endl;
}

int Goto_Ast::get_successor(){
    return successor;
}

Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
    print_ast(file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();

    Value_Bundle bundle;
    bundle.int_v = successor;
    result.set_value(bundle);

	file_buffer << AST_SPACE << "GOTO (BB "<< successor <<")"<<"\n";
    result.set_result_flag(go_to_flag); 
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

Eval_Result & If_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{

	Eval_Result & result = *new Eval_Result_Value_Int();
    int successor;

    print_ast(file_buffer);

	Eval_Result & condition_result = condition->evaluate(eval_env, file_buffer);
    Value_Bundle cond_result_value = condition_result.get_value();
    if(cond_result_value.int_v == 1){ //true statment
        successor =((Goto_Ast*)goto_true)->get_successor();
        file_buffer << AST_SPACE << "Condition True : Goto (BB "<< successor << ")"<<endl;
    }
    else{ //false statment
        successor =((Goto_Ast*)goto_false)->get_successor();
        file_buffer << AST_SPACE << "Condition False : Goto (BB "<< successor << ")"<<endl;
    }
    

    Value_Bundle bundle;
    bundle.int_v = successor;
    result.set_value(bundle);

    result.set_result_flag(go_to_flag); 
    return result;
}
///////////////// UTILS DEFINITION /////////////////////////
//
bool is_compatible(Data_Type d1, Data_Type d2){
    if(d1 == d2) return true;
    if( (d1 == double_data_type && d2 == float_data_type) || (d2 == double_data_type && d1 == float_data_type))
        return true;
    return false;
}

