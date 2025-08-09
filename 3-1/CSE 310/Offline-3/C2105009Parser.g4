parser grammar C2105009Parser;

options {
	tokenVocab = C2105009Lexer;
}

@parser::header {
    #include <iostream>
    #include <fstream>
    #include <string>
    #include <cstdlib>
    #include "C2105009Lexer.h"

	#include"SymbolTable/2105009_SymbolTable.h" // for the symbol table
    #include <vector>

	using namespace std;

    extern ofstream parserLogFile;
    extern ofstream errorFile;

    extern int syntaxErrorCount;

	extern SymbolTable symbolTable;
}

@parser::members {
    void writeIntoparserLogFile(const string message) {
        if (!parserLogFile) {
            cout << "Error opening parserLogFile.txt" << endl;
            return;
        }

        parserLogFile << message << endl;
        parserLogFile.flush();
    }

    void writeIntoErrorFile(const string message) {
        if (!errorFile) {
            cout << "Error opening errorFile.txt" << endl;
            return;
        }
        errorFile << message << endl;
        errorFile.flush();
    }

    string currentTypeInDeclarationList;
}

start:
	p = program {
        writeIntoparserLogFile("Line " + to_string($p.stop->getLine()) + ": start : program");
        writeIntoparserLogFile("");
		symbolTable.printAllScopeTables();
		writeIntoparserLogFile("");
        writeIntoparserLogFile("Total number of lines: " + to_string($p.stop->getLine()) + "\n" + "Total number of errors: " + to_string(syntaxErrorCount));
	};

program
	returns[string prog]:
	p = program u = unit {
        writeIntoparserLogFile("Line " + to_string($u.stop->getLine()) + ": program : program unit");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($p.prog + "\n" + $u.unt);
        writeIntoparserLogFile("");

		$prog=$p.prog + "\n" + $u.unt;
    }
	| u = unit {
        writeIntoparserLogFile("Line " + to_string($u.stop->getLine()) + ": program : unit");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($u.unt);
        writeIntoparserLogFile("");

		$prog=$u.unt;
    };

unit
	returns[string unt]:
	vardec = var_declaration {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": unit : var_declaration");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($var_declaration.var_decl);
        writeIntoparserLogFile("");

		$unt=$var_declaration.var_decl;
    }
	| fdec = func_declaration {
        writeIntoparserLogFile("Line " + to_string($fdec.stop->getLine()) + ": unit : func_declaration");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($func_declaration.func_decl);
        writeIntoparserLogFile("");

		$unt=$func_declaration.func_decl;
    }
	| fdef = func_definition {
        writeIntoparserLogFile("Line " + to_string($fdef.stop->getLine()) + ": unit : func_definition");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($fdef.func_def);
        writeIntoparserLogFile("");

		$unt=$fdef.func_def;
    };

func_declaration
	returns[string func_decl]:
	ts = type_specifier id = ID l = LPAREN pl = parameter_list r = RPAREN sm = SEMICOLON {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($ts.type + " " + $id->getText() + $l->getText() + $pl.param_list + $r->getText() + $sm->getText());
        writeIntoparserLogFile("");

		$func_decl = $ts.type + " " + $id->getText() + $l->getText() + $pl.param_list + $r->getText() + $sm->getText();

		if(!symbolTable.lookUp($id->getText())) { 
			symbolTable.insert($id->getText(), "ID", "NULL", $ts.type, (int)$pl.params.size(), $pl.paramTypes); 
		}
    }
	| ts = type_specifier id = ID l = LPAREN r = RPAREN sm = SEMICOLON {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($ts.type + " " + $id->getText() + $l->getText() + $r->getText() + $sm->getText());
        writeIntoparserLogFile("");

		$func_decl=$ts.type + " " + $id->getText() + $l->getText() + $r->getText() + $sm->getText();

		if(!symbolTable.lookUp($id->getText())) { 
			symbolTable.insert($id->getText(), "ID", "NULL", $ts.type, 0, vector<string>{}); 
		}
    };

