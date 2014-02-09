
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
%token  BB_ID  /* 258 */
%token  FNUM  /* 259 */
%token  NAME  /* 260 */
%token RETURN INTEGER  FLOAT DOUBLE IF ELSE GOTO /* 261 - 267 */

%right ASSIGN_OP /* 265 - 271 */
%left  NE EQ
%left  LT LE GT GE
%left '+' '-'
%left '*' '/'

/********PRECEDENCE RULES*********/
%start program

%%

program:
	declaration_statement_list procedure_name
	procedure_body
|
	procedure_name
	procedure_body
;

procedure_name:
	NAME '(' ')'
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
	INTEGER NAME ';'
|
	FLOAT NAME ';'
|
	DOUBLE NAME ';'
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
	assignment_statement_list
|
	assignment_statement_list RETURN ';'
|
	assignment_statement_list  if_statement
|
	assignment_statement_list goto_statement
;

assignment_statement_list:
|
	assignment_statement_list assignment_statement
;

assignment_statement:
	variable ASSIGN_OP expression ';'
;

goto_statement:
    GOTO BB_ID ';'
;

if_statement:
    IF '(' rel_expression ')'
        goto_statement
    ELSE
        goto_statement
;

expression:
	rel_expression
|
    arith_expression
|
    unary_expression
;

unary_expression:
    '-' unary_expression
|
    atomic_expression
;

atomic_expression: /* TODO string */
	variable
|
	constant
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
