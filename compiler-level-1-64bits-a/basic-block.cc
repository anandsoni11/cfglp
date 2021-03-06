
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

using namespace std;

#include"local-environment.hh"
#include"error-display.hh"
#include"user-options.hh"

#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"

Basic_Block::Basic_Block(int basic_block_number, list<Ast *> & ast_list)
{
	id_number = basic_block_number;
	statement_list = ast_list;
}

Basic_Block::~Basic_Block()
{
	list<Ast *>::iterator i;
	for (i = statement_list.begin(); i != statement_list.end(); i++)
		delete (*i);
}

int Basic_Block::get_bb_number()
{
	return id_number;
}

void Basic_Block::print_bb(ostream & file_buffer)
{
	file_buffer << BB_SPACE << "Basic_Block " << id_number << "\n";

	list<Ast *>::iterator i;
	for(i = statement_list.begin(); i != statement_list.end(); i++)
		(*i)->print_ast(file_buffer);
}

void Basic_Block::print_icode(ostream & file_buffer)
{
	file_buffer << "label" << id_number << ":\n";

	list<Ast *>::iterator i;
	for(i = statement_list.begin(); i != statement_list.end(); i++)
		(*i)->print_icode(file_buffer);

    file_buffer << endl;
}

void  Basic_Block::check_if_return(){
    Ast * last = statement_list.back();
    if(last->get_data_type() != return_data_type){
        report_internal_error("No return at end of last basic block : Atleast one of true, false, direct successors should be set");
    }
}

Eval_Result & Basic_Block::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result * result = NULL;

	file_buffer << "\n" << BB_SPACE << "Basic Block: " << id_number << "\n\n";

    if(statement_list.size() == 0){
        report_internal_error("result from ast cannot be null");
    }
	list <Ast *>::iterator i;
	for (i = statement_list.begin(); i != statement_list.end(); i++)
	{
		if((*i) == NULL)
			report_error ("Ast pointer seems to be NULL", NOLINE);

		result = &((*i)->evaluate(eval_env, file_buffer)); 
        if(result->get_result_enum() == go_to_result) return *result;
        if(result->get_result_enum() == return_result) return *result;
	}

	return *result;
}