func_definition
	returns[string func_def]:
	ts = type_specifier id = ID l = LPAREN pl = parameter_list
            r = RPAREN 
             {
                if(symbolTable.lookUp($id->getText())){
                    string functionReturnType = symbolTable.lookUp($id->getText())->getFunctionReturnType();
                    int functionArgumentNumber = symbolTable.lookUp($id->getText())->getFunctionArgumentNumber();
                    if(functionReturnType == "NULL"){
                        writeIntoparserLogFile("Error at line " + to_string($id->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
                        writeIntoErrorFile("Error at line " + to_string($id->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
                        syntaxErrorCount++;
                    }
                    else if(functionReturnType != $ts.type){ 
                        writeIntoparserLogFile("Error at line " + to_string($id->getLine()) + ": Return type mismatch of " + $id->getText() + "\n");
                        writeIntoErrorFile("Error at line " + to_string($id->getLine()) + ": Return type mismatch of " + $id->getText() + "\n");
                        syntaxErrorCount++;
                    }
                    else if(functionArgumentNumber != (int)$pl.params.size()){
                        writeIntoparserLogFile("Error at line " + to_string($id->getLine()) + ": Total number of arguments mismatch with declaration in function " + $id->getText() + "\n");
                        writeIntoErrorFile("Error at line " + to_string($id->getLine()) + ": Total number of arguments mismatch with declaration in function " + $id->getText() + "\n");
                        syntaxErrorCount++;
                    }
                }
                else{
                    symbolTable.insert($id->getText(), "ID", "NULL", $ts.type, (int)$pl.params.size(), $pl.paramTypes); 
                }
            } 
            cs = compound_statement[$pl.params, $pl.paramTypes] {
                if($cs.functionReturned && $ts.type=="void"){
                    writeIntoparserLogFile("Error at line " + to_string($cs.stop->getLine()) + ": Cannot return value from function " + $id->getText() + " with void return type\n");
                    writeIntoErrorFile("Error at line " + to_string($cs.stop->getLine()) + ": Cannot return value from function " + $id->getText() + " with void return type\n");
                    syntaxErrorCount++;
                }
            writeIntoparserLogFile("Line " + to_string($cs.stop->getLine()) + ": func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement");
            writeIntoparserLogFile("");
            writeIntoparserLogFile($ts.type + " " + $id->getText() + $l->getText() + $pl.param_list + $r->getText() + $cs.cmp_stmnt);
            writeIntoparserLogFile("");

            $func_def=$ts.type + " " + $id->getText() + $l->getText() + $pl.param_list + $r->getText() + $cs.cmp_stmnt;
    }
	| ts = type_specifier id = ID l = LPAREN 
        r = RPAREN 
        {
            if(symbolTable.lookUp($id->getText())){
                    string functionReturnType = symbolTable.lookUp($id->getText())->getFunctionReturnType();
                    int functionArgumentNumber = symbolTable.lookUp($id->getText())->getFunctionArgumentNumber();
                    if(functionReturnType == "NULL"){
                        writeIntoparserLogFile("Error at line " + to_string($id->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
                        writeIntoErrorFile("Error at line " + to_string($id->getLine()) + ": Multiple declaration " + $id->getText() + "\n");
                    }
                    else if(functionReturnType != $ts.type){ 
                        writeIntoparserLogFile("Error at line " + to_string($id->getLine()) + ": Return type mismatch of " + $id->getText() + "\n");
                        writeIntoErrorFile("Error at line " + to_string($id->getLine()) + ": Return type mismatch of " + $id->getText() + "\n");
                        syntaxErrorCount++;
                    }
                    else if(functionArgumentNumber != 0){
                        writeIntoparserLogFile("Error at line " + to_string($id->getLine()) + ": Total number of arguments mismatch with declaration in function " + $id->getText() + "\n");
                        writeIntoErrorFile("Error at line " + to_string($id->getLine()) + ": Total number of arguments mismatch with declaration in function " + $id->getText() + "\n");
                        syntaxErrorCount++;
                    }
            }
            else{
                symbolTable.insert($id->getText(), "ID", "NULL", $ts.type, 0, vector<string>{}); 
            }
        } 
        cs = compound_statement[vector<string>{}, vector<string>{}] {
            if($cs.functionReturned && $ts.type=="void"){
                writeIntoparserLogFile("Error at line " + to_string($cs.stop->getLine()) + ": Cannot return value from function " + $id->getText() + " with void return type\n");
                writeIntoErrorFile("Error at line " + to_string($cs.stop->getLine()) + ": Cannot return value from function " + $id->getText() + " with void return type\n");
                syntaxErrorCount++;
            }
		writeIntoparserLogFile("Line " + to_string($cs.stop->getLine()) + ": func_definition : type_specifier ID LPAREN RPAREN compound_statement");
		writeIntoparserLogFile("");
		writeIntoparserLogFile($ts.type + " " + $id->getText() + $l->getText() + $r->getText() + $cs.cmp_stmnt);
		writeIntoparserLogFile("");

		$func_def=$ts.type + " " + $id->getText() + $l->getText() + $r->getText() + $cs.cmp_stmnt;
    };

parameter_list
	returns[string param_list, vector <string> params, vector <string> paramTypes]:
	p = parameter_list c = COMMA ts = type_specifier id = ID {
        $params = $p.params;
        $paramTypes = $p.paramTypes;

        auto it = find($params.begin(), $params.end(), $id->getText());
        if(it != $params.end()){
            writeIntoparserLogFile("Error at line "+ to_string($id->getLine()) + ": Multiple declaration of " + $id->getText() + " in parameter\n");
            writeIntoErrorFile("Error at line "+ to_string($id->getLine()) + ": Multiple declaration of " + $id->getText() + " in parameter\n");
            syntaxErrorCount++;
        }
        else{
            $params.push_back($id->getText());
        }
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": parameter_list : parameter_list COMMA type_specifier ID");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($p.param_list + $c->getText() + $ts.type + " " + $id->getText());
        writeIntoparserLogFile("");

		$param_list = $p.param_list + $c->getText() + $ts.type + " " + $id->getText();
        $paramTypes.push_back($ts.type);
    }
	| p = parameter_list c = COMMA ts = type_specifier {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": parameter_list : parameter_list COMMA type_specifier");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($p.param_list + $c->getText() + $ts.type);
        writeIntoparserLogFile("");

		$param_list = $p.param_list + $c->getText() + $ts.type;
        $params = $p.params;
        $paramTypes = $p.paramTypes;
    }
	| ts = type_specifier id = ID {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": parameter_list : type_specifier ID");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($ts.type + " " + $id->getText());
        writeIntoparserLogFile("");

		$param_list = $ts.type + " " + $id->getText();

        $params.push_back($id->getText());
        $paramTypes.push_back($ts.type);
    }
	| ts = type_specifier {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": parameter_list : type_specifier");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($ts.type);
        writeIntoparserLogFile("");

		$param_list = $ts.type;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    | ts = type_specifier add = ADDOP {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": parameter_list : type_specifier\n");
        writeIntoparserLogFile($ts.type + "\n");
        writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ADDOP, expecting RPAREN or COMMA\n");
        writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ADDOP, expecting RPAREN or COMMA\n");
        syntaxErrorCount++;

        $param_list = $ts.type;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ;

compound_statement[vector<string> functionParams, vector<string> paramTypes]
	returns[string cmp_stmnt, int functionReturned]:
	l = LCURL {
        symbolTable.enterScope();

        for (int i=0; i<(int)functionParams.size(); i++) {
            symbolTable.insert(functionParams[i], "ID", paramTypes[i]);
        }} sts = statements r = RCURL {
        writeIntoparserLogFile("Line " + to_string($r->getLine()) + ": compound_statement : LCURL statements RCURL");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($l->getText() + "\n" + $sts.stmnts + "\n" + $r->getText());
        writeIntoparserLogFile("");

		$cmp_stmnt = $l->getText() + "\n" + $sts.stmnts + "\n" + $r->getText();

        $functionReturned=$sts.functionReturned;

		symbolTable.printAllScopeTables();
		writeIntoparserLogFile("");

        symbolTable.exitScope();

    }
	| l = LCURL {symbolTable.enterScope();} r = RCURL {
        writeIntoparserLogFile("Line " + to_string($r->getLine()) + ": compound_statement : LCURL RCURL");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($l->getText() + $r->getText());
        writeIntoparserLogFile("");

		$cmp_stmnt = $l->getText() + $r->getText();

        $functionReturned=0;

		symbolTable.printAllScopeTables();
		writeIntoparserLogFile("");

        symbolTable.exitScope();
    };

var_declaration
	returns[string var_decl]:
	t = type_specifier {currentTypeInDeclarationList=$t.type;} dl = declaration_list sm = SEMICOLON {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": var_declaration : type_specifier declaration_list SEMICOLON\n");
        if($t.type=="void") {
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Variable type cannot be void\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Variable type cannot be void\n");
            syntaxErrorCount++;
        }
        writeIntoparserLogFile($t.type+ " " + $dl.dl_list + $sm->getText() + "\n");

		$var_decl = $t.type+ " " + $dl.dl_list + $sm->getText();
        

        // writeIntoparserLogFile(
        //     string("Variable Declaration: type_specifier declaration_list ") +
        //     to_string($sm->getType()) +
        //     " at line " + to_string($sm->getLine())
        // );

        // writeIntoparserLogFile("type_specifier type: " + $t.type);
      }
	| t = type_specifier de = declaration_list_err sm = SEMICOLON {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": var_declaration : type_specifier declaration_list_err SEMICOLON");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($ctx->getText());
        writeIntoparserLogFile("");
        writeIntoErrorFile(
            string("Line# ") + to_string($sm->getLine()) +
            " with error name: " + $de.error_name +
            " - Syntax error at declaration list of variable declaration"
        );

        syntaxErrorCount++;
      };

declaration_list_err
	returns[string error_name]:
	{
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list_err : (empty)");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($ctx->getText());
        writeIntoparserLogFile("");
        $error_name = "Error in declaration list";
    };

type_specifier
	returns[string type]:
	INT {
            writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": type_specifier : INT");
            writeIntoparserLogFile("");
            writeIntoparserLogFile($ctx->getText());
            writeIntoparserLogFile("");

            // $type = "type: INT at line" + to_string($INT->getLine());
			$type = "int";
        }
	| FLOAT {
            writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": type_specifier : FLOAT");
            writeIntoparserLogFile("");
            writeIntoparserLogFile($ctx->getText());
            writeIntoparserLogFile("");

            // $type = "type: FLOAT at line" + to_string($FLOAT->getLine());
			$type = "float";
        }
	| VOID {
            writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": type_specifier : VOID");
            writeIntoparserLogFile("");
            writeIntoparserLogFile($ctx->getText());
            writeIntoparserLogFile("");

            // $type = "type: VOID at line" + to_string($VOID->getLine());
			$type = "void";
        };

