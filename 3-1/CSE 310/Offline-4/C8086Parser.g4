parser grammar C8086Parser;

options {
	tokenVocab = C8086Lexer;
}

@parser::header {
    #include <iostream>
    #include <fstream>
	#include <sstream>
    #include <string>
    #include <cstdlib>
    #include "C8086Lexer.h"

	#include"SymbolTable/2105009_SymbolTable.h" // for the symbol table

	#include <stack>
	#include <map>

	using namespace std;

    extern ofstream parserLogFile;
    // extern ofstream errorFile;
	extern ofstream assemblyFile;

    // extern int syntaxErrorCount;
	extern SymbolTable symbolTable;
}

@parser::members {
    // void writeIntoparserLogFile(const string message) {
    //     if (!parserLogFile) {
    //         cout << "Error opening parserLogFile.txt" << endl;
    //         return;
    //     }

    //     parserLogFile << message << endl;
    //     parserLogFile.flush();
    // }

    void writeIntoAssemblyFile(const string code) {
        if (!assemblyFile) {
            cout << "Error opening code.asm" << endl;
            return;
        }
        assemblyFile << code << endl;
        assemblyFile.flush();
    }

	string readFileToString(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			throw std::runtime_error("Could not open file: " + filename);
		}
		
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	void emptyStack(std::stack<int>& s) {
		while (!s.empty()) {
			s.pop();
		}
	}

	string currentTypeInDeclarationList="";
	int codeSegmentLineCount=0; // to keep track of linecount
	int stackBasePointerOffset=0; // to keep track of stack base pointer offset
	bool inMain=false;
	bool insideConditionalStatement=false;
	bool arithmeticCalculationAssignment=false;
	int localVariableDeclaredInMain=0;
	bool ifElseBlockCompleted=false;
	bool insideForLoopConditional=false;
	bool insideForLoop=false;
	bool insideWhileLoop=false;
	bool insideWhileLoopConditional=false;
	bool insideConditional=false;

	int truthLabel=0;
	int falseLabel=0;
	int nextLabel=0;

	int finalTruthLabel=0;
	int finalFalseLabel=0;

	stack <int> truthLabels;
	stack <int> falseLabels;

	stack <int> ifElseCompletionLabel;

	map <string,int> functionParamsMap;
	bool insideFunction=false;
	int parameterOffset=2;

	int currentLabel=0;

	bool codeSegmentDeclared = false;

	bool dxContainsValue = false;

	int localVariableDecalredInFunction = 0;

	bool returningFromOtherFunctions = false;

	bool returnValueInStack = false;

	int functionParameterStackBasePointerOffset = 0;

	stack <int> lastLabelInserted;

	int returnLabel;
	bool returnLabelAssigned=false;
}

start:
	{
		writeIntoAssemblyFile(".MODEL SMALL");
		writeIntoAssemblyFile(".STACK 1000H");
		writeIntoAssemblyFile(".Data");
		writeIntoAssemblyFile("\tnumber DB \"00000$\"");
	} program {
	};

program: program unit | unit;

unit: var_declaration | func_declaration | func_definition;

func_declaration:
	type_specifier ID LPAREN parameter_list RPAREN SEMICOLON
	| type_specifier ID LPAREN RPAREN SEMICOLON;

