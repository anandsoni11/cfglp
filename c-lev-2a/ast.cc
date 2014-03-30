
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
#include<typeinfo>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"local-environment.hh"
#include"icode.hh"
#include"reg-alloc.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Data_Type Ast::get_data_type()
{
    return node_data_type;
    /*
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
    */
}

void Ast::set_data_type(Data_Type dt){
	node_data_type = dt;
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	stringstream msg;
	msg << "No print_value() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	stringstream msg;
	msg << "No get_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	stringstream msg;
	msg << "No set_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;

	ast_num_child = binary_arity;
	node_data_type = void_data_type;

	lineno = line;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

bool Assignment_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	if ( is_compatible(lhs->get_data_type(), rhs->get_data_type()))
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible");
}

void Assignment_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Asgn:";

	file_buffer << "\n" << AST_NODE_SPACE << "LHS (";
	lhs->print(file_buffer);
	file_buffer << ")";

	file_buffer << "\n" << AST_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	CHECK_INPUT_AND_ABORT(result.is_variable_defined(), "Variable should be defined to be on rhs of Assignment_Ast", lineno);

	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print(file_buffer);

	lhs->print_value(eval_env, file_buffer);

	return result;
}

Code_For_Ast & Assignment_Ast::compile()
{
	/* 
		An assignment x = y where y is a variable is 
		compiled as a combination of load and store statements:
		(load) R <- y 
		(store) x <- R
		If y is a constant, the statement is compiled as:
		(imm_Load) R <- y 
		(store) x <- R
		where imm_Load denotes the load immediate operation.
	*/

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();

	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

	//free the register holding rhs expression
	load_register->reset_used_for_expr_result();
	
	return *assign_stmt;
}

Code_For_Ast & Assignment_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Register_Descriptor * result_register;
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	if(typeid(*rhs) == typeid(Number_Ast<int>) || typeid(*rhs) == typeid(Name_Ast)){
		lra.optimize_lra(mc_2m, lhs, rhs);
	}
	else{ //rhs is rel expression
	}
	
	Code_For_Ast load_stmt = rhs->compile_and_optimize_ast(lra);
	result_register= load_stmt.get_reg();
	Code_For_Ast store_stmt = lhs->create_store_stmt(result_register);

	/* update the reg alloc */
	Symbol_Table_Entry* destination_symbol_entry = &(lhs->get_symbol_entry());
	Register_Descriptor * destination_register = destination_symbol_entry->get_register(); 

	if (destination_register)
		destination_symbol_entry->free_register(destination_register); 
	destination_symbol_entry->update_register(result_register);
	
	
	list<Icode_Stmt *> ic_list;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);
		
	//free the register holding rhs expression
	result_register->reset_used_for_expr_result();

	return *assign_stmt;
}

////////////////////////////////////////////////////////////////

Relational_Expr_Ast::Relational_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs, int temp_op, int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;

	node_data_type = int_data_type;
	ast_num_child = binary_arity;
	rel_op = temp_op;
	lineno = line;
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
	delete lhs;
	delete rhs;
}

bool Relational_Expr_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Expression cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Expression cannot be null");

	if (is_compatible(lhs->get_data_type(), rhs->get_data_type()))
	{
		return true;
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Expression statement data type not compatible");
}

void Relational_Expr_Ast::print(ostream & file_buffer)
{
	file_buffer <<"\n"<<COND_SPACE << "Condition: " << rel_operators_map[rel_op] <<endl;

	file_buffer << COND_NODE_SPACE"LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	file_buffer << COND_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
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
	CHECK_INVARIANT((rhs != NULL), "Rhs of Expression cannot be null");
	Eval_Result & rhs_result = rhs->evaluate(eval_env, file_buffer);

	CHECK_INPUT_AND_ABORT(rhs_result.is_variable_defined(), "Variable should be defined to be on rhs of Expression Ast", lineno);

	CHECK_INVARIANT((lhs != NULL), "lhs of Expression cannot be null");
	Eval_Result & lhs_result = lhs->evaluate(eval_env, file_buffer);

	CHECK_INPUT_AND_ABORT(lhs_result.is_variable_defined(), "Variable should be defined to be on lhs of Expression Ast", lineno);

	// Print the result

	Eval_Result & result = *new Eval_Result_Value_Int();
    int compare_result = this->compare(lhs_result.get_int_value(), rhs_result.get_int_value());
    //file_buffer << "Compare result is "<< compare_result <<endl;
    result.set_value(compare_result);

	return result;
}

