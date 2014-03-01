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


%token  NUM /* 257 */
%token BB_ID  /* 258 */
%token FNUM  /* 259 */
%token  NAME  /* 260 */
%token RETURN INTEGER  FLOAT DOUBLE VOID IF ELSE GOTO /* 261 - 267 */

%right ASSIGN_OP /* 265 - 271 */
%left NE EQ
%left LT LE GT GE
%left '+' '-'
%left '*' '/'


/********PRECEDENCE RULES*********/
%start program

%%

program:
	declaration_statement_list 
    function_definition_list
|
	declaration_statement_list 
    function_declaration_list
    function_definition_list
|
    function_declaration_list
    function_definition_list
|
    function_definition_list
;

function_declaration_list:
    function_declaration
|
    function_declaration_list function_declaration
;

function_declaration:
	VOID NAME '(' decl_arg_list ')' ';'
|
	INTEGER NAME '(' decl_arg_list ')' ';'
|
	DOUBLE NAME '(' decl_arg_list ')' ';'
|
	FLOAT NAME '(' decl_arg_list ')' ';'
;

function_definition_list:
|
    function_definition_list function_definition
;

function_definition:
	NAME '(' decl_arg_list ')'
    procedure_body
;

decl_arg_list:
|
    ne_decl_arg_list
;

ne_decl_arg_list:
    ne_decl_arg_list ',' declaration
|
    declaration
;

procedure_body:
	'{' declaration_statement_list
	basic_block_list '}'
|
	'{' basic_block_list '}'
;

declaration_statement_list:
	declaration_statement
|
	declaration_statement_list declaration_statement
;

declaration_statement:
    declaration ';'
;

declaration:
	INTEGER NAME 
|
	DOUBLE NAME 
|
	FLOAT NAME 
;

basic_block_list:
	basic_block_list basic_block
|
	basic_block
;


basic_block:
    BB_ID ':' executable_statement_list
;

executable_statement_list:
	statement_list
|
	statement_list return_statement 
|
	statement_list  if_statement
|
	statement_list goto_statement
;

return_statement:
    RETURN ';'
|
    RETURN expression ';'
;
statement_list:
|
	statement_list general_statement
;

general_statement:
	variable ASSIGN_OP expression ';'
|
    function_call ';'
;

goto_statement:
    GOTO BB_ID ';'
;

if_statement:
    IF '('expression ')'
        goto_statement
    ELSE
        goto_statement
;

expression:
    '(' INTEGER ')'  atomic_expression
|
    '(' FLOAT ')'  atomic_expression
|
    '(' DOUBLE ')'  atomic_expression
|
	rel_expression
|
    arith_expression
|
    unary_expression
;

unary_expression:
    atomic_expression
|
    '-' atomic_expression
;

atomic_expression: /* TODO string */
	variable
|
	constant
|
	'(' expression ')'
|
    function_call
;

function_call:
    NAME '(' arg_list ')'
;

arg_list:
|
    ne_arg_list
;

ne_arg_list:
    ne_arg_list ',' expression
| 
    expression
;

rel_expression:
	expression LT expression
|
	expression GT expression
|
	expression GE expression
|
	expression LE expression
|
	expression NE expression
|
	expression EQ expression
;

arith_expression:
    expression '+' expression
|
    expression '-' expression
|
    expression '*' expression
|
    expression '/' expression
;


variable:
	NAME
;

constant:
	NUM
|
    FNUM
;