// declaration_list
// 	returns[string dl_list]:
// 	dl = declaration_list c = COMMA id = ID {
//         writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : declaration_list COMMA ID");
//         writeIntoparserLogFile("");
//         writeIntoparserLogFile($dl.dl_list + $c->getText() + $id->getText());
//         writeIntoparserLogFile("");

// 		$dl_list = $dl.dl_list + $c->getText() + $id->getText();

// 		if(!symbolTable.lookUp($id->getText())) { 
// 			symbolTable.insert($id->getText(), "ID"); 
// 		}
//     }
// 	| dl = declaration_list c = COMMA id = ID l3 = LTHIRD cint = CONST_INT r3 = RTHIRD {
//         writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD");
//         writeIntoparserLogFile("");
//         writeIntoparserLogFile($dl.dl_list+ $c->getText() + $id->getText() + $l3->getText() + $cint->getText() + $r3->getText());
//         writeIntoparserLogFile("");

// 		$dl_list = $dl.dl_list+ $c->getText() + $id->getText() + $l3->getText() + $cint->getText() + $r3->getText();

// 		if(!symbolTable.lookUp($id->getText())) { 
// 			symbolTable.insert($id->getText(), "ID"); 
// 		}
//     }
// 	| id = ID {
//         writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : ID");
//         writeIntoparserLogFile("");
//         writeIntoparserLogFile($id->getText());
//         writeIntoparserLogFile("");