Code_For_Ast & Relational_Expr_Ast::compile()
{
	/* 
	*/

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_lhs_stmt = lhs->compile();
	Code_For_Ast & load_rhs_stmt = rhs->compile();


	Register_Descriptor * lhs_register = load_lhs_stmt.get_reg();
	Register_Descriptor * rhs_register = load_rhs_stmt.get_reg();

	Ics_Opd * opd1 = new Register_Addr_Opd(lhs_register);
	Ics_Opd * opd2 = new Register_Addr_Opd(rhs_register);

	Register_Val_Type reg_type = float_num;
	if(node_data_type == int_data_type) reg_type = int_num;
	
	Register_Descriptor * result_register = machine_dscr_object.get_new_register(reg_type); //to hold the result of comparistion
	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * compare_stmt = new Compute_IC_Stmt(rel_tgtop_map[rel_op], opd1, opd2, result_opd);


	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_lhs_stmt.get_icode_list().empty() == false)
		ic_list = load_lhs_stmt.get_icode_list();
	if (load_rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), load_rhs_stmt.get_icode_list());
    //now list contains lhs & rhs stmts, now insert the compare_stmt created above
    ic_list.push_back(compare_stmt);

	Code_For_Ast * rel_stmt;
	if (ic_list.empty() == false)
		rel_stmt = new Code_For_Ast(ic_list, result_register);

	//free lhs & rhs register descriptors
	lhs_register->reset_used_for_expr_result();
	rhs_register->reset_used_for_expr_result();

	return *rel_stmt;
}

Code_For_Ast & Relational_Expr_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
    
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");
	
	Lra_Outcome lra_lhs, lra_rhs;

	if(typeid(*lhs) == typeid(Number_Ast<int>)){
		lra_lhs.optimize_lra(c_2r, NULL, lhs);
	}
	else if(typeid(*lhs) == typeid(Name_Ast)){
		lra_lhs.optimize_lra(m_2r, NULL, lhs);
	}
	Code_For_Ast & load_lhs_stmt = lhs->compile_and_optimize_ast(lra_lhs);
	
	
	if(typeid(*rhs) == typeid(Number_Ast<int>)){
		lra_rhs.optimize_lra(c_2r, NULL, rhs);
	}
	else if(typeid(*rhs) == typeid(Name_Ast)){
		lra_rhs.optimize_lra(m_2r, NULL, rhs);
	}	
	Code_For_Ast & load_rhs_stmt = rhs->compile_and_optimize_ast(lra_rhs);

	Register_Descriptor * lhs_register = load_lhs_stmt.get_reg();
	Register_Descriptor * rhs_register = load_rhs_stmt.get_reg();

	Ics_Opd * opd1 = new Register_Addr_Opd(lhs_register);
	Ics_Opd * opd2 = new Register_Addr_Opd(rhs_register);
	
	Register_Val_Type reg_type = float_num;
	if(node_data_type == int_data_type) reg_type = int_num;
	
	Register_Descriptor * result_register = machine_dscr_object.get_new_register(reg_type); //to hold the result of comparistion
	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * compare_stmt = new Compute_IC_Stmt(rel_tgtop_map[rel_op], opd1, opd2, result_opd);


	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_lhs_stmt.get_icode_list().empty() == false)
		ic_list = load_lhs_stmt.get_icode_list();
	if (load_rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), load_rhs_stmt.get_icode_list());
    //now list contains lhs & rhs stmts, now insert the compare_stmt created above
    ic_list.push_back(compare_stmt);

	Code_For_Ast * rel_stmt;
	if (ic_list.empty() == false)
		rel_stmt = new Code_For_Ast(ic_list, result_register);

	//free lhs & rhs register descriptors
	lhs_register->reset_used_for_expr_result();
	rhs_register->reset_used_for_expr_result();

	return *rel_stmt;
}

///////////////////////////////////////////////////////////////////////////

