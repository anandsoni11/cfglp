
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
#include<iostream>

using namespace std;

#include"local-environment.hh"
#include"user-options.hh"
#include"error-display.hh"

#include"symbol-table.hh"

#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

Symbol_Table::Symbol_Table()
{}

Symbol_Table::~Symbol_Table()
{}

void Symbol_Table::set_table_scope(Table_Scope list_scope)
{
	scope = list_scope;
}

Table_Scope Symbol_Table::get_table_scope()
{
	return scope;
}

void Symbol_Table::push_symbol(Symbol_Table_Entry * variable)
{
	variable_table.push_back(variable);
}

void Symbol_Table::global_list_in_proc_map_check(int line)
{
	list<Symbol_Table_Entry *>::iterator i;
	for (i = variable_table.begin(); i != variable_table.end(); i++)
	{
		string name = (*i)->get_variable_name();
		program_object.variable_in_proc_map_check(name, line);
	}
}

bool Symbol_Table::variable_in_symbol_list_check(string variable)
{
	list<Symbol_Table_Entry *>::iterator i;
	for (i = variable_table.begin(); i != variable_table.end(); i++)
	{
		if ((*i)->get_variable_name() == variable)
			return true;
	}

	return false;
}

Symbol_Table_Entry & Symbol_Table::get_symbol_table_entry(string variable_name)
{
	list<Symbol_Table_Entry *>::iterator i;
	for (i = variable_table.begin(); i != variable_table.end(); i++)
	{
		if ((*i)->get_variable_name() == variable_name)
			return **i;
	}

	report_error("variable symbol entry doesn't exist", NOLINE);
}

void Symbol_Table::create(Local_Environment & local_global_variables_table)
{
	list<Symbol_Table_Entry *>::iterator i;

	for (i = variable_table.begin(); i != variable_table.end(); i++)
	{
		string name = (*i)->get_variable_name();
        Data_Type dtype = (*i)->get_data_type();

        if(dtype == int_data_type){
            Eval_Result_Value_Int * j = new Eval_Result_Value_Int();
            if(scope == global){
                j->set_variable_status(true);

                Value_Bundle bundle;
                bundle.int_v = 0;
                j->set_value(bundle);
            }
            local_global_variables_table.put_variable_value(*j, name);
        }
        else if(dtype == float_data_type){
            Eval_Result_Value_Float * j = new Eval_Result_Value_Float();
            if(scope == global){
                j->set_variable_status(true);

                Value_Bundle bundle;
                bundle.float_v = 0;
                j->set_value(bundle);
            }
            local_global_variables_table.put_variable_value(*j, name);
        }
        else if(dtype == double_data_type){
            Eval_Result_Value_Double * j = new Eval_Result_Value_Double();
            if(scope == global){
                j->set_variable_status(true);

                Value_Bundle bundle;
                bundle.float_v = 0;
                j->set_value(bundle);
            }
            local_global_variables_table.put_variable_value(*j, name);
        }
	}
}

bool Symbol_Table::check(Symbol_Table * temp, int line){
    list<Symbol_Table_Entry *> temp_variable_table = temp->get_variable_table();
    if(variable_table.size() != temp_variable_table.size()){
        report_error("No of params in function prototype & function definition don't match", line);
        return false;
    }
    list<Symbol_Table_Entry*>::iterator it1,it2;
    it1 = variable_table.begin();
    it2 = temp_variable_table.begin();
    for(; it1!=variable_table.end(); it1++, it2++){
        if(! (*it1)->check(*it2)){
            report_error("Parameter name/type in function prototype & function definition don't match", line);
            return false;
        }
    }
    return true;
}

list<Symbol_Table_Entry *> Symbol_Table::get_variable_table(){
    return variable_table;
}
int Symbol_Table::size(){
    return variable_table.size();
}
/////////////////////////////////////////////////////////////

Symbol_Table_Entry::Symbol_Table_Entry()
{}

Symbol_Table_Entry::Symbol_Table_Entry(string & name, Data_Type new_data_type)
{
	variable_name = name;
	variable_data_type = new_data_type;
}

void Symbol_Table_Entry:: set_data_type(Data_Type type) {
    variable_data_type = type;
}

Symbol_Table_Entry::~Symbol_Table_Entry()
{}

Data_Type Symbol_Table_Entry::get_data_type()
{
	return variable_data_type;
}

string Symbol_Table_Entry::get_variable_name()
{
	return variable_name;
}

bool Symbol_Table_Entry::check(Symbol_Table_Entry * e){
    if(e->variable_name == variable_name && e->variable_data_type == variable_data_type) return true;
    return false;
}