func_definition:
	type_specifier ID {
		bool isMain = ($ID->getText() == "main");
		inMain=isMain;

		if(!inMain){
			if(!codeSegmentDeclared) {
				writeIntoAssemblyFile(".CODE");
				codeSegmentDeclared=true;
			}
			writeIntoAssemblyFile($ID->getText() + " PROC");
			writeIntoAssemblyFile("\tPUSH BP\n"
									"\tMOV BP, SP");
		}
	}
	LPAREN parameter_list RPAREN {
		insideFunction=true;
		functionParameterStackBasePointerOffset=0;
	}compound_statement
	{
		insideFunction=false;
		parameterOffset=2;

		int numberOfBytesToRemove=functionParamsMap.size() * 2;
		

		if(!inMain){
			if(!returnLabelAssigned) {
				returnLabel= ++codeSegmentLineCount;
				returnLabelAssigned=true;
			}
			writeIntoAssemblyFile("L" + to_string(returnLabel) + ":");
			////////////////////////////////////////////////////////////
			if(localVariableDecalredInFunction>0){
				writeIntoAssemblyFile("\tADD SP, " + to_string(localVariableDecalredInFunction*2));
			}
			////////////////////////////////////////////////////////////

			writeIntoAssemblyFile("\tPOP BP");
			if(numberOfBytesToRemove>0)	writeIntoAssemblyFile("\tRET " + to_string(numberOfBytesToRemove));
			else writeIntoAssemblyFile("\tRET");

			writeIntoAssemblyFile($ID->getText() + " ENDP");
		}
		functionParamsMap.clear();
		localVariableDecalredInFunction=0;
		returningFromOtherFunctions=false;
		functionParameterStackBasePointerOffset=0;
		returnLabelAssigned=false;
	}
	| type_specifier ID {
		bool isMain = ($ID->getText() == "main");
		inMain=isMain;

		if(isMain){
			if(!codeSegmentDeclared) {
				writeIntoAssemblyFile(".CODE");
				codeSegmentDeclared=true;
			}
			writeIntoAssemblyFile("main PROC");
			writeIntoAssemblyFile("\tMOV AX, @DATA\n"
                     			"\tMOV DS, AX\n"
                     			"\tPUSH BP\n"
                    			 "\tMOV BP, SP");
		}
		else{
			if(!codeSegmentDeclared) {
				writeIntoAssemblyFile(".CODE");
				codeSegmentDeclared=true;
			}
			writeIntoAssemblyFile($ID->getText() + " PROC");
			writeIntoAssemblyFile("\tPUSH BP\n"
									"\tMOV BP, SP");
		}
	} LPAREN RPAREN compound_statement {
		if(isMain){
			writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
			writeIntoAssemblyFile("\tADD SP, " + to_string(localVariableDeclaredInMain*2) + "\n"
									"\tPOP BP\n"
									"\tMOV AX,4CH\n"
									"\tINT 21H");
			writeIntoAssemblyFile("main ENDP");
			writeIntoAssemblyFile(";-------------------------------\n"
									";         print library         \n"
									";-------------------------------\n"
									";-------------------------------");
			
			const string print_library = readFileToString("lib/print.asm");
			writeIntoAssemblyFile(print_library);

			writeIntoAssemblyFile("END main");
			inMain=false;
		}
		else{
			if(!returnLabelAssigned) {
				returnLabel= ++codeSegmentLineCount;
				returnLabelAssigned=true;
			}
			writeIntoAssemblyFile("L" + to_string(returnLabel) + ":");
			////////////////////////////////////////////////////////////
			if(localVariableDecalredInFunction>0){
				writeIntoAssemblyFile("\tADD SP, " + to_string(localVariableDecalredInFunction*2));
			}
			////////////////////////////////////////////////////////////

			writeIntoAssemblyFile("\tPOP BP\n"
									"\tRET");
			writeIntoAssemblyFile($ID->getText() + " ENDP");
			localVariableDecalredInFunction=0;
			returnLabelAssigned=false;
		}
	};

parameter_list:
	parameter_list COMMA type_specifier ID
	{
		parameterOffset+=2;
		functionParamsMap[$ID->getText()]=parameterOffset;
	}
	| parameter_list COMMA type_specifier
	|
	type_specifier ID
	{
		parameterOffset+=2;
		functionParamsMap[$ID->getText()]=parameterOffset;
	}
	| type_specifier;

compound_statement:
	LCURL {symbolTable.enterScope();} statements RCURL {symbolTable.exitScope();}
	| LCURL {symbolTable.enterScope();} RCURL {symbolTable.exitScope();};

var_declaration:
	t = type_specifier {currentTypeInDeclarationList=$t.type;} dl = declaration_list sm = SEMICOLON
		{
        // writeIntoparserLogFile(
        //     string("Variable Declaration: type_specifier declaration_list ") +
        //     to_string($sm->getType()) +
        //     " at line " + to_string($sm->getLine())
        // );

        // writeIntoparserLogFile("type_specifier name_line: " + $t.name_line);
      }
	| t = type_specifier de = declaration_list_err sm = SEMICOLON {
        // writeIntoErrorFile(
        //     string("Line# ") + to_string($sm->getLine()) +
        //     " with error name: " + $de.error_name +
        //     " - Syntax error at declaration list of variable declaration"
        // );

        // syntaxErrorCount++;
      };

declaration_list_err
	returns[string error_name]:
	{
        $error_name = "Error in declaration list";
    };

type_specifier
	returns[string name_line, string type]:
	INT {
            $name_line = "type: INT at line" + to_string($INT->getLine());
			$type="int";
        }
	| FLOAT {
            $name_line = "type: FLOAT at line" + to_string($FLOAT->getLine());
			$type="float";
        }
	| VOID {
            $name_line = "type: VOID at line" + to_string($VOID->getLine());
			$type="void";
        };

