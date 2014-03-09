
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

#include<string>
#include<fstream>
#include<iostream>

using namespace std;

#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

Procedure::Procedure(Data_Type proc_return_type, string proc_name)
{
	return_type = proc_return_type;
	name = proc_name;
}

Procedure::~Procedure()
{
	list<Basic_Block *>::iterator i;
	for (i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		delete (*i);
}

string Procedure::get_proc_name()
{
	return name;
}

void Procedure::set_basic_block_list(list<Basic_Block *> bb_list)
{
	basic_block_list = bb_list;
}

void Procedure::set_local_list(Symbol_Table & new_list)
{
	local_symbol_table = new_list;
	local_symbol_table.set_table_scope(local);
}

Data_Type Procedure::get_return_type()
{
	return return_type;
}

bool Procedure::variable_in_symbol_list_check(string variable)
{
	return local_symbol_table.variable_in_symbol_list_check(variable);
}

Symbol_Table_Entry & Procedure::get_symbol_table_entry(string variable_name)
{
	return local_symbol_table.get_symbol_table_entry(variable_name);
}

Symbol_Table * Procedure::get_symbol_table()
{
    return &local_symbol_table;
}
void Procedure::set_param_count(int count){
    param_count = count;
}

int Procedure::get_param_count(){
    return param_count;
}

bool Procedure::validate_arg_list(list<Ast*> args, int line){
    list<Symbol_Table_Entry *> var_table = local_symbol_table.get_variable_table();

    list<Ast*>::iterator arg_itr = args.begin();
    list<Symbol_Table_Entry*>::iterator var_itr = var_table.begin();
    if(args.size() != param_count){
	    report_error("No of arguments in fn call doesn't match function prototype", line);
        return false;
    }
    for(; arg_itr != args.end(); arg_itr++, var_itr++){
        if ((*arg_itr)->get_data_type() != (*var_itr)->get_data_type()){
	        report_error("Arguments type don't match argument type expected in function prototype", line);
            return false;
        }
    }
    return true;
}

void Procedure::print_ast(ostream & file_buffer)
{
	file_buffer << PROC_SPACE << "Procedure: " << name<< "\n\n";

	list<Basic_Block *>::iterator i = basic_block_list.begin();
	for(; i != basic_block_list.end(); i++)
		(*i)->print_bb(file_buffer);

    i--;
}
	
Basic_Block & Procedure::get_start_basic_block()
{
	list<Basic_Block *>::iterator i;
	i = basic_block_list.begin();
	return **i;
}

Basic_Block * Procedure::get_next_bb(Basic_Block & current_bb)
{
	bool flag = false;

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
	{
		if((*i)->get_bb_number() == current_bb.get_bb_number())
		{
			flag = true;
			continue;
		}
		if (flag)
			return (*i);
	}
	
	return NULL;
}

Basic_Block * Procedure::get_target_bb(int target)
{
	bool flag = false;

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
	{
		if((*i)->get_bb_number() == target)
		{
            return (*i);
        }
	}
	
	return NULL;
}

Eval_Result & Procedure::evaluate(ostream & file_buffer, list<Eval_Result_Value*> args)
{
	Local_Environment & eval_env = *new Local_Environment();
	local_symbol_table.create(eval_env);

    /* evaluate without printing assignment asts (passed as 'args') in eval_env */
	//void put_variable_value(Eval_Result_Value & value, string name);
    list<Symbol_Table_Entry*> var_table = local_symbol_table.get_variable_table();
    list<Symbol_Table_Entry*>::iterator var_it = var_table.begin();

    list<Eval_Result_Value*>::iterator arg_it = args.begin();
    for(; arg_it!=args.end() ;arg_it++, var_it++){
        eval_env.put_variable_value(*(*arg_it), (*var_it)->get_variable_name());
    }
    /* */
	
	Eval_Result * result = NULL;

	file_buffer << PROC_SPACE << "Evaluating Procedure " << name << "\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):\n";
	eval_env.print(file_buffer);
	file_buffer << "\n";
	
	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{
		result = &(current_bb->evaluate(eval_env, file_buffer));
        if(result->get_result_flag() == go_to_flag){ //if this is the result of go-to stmt , update the current_bb accordingly
            Value_Bundle result_value = result->get_value();
            int target = result_value.int_v;
            current_bb = get_target_bb(target);
            if(current_bb == NULL) {
                char c_error[100];
                sprintf(c_error, "bb %d doesn't exist", target);
                string error(c_error);
                report_error(error, -1); //-1 means this is cfglp error (o/w line > 0)
            }
        }
        else{
            current_bb = get_next_bb(*current_bb);		
        }
        if(result->get_result_flag() == return_flag){
            break;
        } //if this is the result of go-to stmt , update the current_bb accordingly
	}

    if(result->get_result_flag() != return_flag){
        report_internal_error("Atleast one of true, false, direct successors should be set");
    }

	file_buffer << "\n\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating):\n";
	eval_env.print(file_buffer);
    if(return_type!=void_data_type){
#define AST_SPACE "         "
	    file_buffer << AST_SPACE << "return : ";
        result->print(file_buffer);
        file_buffer<<endl;
    }

	return *result;
}