Arithmetic_Expr_Ast::Arithmetic_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs, int temp_op, int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
    arith_op = temp_op;
    
    ast_num_child = binary_arity;
    lineno = line;
}

Arithmetic_Expr_Ast::~Arithmetic_Expr_Ast()
{
	delete lhs;
	delete rhs;
}

bool Arithmetic_Expr_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Expression cannot be null");

	if (lhs==NULL || is_compatible(lhs->get_data_type(), rhs->get_data_type()))
	{
        node_data_type = rhs->get_data_type();
		return true;
	}
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Expression statement data type not compatible");
}

void Arithmetic_Expr_Ast::print(ostream & file_buffer)
{
	file_buffer <<"\n"<<COND_SPACE << "Arith: " << arith_operators_map[arith_op] <<endl;

    if(arith_op == 0){
        file_buffer << COND_NODE_SPACE"LHS (";
        rhs->print(file_buffer);
        file_buffer << ")";
    }
    else{
        file_buffer << COND_NODE_SPACE"LHS (";
        lhs->print(file_buffer);
        file_buffer << ")\n";

        file_buffer << COND_NODE_SPACE << "RHS (";
        rhs->print(file_buffer);
        file_buffer << ")";
    }
}

Eval_Result & Arithmetic_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	/*
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
	    return result;
    }
    else if(node_data_type == float_data_type){
        Eval_Result & result = *new Eval_Result_Value_Float();
        Value_Bundle bundle;

        if     (operand_result_enum == int_result)      bundle.float_v = (float) calculated_result.int_v;
        else if(operand_result_enum == float_result)    bundle.float_v = (float) calculated_result.float_v;
        else if(operand_result_enum == double_result)   bundle.float_v = (float) calculated_result.float_v;
        result.set_value(bundle);
	    return result;
    }
    else if(node_data_type == double_data_type){
        Eval_Result & result = *new Eval_Result_Value_Double();
        Value_Bundle bundle;

        if     (operand_result_enum == int_result)      bundle.float_v = (double) calculated_result.int_v;
        else if(operand_result_enum == float_result)    bundle.float_v = (double) calculated_result.float_v;
        else if(operand_result_enum == double_result)   bundle.float_v = (double) calculated_result.float_v;
        result.set_value(bundle);
	    return result;
    }
    */
}

Code_For_Ast & Arithmetic_Expr_Ast::compile()
{
	
	if(arith_op==0) {//unary
		CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");
		Code_For_Ast & load_rhs_stmt = rhs->compile();
		Register_Descriptor * rhs_register = load_rhs_stmt.get_reg();
		Ics_Opd * opd1 = new Register_Addr_Opd(rhs_register);
		Ics_Opd * opd2 = NULL; //dummy second operand
		
		Register_Val_Type reg_type = float_num;
		if(node_data_type == int_data_type) reg_type = int_num;
	
		Register_Descriptor * result_register = machine_dscr_object.get_new_register(reg_type); //to hold the result of 
		CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
		Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
		Icode_Stmt * compare_stmt;
		
		if(node_data_type == int_data_type)
			compare_stmt = new Compute_IC_Stmt(op_uminus, opd1, opd2, result_opd);
		else
			compare_stmt = new Compute_IC_Stmt(op_uminus_d, opd1, opd2, result_opd);
			
		// Store the statement in ic_list

		list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

		if (load_rhs_stmt.get_icode_list().empty() == false)
			ic_list = load_rhs_stmt.get_icode_list();
		
		ic_list.push_back(compare_stmt);
		
		Code_For_Ast * rel_stmt;
		if (ic_list.empty() == false)
			rel_stmt = new Code_For_Ast(ic_list, result_register);

		//free lhs & rhs register descriptors
		rhs_register->reset_used_for_expr_result();
		return *rel_stmt;
	}
	
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_lhs_stmt = lhs->compile();
	Code_For_Ast & load_rhs_stmt = rhs->compile();

	Register_Descriptor * lhs_register = load_lhs_stmt.get_reg();
	Register_Descriptor * rhs_register = load_rhs_stmt.get_reg();

	Ics_Opd * opd1 = new Register_Addr_Opd(lhs_register);
	Ics_Opd * opd2 = new Register_Addr_Opd(rhs_register);

	Register_Val_Type reg_type = float_num;
	if(node_data_type == int_data_type) reg_type = int_num;
	
	Register_Descriptor * result_register = machine_dscr_object.get_new_register(reg_type); //to hold the result of 
	
	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * compare_stmt;
	if(node_data_type == int_data_type)
		compare_stmt = new Compute_IC_Stmt(arith_tgtop_map[arith_op], opd1, opd2, result_opd);
	else
		compare_stmt = new Compute_IC_Stmt(arith_tgtop_map_double[arith_op], opd1, opd2, result_opd);


	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_lhs_stmt.get_icode_list().empty() == false)
		ic_list = load_lhs_stmt.get_icode_list();
	if (load_rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), load_rhs_stmt.get_icode_list());
    //now list contains lhs & rhs stmts, now insert the compare_stmt created above
    ic_list.push_back(compare_stmt);

	Code_For_Ast * rel_stmt;
	if (ic_list.empty() == false)
		rel_stmt = new Code_For_Ast(ic_list, result_register);

	//free lhs & rhs register descriptors
	lhs_register->reset_used_for_expr_result();
	rhs_register->reset_used_for_expr_result();

	return *rel_stmt;

}