declaration_list:
	declaration_list COMMA ID {
		bool inGlobalScope = (symbolTable.getCurrentScope() == 1);

		if(inGlobalScope){
			writeIntoAssemblyFile("\t" + $ID->getText() + " DW 1 DUP (0000H)");

			symbolTable.insert($ID->getText(), "ID", currentTypeInDeclarationList, "NULL", -1, {}, true);
		}
		else{
			writeIntoAssemblyFile("\tSUB SP, 2");

			int offset;
			if(insideFunction) offset = functionParameterStackBasePointerOffset += 2;
			else offset = stackBasePointerOffset += 2;

			symbolTable.insert($ID->getText(), "ID", currentTypeInDeclarationList, "NULL", -1, {}, false, offset);


			if(insideFunction){
				localVariableDecalredInFunction++;
			}
			else{
				localVariableDeclaredInMain++;
			}
		}
	}
	| declaration_list COMMA ID LTHIRD CONST_INT RTHIRD
	| ID {
		bool inGlobalScope = (symbolTable.getCurrentScope() == 1);

		if(inGlobalScope){
			writeIntoAssemblyFile("\t" + $ID->getText() + " DW 1 DUP (0000H)");

			symbolTable.insert($ID->getText(), "ID", currentTypeInDeclarationList, "NULL", -1, {}, true);
		}
		else{
			writeIntoAssemblyFile("\tSUB SP, 2");

			int offset;
			if(insideFunction) offset = functionParameterStackBasePointerOffset += 2;
			else offset = stackBasePointerOffset += 2;

			symbolTable.insert($ID->getText(), "ID", currentTypeInDeclarationList, "NULL", -1, {}, false, offset);


			if(insideFunction){
				localVariableDecalredInFunction++;
			}
			else{
				localVariableDeclaredInMain++;
			}
		}
	}
	| ID LTHIRD CONST_INT RTHIRD {
		bool inGlobalScope = (symbolTable.getCurrentScope() == 1);

		if(inGlobalScope){
			writeIntoAssemblyFile("\t" + $ID->getText() + " DW " + $CONST_INT->getText() + " DUP (0000H)");

			currentTypeInDeclarationList = currentTypeInDeclarationList + " array";

			symbolTable.insert($ID->getText(), "ID", currentTypeInDeclarationList, "NULL", -1, {}, true);
		}

		else{
			int arrayOffset = 2 * stoi($CONST_INT->getText());

			writeIntoAssemblyFile("\tSUB SP, " + to_string(arrayOffset));

			stackBasePointerOffset = stackBasePointerOffset + arrayOffset;
			symbolTable.insert($ID->getText(), "ID", currentTypeInDeclarationList, "NULL", -1, {}, false, stackBasePointerOffset);

			localVariableDeclaredInMain++;
		}

	};

statements: statement| statements statement;

