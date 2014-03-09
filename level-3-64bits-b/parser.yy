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
%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	int integer_value;
	float float_value;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
};

%token <integer_value>  NUM /* 257 */
%token <integer_value> BB_ID  /* 258 */
%token <float_value> FNUM  /* 259 */
%token <string_value>  NAME  /* 260 */
%token RETURN INTEGER  FLOAT DOUBLE VOID IF ELSE GOTO /* 261 - 267 */

%right ASSIGN_OP /* 265 - 271 */
%left <integer_value> NE EQ
%left <integer_value> LT LE GT GE
%left '+' '-'
%left '*' '/'

%type <procedure> function_declaration

%type <symbol_table> declaration_statement_list
%type <symbol_table> decl_arg_list
%type <symbol_table> ne_decl_arg_list

%type <symbol_entry> declaration_statement
%type <symbol_entry> declaration

%type <basic_block_list> basic_block_list
%type <basic_block> basic_block

%type <ast_list> executable_statement_list
%type <ast_list> statement_list
%type <ast_list> arg_list
%type <ast_list> ne_arg_list

%type <ast> general_statement
%type <ast> function_call
%type <ast> variable
%type <ast> constant
%type <ast> expression
%type <ast> rel_expression
%type <ast> arith_expression
%type <ast> unary_expression
%type <ast> atomic_expression
%type <ast> goto_statement
%type <ast> if_statement
%type <ast> return_statement


/********PRECEDENCE RULES*********/
%start program

%%

program:
	declaration_statement_list 
    {
        program_object.set_global_table(*global_symbol_table);
    }
    function_definition_list
|
	declaration_statement_list 
    function_declaration_list
    {
        program_object.set_global_table(*global_symbol_table);
    }
    function_definition_list
|
    function_declaration_list
    {
        program_object.set_global_table(*global_symbol_table);
    }
    function_definition_list
|
    {
        program_object.set_global_table(*global_symbol_table);
    }
    function_definition_list
;


function_declaration_list:
    function_declaration
    {
    }
|
    function_declaration_list function_declaration
    {
    }
;

function_declaration:
	VOID NAME '(' decl_arg_list ')' ';'
	{
		$$ = new Procedure(void_data_type, *$2);
        $$->set_local_list(*$4);
		program_object.set_procedure_map(*$$);
		$4->global_list_in_proc_map_check(get_line_number());

        /* push into global symb table*/
        if(global_symbol_table->variable_in_symbol_list_check(*$2))
        {
            int line = get_line_number();
            report_error("Variable/Function is declared twice", line);
        }
		Symbol_Table_Entry * ste = new Symbol_Table_Entry(*$2, function_data_type);
        global_symbol_table->push_symbol(ste);
	}
|
	INTEGER NAME '(' decl_arg_list ')' ';'
	{
		$$ = new Procedure(int_data_type, *$2);
        $$->set_local_list(*$4);
		program_object.set_procedure_map(*$$);
		$4->global_list_in_proc_map_check(get_line_number());

        /* push into global symb table*/
        if(global_symbol_table->variable_in_symbol_list_check(*$2))
        {
            int line = get_line_number();
            report_error("Variable/Function is declared twice", line);
        }
		Symbol_Table_Entry * ste = new Symbol_Table_Entry(*$2, function_data_type);
        global_symbol_table->push_symbol(ste);
	}
|
	DOUBLE NAME '(' decl_arg_list ')' ';'
	{
		$$ = new Procedure(double_data_type, *$2);
        $$->set_local_list(*$4);
		program_object.set_procedure_map(*$$);
		$4->global_list_in_proc_map_check(get_line_number());

        /* push into global symb table*/
        if(global_symbol_table->variable_in_symbol_list_check(*$2))
        {
            int line = get_line_number();
            report_error("Variable/Function is declared twice", line);
        }
		Symbol_Table_Entry * ste = new Symbol_Table_Entry(*$2, function_data_type);
        global_symbol_table->push_symbol(ste);
	}
|
	FLOAT NAME '(' decl_arg_list ')' ';'
	{
		$$ = new Procedure(float_data_type, *$2);
        $$->set_local_list(*$4);
		program_object.set_procedure_map(*$$);
		$4->global_list_in_proc_map_check(get_line_number());

        /* push into global symb table*/
        if(global_symbol_table->variable_in_symbol_list_check(*$2))
        {
            int line = get_line_number();
            report_error("Variable/Function is declared twice", line);
        }
		Symbol_Table_Entry * ste = new Symbol_Table_Entry(*$2, function_data_type);
        global_symbol_table->push_symbol(ste);
	}
;

function_definition_list:
    {
    }
|
    function_definition_list function_definition
    {
    }
;