Code_For_Ast & Arithmetic_Expr_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	/*
    
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");
	
	Lra_Outcome lra_lhs, lra_rhs;

	if(typeid(*lhs) == typeid(Number_Ast<int>)){
		lra_lhs.optimize_lra(c_2r, NULL, lhs);
	}
	else if(typeid(*lhs) == typeid(Name_Ast)){
		lra_lhs.optimize_lra(m_2r, NULL, lhs);
	}
	Code_For_Ast & load_lhs_stmt = lhs->compile_and_optimize_ast(lra_lhs);
	
	
	if(typeid(*rhs) == typeid(Number_Ast<int>)){
		lra_rhs.optimize_lra(c_2r, NULL, rhs);
	}
	else if(typeid(*rhs) == typeid(Name_Ast)){
		lra_rhs.optimize_lra(m_2r, NULL, rhs);
	}	
	Code_For_Ast & load_rhs_stmt = rhs->compile_and_optimize_ast(lra_rhs);

	Register_Descriptor * lhs_register = load_lhs_stmt.get_reg();
	Register_Descriptor * rhs_register = load_rhs_stmt.get_reg();

	Ics_Opd * opd1 = new Register_Addr_Opd(lhs_register);
	Ics_Opd * opd2 = new Register_Addr_Opd(rhs_register);
	
	Register_Descriptor * result_register = machine_dscr_object.get_new_register(); //to hold the result of comparistion
	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * compare_stmt = new Compute_IC_Stmt(rel_tgtop_map[rel_op], opd1, opd2, result_opd);


	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_lhs_stmt.get_icode_list().empty() == false)
		ic_list = load_lhs_stmt.get_icode_list();
	if (load_rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), load_rhs_stmt.get_icode_list());
    //now list contains lhs & rhs stmts, now insert the compare_stmt created above
    ic_list.push_back(compare_stmt);

	Code_For_Ast * rel_stmt;
	if (ic_list.empty() == false)
		rel_stmt = new Code_For_Ast(ic_list, result_register);

	//free lhs & rhs register descriptors
	lhs_register->reset_used_for_expr_result();
	rhs_register->reset_used_for_expr_result();

	return *rel_stmt;
	*/
}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{
	variable_symbol_entry = &var_entry;

	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");

	ast_num_child = zero_arity;
	node_data_type = variable_symbol_entry->get_data_type();
	lineno = line;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry->get_data_type();
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	return *variable_symbol_entry;
}