// 		if(!symbolTable.lookUp($id->getText())) { 
// 			symbolTable.insert($id->getText(), "ID"); 
// 		}

// 		$dl_list = $id->getText();
//     }
// 	| id = ID l3 = LTHIRD cint = CONST_INT r3 = RTHIRD {
//         writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : ID LTHIRD CONST_INT RTHIRD");
//         writeIntoparserLogFile("");
//         writeIntoparserLogFile($id->getText() + $l3->getText() + $cint->getText() + $r3->getText());
//         writeIntoparserLogFile("");

// 		$dl_list = $id->getText() + $l3->getText() + $cint->getText() + $r3->getText();

// 		if(!symbolTable.lookUp($id->getText())) { 
// 			symbolTable.insert($id->getText(), "ID"); 
// 		}
//     }
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     id1 = ID add=ADDOP id2 = ID{
//         writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ADDOP, expecting COMMA or SEMICOLON");
//         writeIntoparserLogFile("");

//         $dl_list = $id1->getText();
//     }
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     ;


declaration_list
	returns[string dl_list]:
	dl = declaration_list c = COMMA id = ID {
        if(!symbolTable.lookUp($id->getText())) {
			symbolTable.insert($id->getText(), "ID", currentTypeInDeclarationList); 
		}
        else{
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
            syntaxErrorCount++;
        }

        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : declaration_list COMMA ID\n");
        writeIntoparserLogFile($dl.dl_list + $c->getText() + $id->getText() + "\n");

		$dl_list = $dl.dl_list + $c->getText() + $id->getText();
    }
	| dl = declaration_list c = COMMA id = ID l3 = LTHIRD cint = CONST_INT r3 = RTHIRD {
        if(!symbolTable.lookUp($id->getText())) {
			symbolTable.insert($id->getText(), "ID", currentTypeInDeclarationList + " array"); 
		}
        else{
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
            syntaxErrorCount++;
        }

        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD\n");
        writeIntoparserLogFile($dl.dl_list+ $c->getText() + $id->getText() + $l3->getText() + $cint->getText() + $r3->getText() + "\n");

		$dl_list = $dl.dl_list+ $c->getText() + $id->getText() + $l3->getText() + $cint->getText() + $r3->getText();
    }
    // Handle error case with ADDOP - placed before the base cases
    | dl = declaration_list c = COMMA id1 = ID add = ADDOP id2 = ID {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : declaration_list COMMA ID\n");
        writeIntoparserLogFile($dl.dl_list + $c->getText() + $id1->getText() + "\n");
        
        writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ADDOP, expecting COMMA or SEMICOLON\n");
        writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ADDOP, expecting COMMA or SEMICOLON\n");
        syntaxErrorCount++;
        

        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : declaration_list COMMA ID\n");
        writeIntoparserLogFile($dl.dl_list + $c->getText() + $id1->getText() + $c->getText() + $id2->getText() + "\n");

        $dl_list = $dl.dl_list + $c->getText() + $id1->getText() + $c->getText() + $id2->getText();

        if(!symbolTable.lookUp($id1->getText())) {
			symbolTable.insert($id1->getText(), "ID", currentTypeInDeclarationList); 
		}
    }
    // Handle error case for first ID with ADDOP
    | id1 = ID add = ADDOP id2 = ID {
        writeIntoparserLogFile("Line " + to_string($id1->getLine()) + ": declaration_list : ID\n");
        writeIntoparserLogFile($id1->getText() + "\n");
        
        writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ADDOP, expecting COMMA or SEMICOLON\n");
        writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ADDOP, expecting COMMA or SEMICOLON\n");
        syntaxErrorCount++;

        $dl_list = $id1->getText();

        if(!symbolTable.lookUp($id1->getText())) { 
			symbolTable.insert($id1->getText(), "ID", currentTypeInDeclarationList); 
		}
    }
	| id = ID {
        if(!symbolTable.lookUp($id->getText())) {
			symbolTable.insert($id->getText(), "ID", currentTypeInDeclarationList); 
		}
        else{
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
            syntaxErrorCount++;
        }

        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : ID\n");
        writeIntoparserLogFile($id->getText() + "\n");

		$dl_list = $id->getText();
    }
	| id = ID l3 = LTHIRD cint = CONST_INT r3 = RTHIRD {
        if(!symbolTable.lookUp($id->getText())) {
			symbolTable.insert($id->getText(), "ID", currentTypeInDeclarationList+ " array");
		}
        else{
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Multiple declaration of " + $id->getText() + "\n");
            syntaxErrorCount++;
        }

        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": declaration_list : ID LTHIRD CONST_INT RTHIRD\n");
        writeIntoparserLogFile($id->getText() + $l3->getText() + $cint->getText() + $r3->getText() + "\n");

		$dl_list = $id->getText() + $l3->getText() + $cint->getText() + $r3->getText();
    }
    ;