function_definition:
	NAME 
    {
		return_statement_used_flag = false;
        if(*$1 == "main"){ //i.e the main's definition
		    current_procedure = new Procedure(void_data_type, *$1);
        }
        else{
            current_procedure = program_object.get_procedure(*$1);
            if(current_procedure == NULL){
                int line = get_line_number();
                report_error("Procedure has not been declared before", line);
            }
        }
    }
    '(' decl_arg_list ')'
    {
        //TODO match the entries in above symb table and curr->symb-table
    }
    procedure_body
	{
	}
;

decl_arg_list:
    {
        $$ = new Symbol_Table();
    }
|
    ne_decl_arg_list
    {
        $$ = $1;
    }
;

ne_decl_arg_list:
    ne_decl_arg_list ',' declaration
	{
		int line = get_line_number();
		program_object.variable_in_proc_map_check($3->get_variable_name(), line);

		string var_name = $3->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				int line = get_line_number();
				report_error("Variable is declared twice", line);
			}

			$$ = $1;
		}

		else
			$$ = new Symbol_Table();

		$$->push_symbol($3);
	}
|
    declaration
	{
		int line = get_line_number();
		program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		string var_name = $1->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		$$ = new Symbol_Table();
		$$->push_symbol($1);
	}
;

procedure_body:
	'{' declaration_statement_list
	{
		//current_procedure->set_local_list(*$2);  //No need to set as it has already been set during function declaration phase
		//delete $2;
	}
	basic_block_list '}'
	{
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}
        check_goto_validity(); //check if every goto statement points to a block that exists

        /* guard */
        if(current_procedure == NULL){
			int line = get_line_number();
			report_error("Corresponding function has never been declared", line);
        }
        else{
            current_procedure->set_basic_block_list(*$4);
        }

		delete $4;
	}
|
	'{' basic_block_list '}'
	{
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}

        /* guard */
        if(current_procedure == NULL){
			int line = get_line_number();
			report_error("Corresponding function has never been declared", line);
        }
        else{
            current_procedure->set_basic_block_list(*$2);
        }

		delete $2;
	}
;

declaration_statement_list:
	declaration_statement
	{
		int line = get_line_number();
		program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		string var_name = $1->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}
        if(current_procedure == NULL){
		    $$ = global_symbol_table;
        }
        else{
            $$ = current_procedure->get_symbol_table();
        }

		$$->push_symbol($1);
	}
|
	declaration_statement_list declaration_statement
	{
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

		int line = get_line_number();
		program_object.variable_in_proc_map_check($2->get_variable_name(), line);

		string var_name = $2->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				int line = get_line_number();
				report_error("Variable is declared twice", line);
			}

			$$ = $1;
		}

		else
        {
            if(current_procedure == NULL){
                $$ = global_symbol_table;
            }
            else{
                $$ = current_procedure->get_symbol_table();
            }
        }

		$$->push_symbol($2);
	}
;

declaration_statement:
    declaration ';'
    {
        $$ = $1;
    }
;

declaration:
	INTEGER NAME 
	{
		$$ = new Symbol_Table_Entry(*$2, int_data_type);

		delete $2;
	}
|
	DOUBLE NAME 
	{
		$$ = new Symbol_Table_Entry(*$2, double_data_type);

		delete $2;
	}
|
	FLOAT NAME 
	{
		$$ = new Symbol_Table_Entry(*$2, float_data_type);

		delete $2;
	}
;

basic_block_list:
	basic_block_list basic_block
	{
		if (!$2)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		bb_strictly_increasing_order_check($$, $2->get_bb_number());

		$$ = $1;
		$$->push_back($2);
	}
|
	basic_block
	{
		if (!$1)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		$$ = new list<Basic_Block *>;
		$$->push_back($1);
	}
;


basic_block:
    BB_ID ':' executable_statement_list
	{
		if ($1 < 2)
		{
			int line = get_line_number();
			report_error("Illegal basic block lable", line);
		}

		if ($3 != NULL)
			$$ = new Basic_Block($1, *$3);
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			$$ = new Basic_Block($1, *ast_list);
		}
        bb_blocks.push_back($1);

	}
;

executable_statement_list:
	statement_list
	{
		$$ = $1;
	}
|
	statement_list return_statement 
	{

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
	}
|
	statement_list  if_statement
    {
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
    }
|
	statement_list goto_statement
    {
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
    }
;

return_statement:
    RETURN ';'
    {
		return_statement_used_flag = true;					// Current procedure has an occurrence of return statement
		Ast * ret = new Return_Ast(NULL);
        $$ = ret;
    }
|
    RETURN expression ';'
    {
		return_statement_used_flag = true;					// Current procedure has an occurrence of return statement
		Ast * ret = new Return_Ast($2);
        $$ = ret;
    }
;
statement_list:
	{
		$$ = NULL;
	}
|
	statement_list general_statement
	{
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
	}
;

general_statement:
	variable ASSIGN_OP expression ';'
	{
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
	}
|
    function_call ';'
	{
        $$ = $1;
	}