statement:
	var_declaration
	| expression_statement
	| compound_statement
	| FOR LPAREN expression_statement 
	{
		insideForLoopConditional=true;
	}
	expression_statement 
	{
		insideForLoopConditional=false;

		int forLoopBeginLabel=currentLabel;

		int forLoopFalseLabel=finalFalseLabel;
	}
	expression
	{
		writeIntoAssemblyFile("\tJMP L" + to_string(forLoopBeginLabel));

		int incrementlLabel=codeSegmentLineCount-1;
	}
	RPAREN 
	{
		insideForLoop=true;
		writeIntoAssemblyFile("L" + to_string(finalTruthLabel) + ":");

		////////////////////////////////////////
		emptyStack(truthLabels);
		emptyStack(falseLabels);

		truthLabel=finalTruthLabel=0;
		falseLabel=finalFalseLabel=0;
		////////////////////////////////////////
	}
	statement
	{

		writeIntoAssemblyFile("L" + to_string(nextLabel) + ":");
		writeIntoAssemblyFile("\tJMP L" + to_string(incrementlLabel));

		insideForLoop=false;

		currentLabel = forLoopFalseLabel;
		writeIntoAssemblyFile("L" + to_string(currentLabel) + ":");

		if(insideFunction || inMain) lastLabelInserted.push(currentLabel);
	}
	| IF 
	LPAREN
	{
		insideConditional=true;

		emptyStack(truthLabels);
		emptyStack(falseLabels);
	}
	expression 
	RPAREN
	{
		insideConditionalStatement=true;
		insideConditional=false;

		writeIntoAssemblyFile("L" + to_string(finalTruthLabel) + ":");
		currentLabel=finalTruthLabel;
	}
	statement
	{
		writeIntoAssemblyFile("L" + to_string(finalFalseLabel) + ":");

		insideConditionalStatement=false;
		nextLabel=finalFalseLabel; /////?

		truthLabel=finalTruthLabel=0;
		falseLabel=finalFalseLabel=0;

		emptyStack(truthLabels);
		emptyStack(falseLabels);

		nextLabel=0;
	}
	| 
	{
		ifElseBlockCompleted=false;
	}
	IF 
	LPAREN
	{
		insideConditional=true;

		emptyStack(truthLabels);
		emptyStack(falseLabels);
	}
	expression 
	RPAREN 
	{
		insideConditionalStatement=true;
		insideConditional=false;

		writeIntoAssemblyFile("L" + to_string(finalTruthLabel) + ":");
		currentLabel=finalTruthLabel;
	}
	statement
	{
		writeIntoAssemblyFile("L" + to_string(++codeSegmentLineCount) + ":");

		nextLabel=++codeSegmentLineCount;
		if(ifElseCompletionLabel.empty()){
			currentLabel=nextLabel;
			ifElseCompletionLabel.push(currentLabel);
		}
		writeIntoAssemblyFile("\tJMP L" + to_string(ifElseCompletionLabel.top()));
	}
	ELSE
	{
		writeIntoAssemblyFile("L" + to_string(finalFalseLabel) + ":");
	}
	statement
	{
		if(!ifElseCompletionLabel.empty()) currentLabel = nextLabel = ifElseCompletionLabel.top(); /////////remove ??

		insideConditionalStatement=false;

		truthLabel=finalTruthLabel=0;
		falseLabel=finalFalseLabel=0;

		emptyStack(truthLabels);
		emptyStack(falseLabels);
		emptyStack(ifElseCompletionLabel);
	}
	| 
	WHILE 
	LPAREN
	{
		insideWhileLoopConditional=true;
		int conditionalLabel=codeSegmentLineCount+1;
	}
	expression
	{
		int whileLoopFalseLabel;
		if(finalFalseLabel!=0) whileLoopFalseLabel=finalFalseLabel;
		else whileLoopFalseLabel = codeSegmentLineCount+2;
	}
	RPAREN 
	{
		insideWhileLoopConditional=false;

		insideWhileLoop=true;

		int whileLoopTruthLabel;
		if(finalTruthLabel!=0) whileLoopTruthLabel = finalTruthLabel;
		else { 
			whileLoopTruthLabel = codeSegmentLineCount+1;
			codeSegmentLineCount+=2;
		}

		writeIntoAssemblyFile("L" + to_string(whileLoopTruthLabel) + ":");
	}
	statement
	{
		codeSegmentLineCount++;
		writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
		writeIntoAssemblyFile("\tJMP L" + to_string(conditionalLabel));

		insideWhileLoop=false;

		emptyStack(truthLabels);
		emptyStack(falseLabels);

		truthLabel=finalTruthLabel=0;
		falseLabel=finalFalseLabel=0;

		currentLabel=whileLoopFalseLabel;
	}
	| 
	{
		if(!lastLabelInserted.empty() && lastLabelInserted.top() == currentLabel) emptyStack(lastLabelInserted);
		else if(!insideConditionalStatement && !insideForLoop) writeIntoAssemblyFile("L" + to_string(currentLabel) + ":");
	}
	PRINTLN LPAREN ID RPAREN SEMICOLON {
		bool isGlobalVar = symbolTable.lookUpAll($ID->getText())->getIsGlobal();
		int bpOffset = symbolTable.lookUpAll($ID->getText())->getStackBasePointerOffset();

		const string var = isGlobalVar ? $ID->getText() : "[BP-"+to_string(bpOffset)+"]";

		writeIntoAssemblyFile("\tMOV AX, " + var);
		writeIntoAssemblyFile("\tCALL print_output\n"
							"\tCALL new_line");
		
		if(currentLabel<codeSegmentLineCount) currentLabel=codeSegmentLineCount;
		if(!insideConditionalStatement) currentLabel++;
	}
	| RETURN {
		if(!lastLabelInserted.empty()) emptyStack(lastLabelInserted);
		else {
			if(!insideConditionalStatement) writeIntoAssemblyFile("L" + to_string(currentLabel) + ":");
			else {
				currentLabel=++codeSegmentLineCount;
				writeIntoAssemblyFile("L" + to_string(currentLabel) + ":");
			}
		}
		if(!inMain) returningFromOtherFunctions = true;
	} expression 
	{
		if(!inMain && returnValueInStack){
			writeIntoAssemblyFile("\tPOP AX");
		}
		returnValueInStack=false;
	}
	SEMICOLON {
		if($expression.exp != "") writeIntoAssemblyFile("\tMOV AX, " + $expression.exp);

		if(codeSegmentLineCount<currentLabel) codeSegmentLineCount = currentLabel+1;

		if(inMain) writeIntoAssemblyFile("\tJMP L" + to_string(codeSegmentLineCount));
		
		else {
			if(!returnLabelAssigned){
				returnLabel=++codeSegmentLineCount;
				returnLabelAssigned=true;
			}
			writeIntoAssemblyFile("\tJMP L" + to_string(returnLabel));
		}

		if(!inMain) currentLabel=++codeSegmentLineCount;
		nextLabel=0;
	};