statements
	returns[string stmnts, int errflag, int functionReturned]:
	st = statement {
        if(!$st.errflag){
            writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": statements : statement");
            writeIntoparserLogFile("");
            writeIntoparserLogFile($st.stmnt);
            writeIntoparserLogFile("");
        }
         $stmnts = $st.stmnt;
         $st.errflag=0;
         $functionReturned=$st.functionReturned;
    }
	| sts = statements st = statement {
        if(!$st.errflag){
            writeIntoparserLogFile("Line " + to_string($st.stop->getLine()) + ": statements : statements statement");
            writeIntoparserLogFile("");
            writeIntoparserLogFile($sts.stmnts + "\n" + $st.stmnt);
            writeIntoparserLogFile("");
        }
         $stmnts = $sts.stmnts + ($st.errflag? "" : "\n")+ $st.stmnt;
         $st.errflag=0;
         $functionReturned=$st.functionReturned;
    };

statement
	returns[string stmnt, int errflag, int functionReturned]:
	vd = var_declaration {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": statement : var_declaration");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($vd.var_decl);
        writeIntoparserLogFile("");

		$stmnt = $vd.var_decl;
        $errflag = 0;
        $functionReturned = 0;
    }
	| es = expression_statement {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": statement : expression_statement");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($es.exp_stmnt);
        writeIntoparserLogFile("");

		$stmnt = $es.exp_stmnt;
        $errflag = 0;
        $functionReturned = 0;
    }
	| cs = compound_statement[vector<string>{}, vector<string>{}] {
        writeIntoparserLogFile("Line " + to_string($cs.stop->getLine()) + ": statement : compound_statement");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($cs.cmp_stmnt);
        writeIntoparserLogFile("");

		$stmnt = $cs.cmp_stmnt;
        $errflag = 0;
        $functionReturned = 0;
    }
	| for = FOR l = LPAREN es1 = expression_statement es2 = expression_statement e = expression r =
		RPAREN st = statement {
        writeIntoparserLogFile("Line " + to_string($st.stop->getLine()) + ": statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($for->getText() + $l->getText() + $es1.exp_stmnt + $es2.exp_stmnt + $e.exp + $r->getText() + $st.stmnt);
        writeIntoparserLogFile("");

		$stmnt = $for->getText() + $l->getText() + $es1.exp_stmnt + $es2.exp_stmnt + $e.exp + $r->getText() + $st.stmnt;
        $errflag = 0;
        $functionReturned = 0;
    }
	| if = IF l = LPAREN e = expression r = RPAREN st = statement {
        writeIntoparserLogFile("Line " + to_string($st.stop->getLine()) + ": statement : IF LPAREN expression RPAREN statement");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($if->getText() + $l->getText() + $e.exp + $r->getText() + $st.stmnt);
        writeIntoparserLogFile("");

		$stmnt = $if->getText() + $l->getText() + $e.exp + $r->getText() + $st.stmnt;
        $errflag = 0;
        $functionReturned = 0;
    }
	| if = IF l = LPAREN e = expression r = RPAREN st1 = statement else = ELSE st2 = statement {
        writeIntoparserLogFile("Line " + to_string($st2.stop->getLine()) + ": statement : IF LPAREN expression RPAREN statement ELSE statement");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($if->getText() + $l->getText() + $e.exp + $r->getText() + $st1.stmnt + $else->getText() + " " + $st2.stmnt);
        writeIntoparserLogFile("");

		$stmnt = $if->getText() + $l->getText() + $e.exp + $r->getText() + $st1.stmnt + $else->getText() + " " + $st2.stmnt;
        $errflag = 0;
        $functionReturned = 0;
    }
	| while = WHILE l = LPAREN e = expression r = RPAREN st = statement {
        writeIntoparserLogFile("Line " + to_string($st.stop->getLine()) + ": statement : WHILE LPAREN expression RPAREN statement");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($while->getText() + $l->getText() + $e.exp + $r->getText() + $st.stmnt);
        writeIntoparserLogFile("");

		$stmnt = $while->getText() + $l->getText() + $e.exp + $r->getText() + $st.stmnt;
        $errflag = 0;
        $functionReturned = 0;
    }
	// | print = PRINTLN l = LPAREN id = ID r = RPAREN sm = SEMICOLON { writeIntoparserLogFile("Line
	// " + to_string($ctx->getStart()->getLine()) + ": statement : PRINTLN LPAREN ID RPAREN
	// SEMICOLON"); writeIntoparserLogFile(""); writeIntoparserLogFile($print->getText() +
	// $l->getText() + $id->getText() + $r->getText() + $sm->getText()); writeIntoparserLogFile("");

	// $stmnt = $print->getText() + $l->getText() + $id->getText() + $r->getText() + $sm->getText();
	// }
	| print = PRINTF l = LPAREN id = ID r = RPAREN sm = SEMICOLON {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": statement : PRINTLN LPAREN ID RPAREN SEMICOLON\n");

        if(!symbolTable.lookUpAll($id->getText())){
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Undeclared variable " + $id->getText() + "\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Undeclared variable " + $id->getText() + "\n");
            syntaxErrorCount++;
        }

        writeIntoparserLogFile($print->getText() + $l->getText() + $id->getText() + $r->getText() + $sm->getText() + "\n");

		$stmnt = $print->getText() + $l->getText() + $id->getText() + $r->getText() + $sm->getText();
        $errflag = 0;
        $functionReturned = 0;
    }
	| ret = RETURN e = expression sm = SEMICOLON {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": statement : RETURN expression SEMICOLON");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($ret->getText() + " " + $e.exp + $sm->getText());
        writeIntoparserLogFile("");

		$stmnt = $ret->getText() + " " + $e.exp + $sm->getText();
        $errflag = 0;
        $functionReturned = 1;
    }
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	| ec = ERROR_CHAR {
        writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Unrecognized character " + $ec->getText() + "\n");
        writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Unrecognized character " + $ec->getText() + "\n");
        syntaxErrorCount++;
        $stmnt = "";
        $errflag = 1;
        $functionReturned = 0;
    }
    | v = variable 
        asgn = ASSIGNOP 
        simple_expression {
        writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ASSIGNOP\n");
        writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ASSIGNOP\n");
        syntaxErrorCount++;
        } 
        ADDOP 
        ASSIGNOP 
        expression{ 
        $errflag = 1;
        $stmnt = "";
        $functionReturned = 0;
	}
	; 
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