void Name_Ast::print(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_symbol_entry->get_variable_name();
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	Eval_Result * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		CHECK_INVARIANT(loc_var_val->get_result_enum() == int_result, "Result type can only be int");
		file_buffer << loc_var_val->get_int_value() << "\n";
	}

	else
	{
		CHECK_INVARIANT(glob_var_val->get_result_enum() == int_result, 
			"Result type can only be int and float");

		if (glob_var_val == NULL)
			file_buffer << "0\n";
		else
			file_buffer << glob_var_val->get_int_value() << "\n";
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	if (eval_env.does_variable_exist(variable_name))
	{
		CHECK_INPUT_AND_ABORT((eval_env.is_variable_defined(variable_name) == true), 
					"Variable should be defined before its use", lineno);

		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	CHECK_INPUT_AND_ABORT((interpreter_global_table.is_variable_defined(variable_name) == true), 
				"Variable should be defined before its use", lineno);

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result * i;
	string variable_name = variable_symbol_entry->get_variable_name();

	if (variable_symbol_entry->get_data_type() == int_data_type)
		i = new Eval_Result_Value_Int();
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (result.get_result_enum() == int_result)
	 	i->set_value(result.get_int_value());
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

Code_For_Ast & Name_Ast::compile()
{
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);
	Register_Val_Type reg_type = float_num;
	if(node_data_type == int_data_type) reg_type = int_num;
		
	Register_Descriptor * result_register = machine_dscr_object.get_new_register(reg_type);
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Tgt_Op tgt_operation = load_d;
	if(node_data_type == int_data_type) tgt_operation = load;
	
	//cout << "creating move ic stmt Name_Ast " <<endl;
	Icode_Stmt * load_stmt = new Move_IC_Stmt(tgt_operation, opd, register_opd);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{
	CHECK_INVARIANT((store_register != NULL), "Store register cannot be null");

	Ics_Opd * register_opd = new Register_Addr_Opd(store_register);
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

	Icode_Stmt * store_stmt;
	if(node_data_type == int_data_type)
		store_stmt =  new Move_IC_Stmt(store, register_opd, opd);
	else
		store_stmt =  new Move_IC_Stmt(store_d, register_opd, opd);

	if (command_options.is_do_lra_selected() == false)
		variable_symbol_entry->free_register(store_register); //i.e if lra is false, then simply free register holding the value

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(store_stmt);

	Code_For_Ast & name_code = *new Code_For_Ast(ic_list, store_register);

	return name_code;
}

Code_For_Ast & Name_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;;

	bool load_needed = lra.is_load_needed();

	Register_Descriptor * result_register = lra.get_register();
	CHECK_INVARIANT((result_register != NULL), "Register cannot be null");
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = NULL;
	if (load_needed)
	{
		Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

		load_stmt = new Move_IC_Stmt(load, opd, register_opd);
			
		ic_list.push_back(load_stmt);
	}

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	constant = number;
	node_data_type = constant_data_type;

	ast_num_child = zero_arity;

	lineno = line;
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
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	file_buffer << std::fixed;
	file_buffer << std::setprecision(2);

	file_buffer << "Num : " << constant;
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

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile()
{
	Register_Val_Type reg_type = float_num;
	if(node_data_type == int_data_type) reg_type = int_num;
	
	Register_Descriptor * result_register = machine_dscr_object.get_new_register(reg_type);
	
	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(result_register);
	Ics_Opd * opd;
	if(node_data_type == int_data_type)
		opd = new Const_Opd<int>(constant);
	else
		opd = new Const_Opd<float>(constant);

	Tgt_Op tgt_operation = imm_load_d;
	if(node_data_type == int_data_type) tgt_operation = imm_load;
	
	Icode_Stmt * load_stmt = new Move_IC_Stmt(tgt_operation, opd, load_register);
	

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, result_register);

	return num_code;
}

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lra.get_register() != NULL), "Register assigned through optimize_lra cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(lra.get_register());
	Ics_Opd * opd = new Const_Opd<int>(constant);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, lra.get_register());

	return num_code;
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(int line)
{
	lineno = line;
	node_data_type = void_data_type;
	ast_num_child = zero_arity;
}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	print(file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();
	return result;
}

Code_For_Ast & Return_Ast::compile()
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

Code_For_Ast & Return_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

///////////////////////////////////////////////////////////////////////////////

Goto_Ast::Goto_Ast(int succ, int line)
{
	lineno = line;
	successor = succ;
	ast_num_child = unary_arity;
}

Goto_Ast::~Goto_Ast()
{}

void Goto_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n";
	file_buffer << AST_SPACE << "Goto statement:\n";
	file_buffer << AST_NODE_SPACE"Successor: "<< successor <<endl;
}

int Goto_Ast::get_successor(){
    return successor;
}


Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	print(file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();
    result.set_result_enum(go_to_result); //set the type to go_to_result enum
    result.set_value(successor);
	file_buffer << AST_SPACE << "GOTO (BB "<< successor <<")"<<"\n";
	return result;
}