expression_statement:
	SEMICOLON | expression SEMICOLON;

variable
	returns[string var]:
	ID {
		bool isGlobal=false;
		int bpOffset=0;
		if(symbolTable.lookUpAll($ID->getText())){
			isGlobal = symbolTable.lookUpAll($ID->getText())->getIsGlobal();
			bpOffset = symbolTable.lookUpAll($ID->getText())->getStackBasePointerOffset();
		}

		if(isGlobal) $var=$ID->getText() ;
		else $var="[BP-" + to_string(bpOffset) + "]";

		if(insideFunction && functionParamsMap.find($ID->getText()) != functionParamsMap.end()) $var="[BP+" + to_string(functionParamsMap[$ID->getText()]) + "]";
	}
	| ID LTHIRD expression RTHIRD
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		bool isGlobal = symbolTable.lookUpAll($ID->getText())->getIsGlobal();
		int bpOffset = symbolTable.lookUpAll($ID->getText())->getStackBasePointerOffset();
		
		if(isGlobal) {
			
			if(!arithmeticCalculationAssignment){
				codeSegmentLineCount++;
				writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
			}
			
			writeIntoAssemblyFile("\tMOV AX, " + $expression.exp);
			writeIntoAssemblyFile("\tPUSH AX");
			writeIntoAssemblyFile("\tPOP BX");
			writeIntoAssemblyFile("\tMOV AX, 2");
			writeIntoAssemblyFile("\tMUL BX");
			writeIntoAssemblyFile("\tMOV BX, AX");
			
			$var = $ID->getText() + "[BX]";
		}
		else {
			
			if(!arithmeticCalculationAssignment){
				codeSegmentLineCount++;
				writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
			}
			
			writeIntoAssemblyFile("\tMOV AX, " + $expression.exp);
			writeIntoAssemblyFile("\tPUSH AX");
			writeIntoAssemblyFile("\tPOP BX");
			writeIntoAssemblyFile("\tMOV AX, 2");
			writeIntoAssemblyFile("\tMUL BX");
			writeIntoAssemblyFile("\tMOV BX, AX");
			writeIntoAssemblyFile("\tMOV AX, " + to_string(bpOffset));
			writeIntoAssemblyFile("\tSUB AX, BX");
			writeIntoAssemblyFile("\tMOV BX, AX");
			writeIntoAssemblyFile("\tMOV SI, BX");
			writeIntoAssemblyFile("\tNEG SI");
			
			$var = "[BP+SI]";
		}
		
		arithmeticCalculationAssignment = true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	;

