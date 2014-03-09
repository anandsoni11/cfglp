
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

#ifndef AST_HH
#define AST_HH

#include <string>
#include <map>
#include <iomanip>
#include "procedure.hh" //NEW

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "
#define COND_SPACE "            "
#define COND_NODE_SPACE "               "



static map<int, string> rel_operators_map = {{0, "LT"}, {1, "GT"}, {2, "GE"}, {3, "LE"}, {4, "NE"}, {5, "EQ"}};
static map<int, string> arith_operators_map = {{0, "UMINUS"}, {1, "PLUS"}, {2, "MINUS"}, {3, "MULT"}, {4, "DIV"}};

using namespace std;

class Ast;

class Ast
{
protected:
	Data_Type node_data_type;
public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual void set_data_type(Data_Type type); 
	virtual bool check_ast(int line);

	virtual void print_ast(ostream & file_buffer) = 0;
	virtual void print_value(Local_Environment & eval_env, ostream & file_buffer);

	virtual Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	virtual void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	virtual Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer) = 0;
};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Assignment_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);
	void set_data_type(Data_Type type); 

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Relational_Expr_Ast:public Ast
{
	Ast * lhs;
    int rel_op;
	Ast * rhs;

public:
	Relational_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs, int temp_op, Data_Type constant_data_type);
	~Relational_Expr_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);
    int compare(Value_Bundle x, Value_Bundle y, Result_Enum res_enum);
	void set_data_type(Data_Type type); 

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Arithmetic_Expr_Ast:public Ast
{
	Ast * lhs;
    int arith_op;
	Ast * rhs;

public:
	Arithmetic_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs, int temp_op);
	~Arithmetic_Expr_Ast();
	void set_data_type(Data_Type type); 

	Data_Type get_data_type();
	bool check_ast(int line);
	void print_ast(ostream & file_buffer);
    Value_Bundle calculate(Value_Bundle x, Value_Bundle y, Result_Enum res_enum);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Function_Call_Ast:public Ast
{
    string name;
    list<Ast*> args;
    Procedure * proc;

public:
	Function_Call_Ast(string temp_name, list<Ast*> temp_args, Procedure *p);
	~Function_Call_Ast();
	void set_data_type(Data_Type type); 

	Data_Type get_data_type();
	bool check_ast(int line);
	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Name_Ast:public Ast
{
	string variable_name;
	Symbol_Table_Entry variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry);
	~Name_Ast();

	Data_Type get_data_type();

	void set_data_type(Data_Type type); 
	void print_ast(ostream & file_buffer);

	void print_value(Local_Environment & eval_env, ostream & file_buffer);
	Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

template <class T>
class Number_Ast:public Ast //Method definition @ End of this file
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type);
	~Number_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type type); 

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Return_Ast:public Ast
{
    Ast * expression;

public:
	Return_Ast(Ast * expr);
	~Return_Ast();
	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Goto_Ast:public Ast
{
    int successor;

public:
	Goto_Ast(int succ);
	~Goto_Ast();
	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);
    int get_successor();

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class If_Ast:public Ast
{
	Ast * condition;
	Ast * goto_true;
	Ast * goto_false;

public:
	If_Ast(Ast * temp_condition, Ast * temp_goto_true, Ast* temp_goto_false);
	~If_Ast();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

/////////////////////////////  UTILS FUNCTIONS ///////////////////////////////////////////////////////////
bool is_compatible(Data_Type d1, Data_Type d2);
//////////////////////////NUMBER AST TEMPLATE DEFININTION/////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type)
{
	constant = number;
	node_data_type = constant_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::set_data_type(Data_Type type)
{
    node_data_type = type;
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
	file_buffer << "Num : " << fixed << setprecision(2) << constant;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
        Value_Bundle bundle;
        bundle.int_v = constant;
		result.set_value(bundle);

		return result;
	}
    else if (node_data_type == float_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Float();
        Value_Bundle bundle;
        bundle.float_v = constant;
		result.set_value(bundle);

		return result;
	}
    else if (node_data_type == double_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Double();
        Value_Bundle bundle;
        bundle.float_v = constant;
		result.set_value(bundle);

		return result;
	}
}

#endif
