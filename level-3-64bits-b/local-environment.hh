
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

#ifndef LOCAL_ENVIRONMENT_HH
#define LOCAL_ENVIRONMENT_HH

#include<string>
#include<map>

#define VAR_SPACE "         "

using namespace std;

typedef struct{
    int int_v;
    float float_v;
} Value_Bundle;

typedef enum
{
	int_result,
	float_result,
	double_result,
	void_result,
} Result_Enum;

typedef enum
{
    normal_flag,
    go_to_flag, //go_to_ast returns Eval_Result_Value_Int with this flag 
    return_flag
} Result_Flag;

class Eval_Result;
class Local_Environment;

class Eval_Result 
{
protected:
	Result_Enum result_type;
	Result_Flag result_flag; //must be set before returning

public:
	virtual Value_Bundle get_value();
	virtual void set_value(Value_Bundle value);
    virtual void print(ostream & file_buffer) = 0;

	virtual bool is_variable_defined();
	virtual void set_variable_status(bool def);

	virtual void set_result_enum(Result_Enum res) = 0;
	virtual Result_Enum get_result_enum() = 0;
	virtual void set_result_flag(Result_Flag res) = 0;
	virtual Result_Flag get_result_flag() = 0;
};

class Eval_Result_Value:public Eval_Result
{
public:
	virtual void set_value(Value_Bundle number) = 0;
	virtual Value_Bundle get_value() = 0;
    virtual void print(ostream & file_buffer) = 0;

	virtual bool is_variable_defined() = 0;
	virtual void set_variable_status(bool def) = 0;

	virtual void set_result_enum(Result_Enum res) = 0;
	virtual Result_Enum get_result_enum() = 0;
	virtual void set_result_flag(Result_Flag res) = 0;
	virtual Result_Flag get_result_flag() = 0;
};

class Eval_Result_Value_Int:public Eval_Result_Value
{
    int value;
	bool defined;
public:
	Eval_Result_Value_Int();
	~Eval_Result_Value_Int();

	void set_value(Value_Bundle number);
	Value_Bundle get_value();
    void print(ostream & file_buffer);

	void set_variable_status(bool def);
	bool is_variable_defined();

	void set_result_enum(Result_Enum res);
	Result_Enum get_result_enum();
	void set_result_flag(Result_Flag res);
	Result_Flag get_result_flag();
};

class Eval_Result_Value_Float:public Eval_Result_Value
{
	float value;
	bool defined;
public:
	Eval_Result_Value_Float();
	~Eval_Result_Value_Float();

	void set_value(Value_Bundle number);
	Value_Bundle get_value();
    void print(ostream & file_buffer);

	void set_variable_status(bool def);
	bool is_variable_defined();

	void set_result_enum(Result_Enum res);
	Result_Enum get_result_enum();
	void set_result_flag(Result_Flag res);
	Result_Flag get_result_flag();
};

class Eval_Result_Value_Double:public Eval_Result_Value
{
	double value;
	bool defined;
public:
	Eval_Result_Value_Double();
	~Eval_Result_Value_Double();

	void set_value(Value_Bundle number);
	Value_Bundle get_value();
    void print(ostream & file_buffer);

	void set_variable_status(bool def);
	bool is_variable_defined();

	void set_result_enum(Result_Enum res);
	Result_Enum get_result_enum();
	void set_result_flag(Result_Flag res);
	Result_Flag get_result_flag();
};

class Local_Environment
{
	map<string, Eval_Result_Value *> variable_table;

public:
	Local_Environment();
	~Local_Environment();

	void print(ostream & file_buffer);
	void print_new(ostream & file_buffer, Eval_Result_Value * val, int param_count);
	bool is_variable_defined(string name);
	Eval_Result_Value * get_variable_value(string name);
	void put_variable_value(Eval_Result_Value & value, string name);
	bool does_variable_exist(string name);
};

extern Local_Environment interpreter_global_table;

#endif