expression
	returns[string exp, string result_reg]:
	logic_expression {
		$exp=$logic_expression.lexp;
	}
	| 
	variable ASSIGNOP 
	logic_expression {
						if($logic_expression.usage=="relational assignment"){
							//first level
							writeIntoAssemblyFile("L" + to_string(finalTruthLabel) + ":");
							writeIntoAssemblyFile("\tMOV AX, 1\n"
													"\tJMP L" + to_string(finalFalseLabel + 1));

							//second level
							writeIntoAssemblyFile("L" + to_string(finalFalseLabel) + ":");
							writeIntoAssemblyFile("\tMOV AX, 0");


							// next instruction
							writeIntoAssemblyFile("L" + to_string(finalFalseLabel + 1) + ":");

							codeSegmentLineCount++;

							truthLabel=finalTruthLabel=0;
							falseLabel=finalFalseLabel=0;

							emptyStack(truthLabels);
							emptyStack(falseLabels);
						}
						else if($logic_expression.usage=="logical assignment"){
							//level 1
							writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount+2) + ":");
							writeIntoAssemblyFile("\tMOV AX, 1");
							
							writeIntoAssemblyFile("\tJMP L" + to_string(codeSegmentLineCount+3)); // this jump level offset is common to both '||' and '&&'

							//level 2
							writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount+4) + ":");
							writeIntoAssemblyFile("\tMOV AX, 0");

							codeSegmentLineCount+=2;
						}

						if(!arithmeticCalculationAssignment && !insideConditionalStatement && !($logic_expression.usage=="relational assignment")){
							if(codeSegmentLineCount<currentLabel) codeSegmentLineCount=currentLabel;
							else codeSegmentLineCount++;
							writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
						}


						if($logic_expression.usage=="relational assignment"){
							
						}
						else if($logic_expression.usage=="logical assignment"){
							
						}
						else if($logic_expression.lexp!="") {
							writeIntoAssemblyFile("\tMOV AX, " + $logic_expression.lexp);
						}
						else {
							// only popping for arithmetic calculations
							writeIntoAssemblyFile("\tPOP AX");
						}

						writeIntoAssemblyFile("\tMOV " + $variable.var + ", AX");

						arithmeticCalculationAssignment=false;

						if($logic_expression.usage=="relational assignment" || $logic_expression.usage=="logical assignment"){
							codeSegmentLineCount++;
						}

						currentLabel=++codeSegmentLineCount;
					$result_reg=$logic_expression.result_reg;

	};

logic_expression
	returns[string lexp, string usage, string result_reg]:
	rel_expression {
		$lexp=$rel_expression.rexp;
		$usage=$rel_expression.usage;
		$result_reg=$rel_expression.result_reg;
	}
	| 
	re1=rel_expression 
	LOGICOP 
	{
		if($LOGICOP->getText() == "||"){
			
			while(!falseLabels.empty() && falseLabels.top() == -1) falseLabels.pop();
			if(!falseLabels.empty()){
				nextLabel=falseLabels.top();
				falseLabels.pop();
			}
			else{
				nextLabel=falseLabel;
			}
			
		}
		else if($LOGICOP->getText() == "&&"){

			while(!truthLabels.empty() && truthLabels.top() == -1) truthLabels.pop();
			if(!truthLabels.empty()){
				while(!truthLabels.empty() && truthLabels.top() == -1) truthLabels.pop();
				nextLabel=truthLabels.top();
				truthLabels.pop();
			}
			else{
				nextLabel=truthLabel;
			}
		}
	}
	re2=rel_expression {

		$usage="logical assignment";


		if($re1.rexp!="" && $re2.rexp!=""){
			codeSegmentLineCount++;
			writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");

			//for operand 1
			writeIntoAssemblyFile("\tMOV AX, " + $re1.rexp + "\n"
									"\tCMP AX, 0");
			
			if($LOGICOP->getText() == "||"){
				writeIntoAssemblyFile("\tJNE L" + to_string(codeSegmentLineCount+2));
				writeIntoAssemblyFile("\tJMP L" + to_string(codeSegmentLineCount+1));
			}
			else if($LOGICOP->getText() == "&&"){
				writeIntoAssemblyFile("\tJNE L" + to_string(codeSegmentLineCount+1));
				writeIntoAssemblyFile("\tJMP L" + to_string(codeSegmentLineCount+4));
			}

			
			//for operand 2
			writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount+1) + ":");
			writeIntoAssemblyFile("\tMOV AX, " + $re2.rexp + "\n"
									"\tCMP AX, 0");

			if($LOGICOP->getText() == "||"){
				writeIntoAssemblyFile("\tJNE L" + to_string(codeSegmentLineCount+2));
				writeIntoAssemblyFile("\tJMP L" + to_string(codeSegmentLineCount+4));
			}
			else if($LOGICOP->getText() == "&&"){
				writeIntoAssemblyFile("\tJNE L" + to_string(codeSegmentLineCount+2));
				writeIntoAssemblyFile("\tJMP L" + to_string(codeSegmentLineCount+4));
			}
		}
		else{
			if(!truthLabels.empty()) truthLabels.push(-1);
			else if(!falseLabels.empty()) falseLabels.push(-1);
		}
	};