Code_For_Ast & Goto_Ast::compile()
{
	Ics_Opd * opd1 = new Const_Opd<int>(successor);
	Icode_Stmt * goto_statement = new Control_Flow_IC_Stmt(tgoto, opd1, NULL, NULL);

	// Store the statement in ic_list
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

    ic_list.push_back(goto_statement);

	Code_For_Ast* goto_cfa_stmt = new Code_For_Ast(ic_list, NULL);

	return *goto_cfa_stmt;
}

Code_For_Ast & Goto_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Code_For_Ast & ret_code = compile();
	return ret_code;
}

///////////////////////////////////////////////////////////////////////////////

If_Ast::If_Ast(Ast * temp_condition, Ast * temp_goto_true, Ast* temp_goto_false, int line)
{
	lineno = line;
	condition = temp_condition;
	goto_true = temp_goto_true;
	goto_false= temp_goto_false;
	ast_num_child = binary_arity; //TODO
}

If_Ast::~If_Ast()
{}

void If_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n";
	file_buffer << AST_SPACE << "If_Else statement:";
    condition->print(file_buffer);
    file_buffer <<endl; //this is required because rel_expr->print() doesn't end in newline
    file_buffer << AST_NODE_SPACE << "True Successor: "<< ((Goto_Ast*)goto_true)->get_successor() <<endl;
    file_buffer << AST_NODE_SPACE << "False Successor: "<< ((Goto_Ast*)goto_false)->get_successor() <<endl;
}


Eval_Result & If_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_Int();
    int successor;

    print(file_buffer);

	Eval_Result & condition_result = condition->evaluate(eval_env, file_buffer);
    int cond_result_value = condition_result.get_int_value();
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

Code_For_Ast & If_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((condition != NULL), "Condition cannot be null");
	CHECK_INVARIANT((goto_true != NULL), "goto_true cannot be null");
	CHECK_INVARIANT((goto_false != NULL), "goto_false cannot be null");

	Lra_Outcome lra_cond;

	if(typeid(*condition) == typeid(Number_Ast<int>)){
		lra_cond.optimize_lra(c_2r, NULL, condition);
	}
	else if(typeid(*condition) == typeid(Name_Ast)){
		lra_cond.optimize_lra(m_2r, NULL, condition);
	}
	
	Code_For_Ast & load_cond_stmt = condition->compile_and_optimize_ast(lra_cond);
    // 1) load statements

    // 2) bne statement
	Register_Descriptor * cond_register = load_cond_stmt.get_reg();
	Register_Descriptor * zero_register = machine_dscr_object.get_register(zero);

    //opd1 : $t1, opd2 : $zero, opd3 : label(int)
	Ics_Opd * opd1 = new Register_Addr_Opd(cond_register);
	Ics_Opd * opd2 = new Register_Addr_Opd(zero_register);
	Ics_Opd * opd3 = new Const_Opd<int>(((Goto_Ast*)goto_true)->get_successor());
	Icode_Stmt * bne_statement = new Control_Flow_IC_Stmt(bne, opd1, opd2, opd3);

    // 3) goto statement
	opd1 = new Const_Opd<int>(((Goto_Ast*)goto_false)->get_successor());
	Icode_Stmt * goto_statement = new Control_Flow_IC_Stmt(tgoto, opd1, NULL, NULL);

	// Store the statement in ic_list
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	if (load_cond_stmt.get_icode_list().empty() == false)   //---1
		ic_list = load_cond_stmt.get_icode_list();

    ic_list.push_back(bne_statement);   //---2
    ic_list.push_back(goto_statement);   //---3

	Code_For_Ast* if_cfa_stmt = new Code_For_Ast(ic_list, NULL);

    //free the register holding condition value
    cond_register->reset_used_for_expr_result();

	return *if_cfa_stmt;
}