;

goto_statement:
    GOTO BB_ID ';'
    {
        $$ = new Goto_Ast($2);
        goto_targets.push_back($2);
    }
;

if_statement:
    IF '('expression ')'
        goto_statement
    ELSE
        goto_statement
    {
        $$ = new If_Ast($3, $5, $7);
    }
;

expression:
    '(' INTEGER ')'  atomic_expression
    {
		$4->set_data_type(int_data_type);
        $$ = $4;
    }
|
    '(' FLOAT ')'  atomic_expression
    {
		$4->set_data_type(float_data_type);
        $$ = $4;
    }
|
    '(' DOUBLE ')'  atomic_expression
    {
		$4->set_data_type(double_data_type);
        $$ = $4;
    }
|
	rel_expression
    {
        $$ = $1;
    }
|
    arith_expression
    {
        $$ = $1;
    }
|
    unary_expression
    {
        $$ = $1;
    }
;

unary_expression:
    atomic_expression
    {
        $$ = $1;
    }
|
    '-' atomic_expression
    {
        $$ = new Arithmetic_Expr_Ast(NULL, $2, 0);
		int line = get_line_number();
		$$->check_ast(line);
    }
;

atomic_expression: /* TODO string */
	variable
    {
        $$ = $1;
    }
|
	constant
    {
        $$ = $1;
    }
|
	'(' expression ')'
    {
        $$ = $2;
    }
|
    function_call
    {
        $$ = $1;
    }
;

function_call:
    NAME '(' arg_list ')'
    {
		int line = get_line_number();

        Procedure * p = program_object.get_procedure(*$1);
        if(p == NULL){
			report_error("Function has not been declared but used here", line);
        }
		$$ = new Function_Call_Ast(*$1, *$3, p);

		$$->check_ast(line);
    }
;

arg_list:
    {
        $$ = new list<Ast*>; //empty list
    }
|
    ne_arg_list
    {
        $$ = $1;
    }
;

ne_arg_list:
    ne_arg_list ',' expression
	{

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($3);
	}
| 
    expression
    {
		$$ = new list<Ast *>;

		$$->push_back($1);
    }
;

rel_expression:
	expression LT expression
    {
        $$ = new Relational_Expr_Ast($1, $3, $2, int_data_type);
		int line = get_line_number();
		$$->check_ast(line);
    }
|
	expression GT expression
    {
        $$ = new Relational_Expr_Ast($1, $3, $2, int_data_type);
		int line = get_line_number();
		$$->check_ast(line);
    }
|
	expression GE expression
    {
        $$ = new Relational_Expr_Ast($1, $3, $2, int_data_type);
		int line = get_line_number();
		$$->check_ast(line);
    }
|
	expression LE expression
    {
        $$ = new Relational_Expr_Ast($1, $3, $2, int_data_type);
		int line = get_line_number();
		$$->check_ast(line);
    }
|
	expression NE expression
    {
        $$ = new Relational_Expr_Ast($1, $3, $2, int_data_type);
		int line = get_line_number();
		$$->check_ast(line);
    }
|
	expression EQ expression
    {
        $$ = new Relational_Expr_Ast($1, $3, $2, int_data_type);
		int line = get_line_number();
		$$->check_ast(line);
    }
;

arith_expression:
    expression '+' expression
    {
        $$ = new Arithmetic_Expr_Ast($1, $3, 1);
		int line = get_line_number();
		$$->check_ast(line);
    }
|
    expression '-' expression
    {
        $$ = new Arithmetic_Expr_Ast($1, $3, 2);
		int line = get_line_number();
		$$->check_ast(line);
    }
|
    expression '*' expression
    {
        $$ = new Arithmetic_Expr_Ast($1, $3, 3);
		int line = get_line_number();
		$$->check_ast(line);
    }
|
    expression '/' expression
    {
        $$ = new Arithmetic_Expr_Ast($1, $3, 4);
		int line = get_line_number();
		$$->check_ast(line);
    }
;


variable:
	NAME
	{
		Symbol_Table_Entry var_table_entry;
        /* guard */
        if(current_procedure == NULL){
			int line = get_line_number();
			report_error("Corresponding function has never been declared", line);
        }

		else if (current_procedure->variable_in_symbol_list_check(*$1))
			 var_table_entry = current_procedure->get_symbol_table_entry(*$1);

		else if (program_object.variable_in_symbol_list_check(*$1))
			var_table_entry = program_object.get_symbol_table_entry(*$1);

		else
		{
			int line = get_line_number();
			report_error("Variable has not been declared", line);
		}

		$$ = new Name_Ast(*$1, var_table_entry);

		delete $1;
	}
;

constant:
	NUM
	{
		$$ = new Number_Ast<int>($1, int_data_type);
	}
|
    FNUM
	{
		$$ = new Number_Ast<float>($1, float_data_type);
	}
;