rel_expression
	returns[string rexp, string usage, string result_reg]:
	simple_expression {
		$rexp=$simple_expression.sexp;
		$result_reg=$simple_expression.result_reg;
	}
	| {
		if(nextLabel==0){
			currentLabel = ++codeSegmentLineCount;
		}
		else {
			currentLabel = nextLabel; 
		}
		if(!insideConditionalStatement) writeIntoAssemblyFile("L" + to_string(currentLabel) + ":");
	} se1=simple_expression RELOP se2=simple_expression {
		
		if(truthLabels.empty()){
			truthLabel=++codeSegmentLineCount;
			truthLabels.push(truthLabel);
		}
		else if(truthLabels.top() == -1){
			truthLabels.pop();
			truthLabel=++codeSegmentLineCount;
			truthLabels.push(truthLabel);
		}
		else{
			truthLabel = truthLabels.top();
			truthLabels.pop();
		}

		finalTruthLabel = truthLabel;

		if(falseLabels.empty()){
			falseLabel=++codeSegmentLineCount;
			falseLabels.push(falseLabel);
		}
		else if(falseLabels.top() == -1){
			falseLabels.pop();
			falseLabel=++codeSegmentLineCount;
			falseLabels.push(falseLabel);
		}
		else{
			falseLabel = falseLabels.top();
			falseLabels.pop();
		}
		finalFalseLabel = falseLabel;
		

		if($se2.sexp!="") writeIntoAssemblyFile("\tMOV AX, " + $se2.sexp);
		else writeIntoAssemblyFile("\tPOP AX");

		writeIntoAssemblyFile("\tMOV DX, AX");

		if($se1.sexp!="") writeIntoAssemblyFile("\tMOV AX, " + $se1.sexp);
		else writeIntoAssemblyFile("\tPOP AX");

		writeIntoAssemblyFile("\tCMP AX, DX");

		// writeIntoAssemblyFile("\tMOV AX, " + $se2.sexp + "\n"
		// 						"\tMOV DX, AX\n"
		// 						"\tMOV AX, " + $se1.sexp + "\n"
		// 						"\tCMP AX, DX");
								
		if($RELOP->getText() == "<=") {
			int label = truthLabel;
			writeIntoAssemblyFile("\tJLE L" + to_string(label));
		}
		else if($RELOP->getText() == "<") {
			int label = truthLabel;
			writeIntoAssemblyFile("\tJL L" + to_string(label));
		}
		else if($RELOP->getText() == ">") {
			int label = truthLabel;
			writeIntoAssemblyFile("\tJG L" + to_string(truthLabel));
		}
		else if($RELOP->getText() == "!=") {
			int label = truthLabel;
			writeIntoAssemblyFile("\tJNE L" + to_string(label));
		}
		else if($RELOP->getText() == "==") {
			int label = truthLabel;
			writeIntoAssemblyFile("\tJE L" + to_string(label));
		}

		int jmpLabel = falseLabel;
		writeIntoAssemblyFile("\tJMP L" + to_string(jmpLabel));

		if(insideForLoop) nextLabel=falseLabel;

		$usage="relational assignment";
		$rexp="";
	};

simple_expression
    returns[string sexp, string result_reg]:
    term {
        $sexp=$term.trm;
		$result_reg=$term.result_reg;
    }
    | se=simple_expression ADDOP t=term {
		if(!arithmeticCalculationAssignment && !insideWhileLoop){
			codeSegmentLineCount++;
			writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
		}
		
		// handling the right operand
		if($t.trm!="") {
			writeIntoAssemblyFile("\tMOV AX, " + $t.trm);
		} else {
			// Result from previous calculation, get from stack
			writeIntoAssemblyFile("\tPOP AX");
		}
		writeIntoAssemblyFile("\tMOV DX, AX");
		
		// handling the left operand
		if($se.sexp!="") {
			writeIntoAssemblyFile("\tMOV AX, " + $se.sexp);
		} else {
			// get Result from previous calculation
			writeIntoAssemblyFile("\tPOP AX");
		}

		if($ADDOP->getText() == "+") {
			writeIntoAssemblyFile("\tADD AX, DX");
		} else if($ADDOP->getText() == "-") {
			writeIntoAssemblyFile("\tSUB AX, DX");
		}
		
		writeIntoAssemblyFile("\tPUSH AX");

		if(!inMain && returningFromOtherFunctions) returnValueInStack = true;

		$sexp="";
		$result_reg="AX";
		arithmeticCalculationAssignment=true;
};