Code_For_Ast & If_Ast::compile()
{
	CHECK_INVARIANT((condition != NULL), "Condition cannot be null");
	CHECK_INVARIANT((goto_true != NULL), "goto_true cannot be null");
	CHECK_INVARIANT((goto_false != NULL), "goto_false cannot be null");

	Code_For_Ast & load_cond_stmt = condition->compile();
    // 1) load statements

    // 2) bne statement
	Register_Descriptor * cond_register = load_cond_stmt.get_reg();
	Register_Descriptor * zero_register = machine_dscr_object.get_register(zero);

    //opd1 : $t1, opd2 : $zero, opd3 : label(int)
	Ics_Opd * opd1 = new Register_Addr_Opd(cond_register);
	Ics_Opd * opd2 = new Register_Addr_Opd(zero_register);
	Ics_Opd * opd3 = new Const_Opd<int>(((Goto_Ast*)goto_true)->get_successor());
	Icode_Stmt * bne_statement = new Control_Flow_IC_Stmt(bne, opd1, opd2, opd3);

    // 3) goto statement
	opd1 = new Const_Opd<int>(((Goto_Ast*)goto_false)->get_successor());
	Icode_Stmt * goto_statement = new Control_Flow_IC_Stmt(tgoto, opd1, NULL, NULL);

	// Store the statement in ic_list
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	if (load_cond_stmt.get_icode_list().empty() == false)   //---1
		ic_list = load_cond_stmt.get_icode_list();

    ic_list.push_back(bne_statement);   //---2
    ic_list.push_back(goto_statement);   //---3

	Code_For_Ast* if_cfa_stmt = new Code_For_Ast(ic_list, NULL);

    //free the register holding condition value
    cond_register->reset_used_for_expr_result();

	return *if_cfa_stmt;
}

//////////////////////////////// Type_Cast ast /////////////

Type_Cast_Ast::Type_Cast_Ast(Ast * temp_expression, Data_Type type, int line)
{
	lineno = line;
	node_data_type = type;
	expression = temp_expression;
	ast_num_child = unary_arity; //TODO
}

Type_Cast_Ast::~Type_Cast_Ast()
{}

void Type_Cast_Ast::print(ostream & file_buffer)
{
	expression->print(file_buffer);
}


Eval_Result & Type_Cast_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
}

Code_For_Ast & Type_Cast_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
}

Code_For_Ast & Type_Cast_Ast::compile()
{
	CHECK_INVARIANT((expression != NULL), "Expression for typecast cannot be null");
	Code_For_Ast & expression_stmt = expression->compile();
	Register_Descriptor * expression_register = expression_stmt.get_reg();
    //opd1 : $t1, opd2 : $zero, opd3 : label(int)
	Ics_Opd * opd1 = new Register_Addr_Opd(expression_register);
	
	Icode_Stmt * move_stmt; //if needed
	Register_Descriptor * result_register ;
	
	if(node_data_type==int_data_type && expression->get_data_type()!=int_data_type){
		//use typecasting to int
		result_register = machine_dscr_object.get_new_register(int_num); //to hold the result of 
		CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
		Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
		move_stmt = new Move_IC_Stmt(cast_to_int, opd1, result_opd);
	}
	else if( node_data_type!= int_data_type && expression->get_data_type()==int_data_type){
		//use typecasting to float
		result_register = machine_dscr_object.get_new_register(float_num); //to hold the result of 
		CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
		Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
		move_stmt = new Move_IC_Stmt(cast_to_float, opd1, result_opd);
	}
	else{
		//Nothing needs to be done, typecast is useless
		move_stmt = NULL;
		return expression_stmt;
	}
	
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (expression_stmt.get_icode_list().empty() == false)
		ic_list = expression_stmt.get_icode_list();
	//now insert the move stmt created above
    ic_list.push_back(move_stmt);

	Code_For_Ast * typecast_stmt;
	if (ic_list.empty() == false)
		typecast_stmt = new Code_For_Ast(ic_list, result_register);

	//free register holding the expression
	expression_register->reset_used_for_expr_result();

	return *typecast_stmt;
	
}
/////////////////////////////////////////////

template class Number_Ast<int>;
template class Number_Ast<float>;
template class Number_Ast<double>;

///////////////// UTILS DEFINITION /////////////////////////
//
bool is_compatible(Data_Type d1, Data_Type d2){
    if(d1 == d2) return true;
    if( (d1 == double_data_type && d2 == float_data_type) || (d2 == double_data_type && d1 == float_data_type))
        return true;
    return false;
}