expression_statement
	returns[string exp_stmnt]:
	sm = SEMICOLON {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": expression_statement : SEMICOLON");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($sm->getText());
        writeIntoparserLogFile("");

		$exp_stmnt = $sm->getText();
    }
	| e = expression sm = SEMICOLON {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": expression_statement : expression SEMICOLON");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($e.exp + $sm->getText());
        writeIntoparserLogFile("");

		$exp_stmnt = $e.exp + $sm->getText();
    }
    ;

variable
	returns[string var, string type, string var_id]:
	id = ID {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": variable : ID\n");

        if(!symbolTable.lookUpAll($id->getText())){
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Undeclared variable " + $id->getText() + "\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Undeclared variable " + $id->getText() + "\n");
            syntaxErrorCount++;
        }
        if(symbolTable.lookUpAll($id->getText())){
            if(symbolTable.lookUpAll($id->getText())->getVarDataType().find("array")!=string::npos){
                writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Type mismatch, " + $id->getText() + " is an array\n");
                writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Type mismatch, " + $id->getText() + " is an array\n");
                syntaxErrorCount++;
            }
        }
        // else{
        //     writeIntoparserLogFile("Type: " + symbolTable.lookUp($id->getText())->getVarDataType() + " Name: " + symbolTable.lookUp($id->getText())->getName() +"\n");
        //     // if(symbolTable.lookUp($id->getText())->getVarDataType() !=)
        // }

        writeIntoparserLogFile($id->getText() + "\n");

		$var=$id->getText();
        if(symbolTable.lookUpAll($id->getText())){
            $type=symbolTable.lookUpAll($id->getText())->getVarDataType();
        }
        $var_id=$id->getText();
    }
	| id = ID l3 = LTHIRD e = expression r3 = RTHIRD {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": variable : ID LTHIRD expression RTHIRD\n");
        if(symbolTable.lookUpAll($id->getText())){
            if(symbolTable.lookUpAll($id->getText())->getVarDataType().find("array") == string::npos){
                writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": " + $id->getText() + " not an array\n");
                writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": " + $id->getText() + " not an array\n");
                syntaxErrorCount++;
            }
            string varDataType=symbolTable.lookUpAll($id->getText())->getVarDataType();
            $type=varDataType;
            if (varDataType.length() >= 5 && varDataType.substr(varDataType.length() - 5) == "array") {
                $type=varDataType.erase(varDataType.length() - 6);
            }
        }
        if($e.type != "int"){
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Expression inside third brackets not an integer\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Expression inside third brackets not an integer\n");
            syntaxErrorCount++;
        }

        writeIntoparserLogFile($id->getText() + $l3->getText() + $e.exp + $r3->getText() + "\n");

		$var = $id->getText() + $l3->getText() + $e.exp + $r3->getText();
        $var_id=$id->getText();
    };