term
	returns[string trm, string result_reg]:
	unary_expression {
		$trm=$unary_expression.uexp;
		$result_reg=$unary_expression.result_reg;
	}
	| t=term MULOP unary_expression {
		if(!arithmeticCalculationAssignment){
			codeSegmentLineCount++;
			writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
		}

		// handling the right operand 
		if($unary_expression.uexp!="") {
			writeIntoAssemblyFile("\tMOV AX, " + $unary_expression.uexp);
		} else if($unary_expression.uexp!="AX"){
			writeIntoAssemblyFile("\tPOP AX");
		}
		writeIntoAssemblyFile("\tMOV CX, AX");
		
		// handling the left operand 
		if($t.trm!="") {
			writeIntoAssemblyFile("\tMOV AX, " + $t.trm);
		} else {
			// If $t.trm is empty, it means the result was from a previous calculation
			// We need to get it from where it was stored
			writeIntoAssemblyFile("\tPOP AX");  // Get the preserved result
		}
		
		writeIntoAssemblyFile("\tCWD");
		
		if($MULOP->getText() == "*") {
			writeIntoAssemblyFile("\tMUL CX");
		} else if($MULOP->getText() == "%") {
			writeIntoAssemblyFile("\tDIV CX");
			writeIntoAssemblyFile("\tMOV AX, DX");  // Remainder is in DX
		}
		
		writeIntoAssemblyFile("\tPUSH AX");
		
		$trm="";
		$result_reg="AX";
		arithmeticCalculationAssignment=true;
};

unary_expression
	returns[string uexp, string result_reg]:
	ADDOP unary_expression {
		$uexp="";

		if(!arithmeticCalculationAssignment){
			codeSegmentLineCount++;
			writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
		}

		if($ADDOP->getText() == "-"){
			writeIntoAssemblyFile("\tMOV AX, " + $unary_expression.uexp + "\n"
									"\tNEG AX\n"
									"\tPUSH AX");
		}
		arithmeticCalculationAssignment=true;

		$result_reg="AX";
	}
	| NOT unary_expression {
		$uexp="";
	}
	| factor {
		$uexp=$factor.fct;
	};

factor
	returns[string fct]:
	variable {
		$fct=$variable.var;
	}
	// | ID LPAREN argument_list RPAREN
	| ID LPAREN argument_list RPAREN {
		if(!arithmeticCalculationAssignment){
			codeSegmentLineCount++;
			writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
		}
		
		writeIntoAssemblyFile("\tCALL " + $ID->getText());
		writeIntoAssemblyFile("\tPUSH AX");
	}
	| LPAREN expression RPAREN
	| CONST_INT {
		$fct=$CONST_INT->getText();
	}
	| CONST_FLOAT {
		$fct=$CONST_FLOAT->getText();
	}
	| variable INCOP {
		$fct="";
		
		++codeSegmentLineCount;

		writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
		writeIntoAssemblyFile("\tMOV AX, " + $variable.var + "\n"
								"\tPUSH AX\n"
								"\tINC AX\n"
								"\tMOV " + $variable.var + ", AX\n"
								"\tPOP AX");
		currentLabel=++codeSegmentLineCount;
		
	}
	| variable DECOP {
		$fct="";

		codeSegmentLineCount++;

		writeIntoAssemblyFile("L" + to_string(codeSegmentLineCount) + ":");
		writeIntoAssemblyFile("\tMOV AX, " + $variable.var + "\n"
								"\tPUSH AX\n"
								"\tDEC AX\n"
								"\tMOV " + $variable.var + ", AX\n"
								"\tPOP AX");
		
		if(insideWhileLoopConditional){
			writeIntoAssemblyFile("\tCMP AX, 0\n"
			"\tJNE L" + to_string(codeSegmentLineCount+1) + "\n"
			"\tJMP L" + to_string(codeSegmentLineCount+2));
		}
	};

argument_list: arguments |;

// arguments: arguments COMMA logic_expression | logic_expression;

arguments: 
    logic_expression COMMA arguments {
        if($logic_expression.lexp != "") {
            writeIntoAssemblyFile("\tMOV AX, " + $logic_expression.lexp);
			writeIntoAssemblyFile("\tPUSH AX");
        }
        // writeIntoAssemblyFile("\tPUSH AX");
    }
    | logic_expression {
        if($logic_expression.lexp != "") {
            writeIntoAssemblyFile("\tMOV AX, " + $logic_expression.lexp);
			writeIntoAssemblyFile("\tPUSH AX");
        }
    };