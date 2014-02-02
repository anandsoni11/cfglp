
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

%filenames="scanner"
%lex-source="scanner.cc"

%%

int		{
			store_token_name("INTEGER");
			return Parser::INTEGER; 
		}

return		{ 
			store_token_name("RETURN");
			return Parser::RETURN; 
		}

if		{ 
			store_token_name("IF");
			return Parser::IF; 
		}

else	{ 
			store_token_name("ELSE");
			return Parser::ELSE; 
		}

goto	{ 
			store_token_name("GOTO");
			return Parser::GOTO; 
		}

"="     {
			store_token_name("ASSIGN_OP");
			return Parser::ASSIGN_OP; 
        }

("<bb "([0-9]+)">") {
            store_token_name("BASIC BLOCK");
            ParserBase::STYPE__ * val = getSval();
            std::string bb = matched();
            std::string id = bb.substr(4, bb.length()-5);
            std::cout <<"Basic Block #'" << id <<"'"<<endl;
            val->integer_value = atoi(id.c_str());
            return Parser::BB_ID;
        }

"<"     {
			store_token_name("LT");
            ParserBase::STYPE__ * val = getSval();
            val->integer_value = 0;
			return Parser::LT; 
        }

">"     {
			store_token_name("GT");
            ParserBase::STYPE__ * val = getSval();
            val->integer_value = 1;
			return Parser::GT; 
        }

">="     {
			store_token_name("GE");
            ParserBase::STYPE__ * val = getSval();
            val->integer_value = 2;
			return Parser::GE; 
        }

"<="     {
			store_token_name("LE");
            ParserBase::STYPE__ * val = getSval();
            val->integer_value = 3;
			return Parser::LE; 
        }

"!="     {
			store_token_name("NE");
            ParserBase::STYPE__ * val = getSval();
            val->integer_value = 4;
			return Parser::NE; 
        }

"=="     {
			store_token_name("EQ");
            ParserBase::STYPE__ * val = getSval();
            val->integer_value = 5;
			return Parser::EQ; 
        }


[:{}();]	{
			store_token_name("META CHAR");
			return matched()[0];
		}

[-]?[[:digit:]_]+ 	{ 
				store_token_name("NUM");

				ParserBase::STYPE__ * val = getSval();
				val->integer_value = atoi(matched().c_str());

				return Parser::NUM; 
			}

[[:alpha:]_][[:alpha:][:digit:]_]* {
					store_token_name("NAME");

					ParserBase::STYPE__ * val = getSval();
					val->string_value = new std::string(matched());

					return Parser::NAME; 
				}

\n		{ 
			if (command_options.is_show_tokens_selected())
				ignore_token();
		}    

(";;".*)  	|
([ \t]*"//".*) |
[ \t]		{
			if (command_options.is_show_tokens_selected())
				ignore_token();
		}

.		{ 
			string error_message;
			error_message =  "Illegal character `" + matched();
			error_message += "' on line " + lineNr();
			
			int line_number = lineNr();
			report_error(error_message, line_number);
		}