expression
	returns[string exp, string type]:
	le = logic_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": expression : logic_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($le.lexp);
        writeIntoparserLogFile("");

		$exp=$le.lexp;
        $type=$le.type;
    }
	| v = variable asgn = ASSIGNOP le = logic_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": expression : variable ASSIGNOP logic_expression");
        if(symbolTable.lookUpAll($le.singleFuncName)){
            if(symbolTable.lookUpAll($le.singleFuncName)->getFunctionReturnType()=="void"){
                writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Void function used in expression\n");
                writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Void function used in expression\n");
                syntaxErrorCount++;
            }
        }
        if($v.type.find("array") != string::npos && $v.type.find($le.type) == string::npos){
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Type Mismatch\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Type Mismatch\n");
            syntaxErrorCount++;
        }
        else if($v.type.find("array") == string::npos && 
            symbolTable.lookUpAll($v.var_id) &&
            $v.type!="float" &&
            $v.type!=$le.type && $le.val!=""){
                writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Type Mismatch\n");
                writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Type Mismatch\n");
                syntaxErrorCount++;
        }

        writeIntoparserLogFile($v.var + $asgn->getText() + $le.lexp + "\n");

		$exp=$v.var + $asgn->getText() + $le.lexp;
        $type="";
    }
	// | v = variable asgn = ASSIGNOP e = expression { writeIntoparserLogFile("Line " +
	// to_string($ctx->getStart()->getLine()) + ": expression : variable ASSIGNOP expression");
	// writeIntoparserLogFile(""); writeIntoparserLogFile($v.var + $asgn->getText() + $e.exp);
	// writeIntoparserLogFile("");

	// $exp=$v.var + $asgn->getText() + $e.exp; }
    ;

logic_expression
	returns[string lexp, string type, string singleFuncName, string val]:
	re = rel_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": logic_expression : rel_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($re.rlexp);
        writeIntoparserLogFile("");

		$lexp = $re.rlexp;
        $type= $re.type;
        $singleFuncName=$re.singleFuncName;
        $val=$re.val;
    }
	| re1 = rel_expression lg = LOGICOP re2 = rel_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": logic_expression : rel_expression LOGICOP rel_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($re1.rlexp + $lg->getText() + $re2.rlexp);
        writeIntoparserLogFile("");

		$lexp = $re1.rlexp + $lg->getText() + $re2.rlexp;
        $type="";
        $singleFuncName="";
        $val="";
    };

rel_expression
	returns[string rlexp, string type, string singleFuncName, string val]:
	se = simple_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": rel_expression : simple_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($se.sexp);
        writeIntoparserLogFile("");

		$rlexp = $se.sexp;
        $type=$se.type;
        $singleFuncName=$se.singleFuncName;
        $val=$se.val;
    }
	| se1 = simple_expression rl = RELOP se2 = simple_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": rel_expression : simple_expression RELOP simple_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($se1.sexp + $rl->getText() + $se2.sexp);
        writeIntoparserLogFile("");

		$rlexp = $se1.sexp + $rl->getText() + $se2.sexp;
        $type="";
        $singleFuncName="";
        $val="";
    };

simple_expression
	returns[string sexp, string type, string singleFuncName, string val]:
	t = term {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": simple_expression : term");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($t.trm);
        writeIntoparserLogFile("");

		$sexp = $t.trm;
        $type=$t.type;
        $singleFuncName=$t.singleFuncName;
        $val=$t.val;
    }
	| se = simple_expression add = ADDOP t = term {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": simple_expression : simple_expression ADDOP term");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($se.sexp + $add->getText() + $t.trm);
        writeIntoparserLogFile("");

		$sexp = $se.sexp + $add->getText() + $t.trm;
        $type=$t.type;
        $singleFuncName="";
        $val="";
    }
    // //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // | term ADDOP error_expr {
    //     writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": syntax error, unexpected ASSIGNOP");
    //     syntaxErrorCount++;
    //     $sexp="";
    // }
    // //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ;

term
	returns[string trm, string type, string singleFuncName, string val]:
	ue = unary_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": term : unary_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($ue.uexp);
        writeIntoparserLogFile("");

		$trm = $ue.uexp;
        $type = $ue.type;
        $singleFuncName = $ue.singleFuncName;
        $val=$ue.val;
    }
	| t = term mul = MULOP ue = unary_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": term : term MULOP unary_expression\n");

        if(symbolTable.lookUpAll($ue.singleFuncName)){
            if(symbolTable.lookUpAll($ue.singleFuncName)->getFunctionReturnType()=="void"){
                writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Void function used in expression\n");
                writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Void function used in expression\n");
                syntaxErrorCount++;
            }
        }
        if($mul->getText() == "%" && $ue.val!="" && stod($ue.val)==0){
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Modulus by Zero\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Modulus by Zero\n");
            syntaxErrorCount++;
        }
        if($mul->getText() == "%" && $ue.val!="" && stod($ue.val)!=(int)stod($ue.val)){
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Non-Integer operand on modulus operator\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Non-Integer operand on modulus operator\n");
            syntaxErrorCount++;
        }
        writeIntoparserLogFile($t.trm + $mul->getText() + $ue.uexp + "\n");

		$trm = $t.trm + $mul->getText() + $ue.uexp;
        $type = $ue.type;
        $singleFuncName = "";
        $val="";
    };

unary_expression
	returns[string uexp, string type, string singleFuncName, string val]:
	add = ADDOP ue = unary_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": unary_expression : ADDOP unary_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($add->getText() + $ue.uexp);
        writeIntoparserLogFile("");

		$uexp = $add->getText() + $ue.uexp;
        $type="";
        $singleFuncName="";
        $val="";
    }
	| not = NOT ue = unary_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": unary_expression : NOT unary_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($not->getText() + $ue.uexp);
        writeIntoparserLogFile("");

		$uexp = $not->getText() + $ue.uexp;
        $type="";
        $singleFuncName="";
        $val="";
    }
	| f = factor {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": unary_expression : factor");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($f.fact);
        writeIntoparserLogFile("");

		$uexp = $f.fact;
        $type= $f.type;
        $singleFuncName=$f.singleFuncName;
        $val=$f.val;
    };

factor
	returns[string fact, string type, string singleFuncName, string val]:
	v = variable {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": factor : variable");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($v.var);
        writeIntoparserLogFile("");

		$fact = $v.var;
        $type=$v.type;
        $singleFuncName="";
        $val=$v.type;
    }
	| id = ID l = LPAREN al = argument_list r = RPAREN {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": factor : ID LPAREN argument_list RPAREN\n");

        if(symbolTable.lookUpAll($id->getText())){
            if(symbolTable.lookUpAll($id->getText())->getFunctionReturnType() != "NULL"){
                vector<string>functionParams=symbolTable.lookUpAll($id->getText())->getParameterTypes();
                if(functionParams.size() != $al.paramTypes.size()){
                    writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Total number of arguments mismatch with declaration in function " + $id->getText() + "\n");
                    writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Total number of arguments mismatch with declaration in function " + $id->getText() + "\n");
                    syntaxErrorCount++;
                }
                else{

                    for(int i=0; i<(int)functionParams.size(); i++){
                        if($al.paramTypes[i].find("array")==string::npos && functionParams[i] != $al.paramTypes[i]){
                            writeIntoparserLogFile("Types are: " + functionParams[i] + " and " + $al.paramTypes[i]);
                            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": " + to_string(i+1) + "th argument mismatch in function " + $id->getText() + "\n");
                            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": " + to_string(i+1) + "th argument mismatch in function " + $id->getText() + "\n");
                            syntaxErrorCount++;
                            break;
                        }
                }
                }
                
            }
        }
        else{
            writeIntoparserLogFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Undefined function " + $id->getText() + "\n");
            writeIntoErrorFile("Error at line " + to_string($ctx->getStart()->getLine()) + ": Undefined function " + $id->getText() + "\n");
            syntaxErrorCount++;
        }

        writeIntoparserLogFile($id->getText() + $l->getText() + $al.arg_list + $r->getText() + "\n");

		$fact = $id->getText() + $l->getText() + $al.arg_list + $r->getText();
        $type="";
        $singleFuncName=$id->getText();
        $val="";
    }
	| l = LPAREN e = expression r = RPAREN {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": factor : LPAREN expression RPAREN");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($l->getText() + $e.exp + $r->getText());
        writeIntoparserLogFile("");

		$fact = $l->getText() + $e.exp + $r->getText();
        $type="";
        $singleFuncName="";
        $val="";
    }
	| cint = CONST_INT {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": factor : CONST_INT");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($cint->getText());
        writeIntoparserLogFile("");

		$fact = $cint->getText();
        $type="int";
        $singleFuncName="";
        $val=$cint->getText();
    }
	| cf = CONST_FLOAT {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": factor : CONST_FLOAT");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($cf->getText());
        writeIntoparserLogFile("");

		$fact = $cf->getText();
        $type="float";
        $singleFuncName="";
        $val=$cf->getText();
    }
	| v = variable inc = INCOP {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": factor : variable INCOP");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($v.var + $inc->getText());
        writeIntoparserLogFile("");

		$fact = $v.var + $inc->getText();
        $type="";
        $singleFuncName="";
        $val="";
    }
	| v = variable dec = DECOP {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": factor : variable DECOP");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($v.var + $dec->getText());
        writeIntoparserLogFile("");

		$fact = $v.var + $dec->getText();
        $type="";
        $singleFuncName="";
        $val="";
    };

argument_list
	returns[string arg_list, vector<string>paramTypes]:
	a = arguments {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": argument_list : arguments");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($a.args);
        writeIntoparserLogFile("");

		$arg_list = $a.args;
        $paramTypes=$a.paramTypes;
    }
	| {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": argument_list : (empty)");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($ctx->getText());
        writeIntoparserLogFile("");

        $paramTypes=vector<string>{};
    };

arguments
	returns[string args, vector<string>paramTypes]:
	a = arguments c = COMMA le = logic_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": arguments : arguments COMMA logic_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($a.args + $c->getText() + $le.lexp);
        writeIntoparserLogFile("");

		$args = $a.args + $c->getText() + $le.lexp;

        $paramTypes=$a.paramTypes;
        $paramTypes.push_back($le.type);
    }
	| le = logic_expression {
        writeIntoparserLogFile("Line " + to_string($ctx->getStart()->getLine()) + ": arguments : logic_expression");
        writeIntoparserLogFile("");
        writeIntoparserLogFile($le.lexp);
        writeIntoparserLogFile("");

		$args = $le.lexp;

        $paramTypes.push_back($le.type);
    };

////////////////////////////////////////////  new rules ////////////////////////////////////////////

error_expr:  ADDOP;