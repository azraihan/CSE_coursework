%option noyywrap

%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <fstream>
#include"SymbolTable/2105009_SymbolTable.h"
#include"SymbolTable/2105009_hash.h"

using namespace std;

int line_count=1;
int error_count=0;

int string_start_line = 0;
int comment_start_line = 0;

ofstream logout_stream;
ofstream tokenout_stream;

char string_buffer[1000];
int string_buffer_index = 0;

char comment_buffer[2000];
int comment_buffer_index = 0;

SymbolTable* symbolTable = nullptr;
const int NUM_BUCKETS = 7;



// Helper function to handle character literal escape sequences
char getEscapedChar(char c) {
    switch(c) {
        case 'n': return '\n';
        case 't': return '\t';
        case '\\': return '\\';
        case '\'': return '\'';
        case '\"': return '\"';
        case 'a': return '\a';
        case 'f': return '\f';
        case 'r': return '\r';
        case 'b': return '\b';
        case 'v': return '\v';
        case '0': return '\0';
        default: return c;
    }
}

/////////
char original_string_buffer[2000];
int original_string_buffer_index = 0;
/////////////////

// Helper function to reset string buffer
void reset_string_buffer() {
    string_buffer_index = 0;
    string_buffer[0] = '\0';
}

// Helper function to add char to string buffer
void add_to_string_buffer(char c) {
    string_buffer[string_buffer_index++] = c;
    string_buffer[string_buffer_index] = '\0';
}

/////////////////
void reset_original_string_buffer() {
    original_string_buffer_index = 0;
    original_string_buffer[0] = '\0';
}

void add_to_original_string_buffer(const char* str) {
    strcat(original_string_buffer, str);
    original_string_buffer_index += strlen(str);
}
/////////////////

// Helper functions for comment buffer
void reset_comment_buffer() {
    comment_buffer_index = 0;
    comment_buffer[0] = '\0';
}

void add_to_comment_buffer(char c) {
    comment_buffer[comment_buffer_index++] = c;
    comment_buffer[comment_buffer_index] = '\0';
}

void add_string_to_comment_buffer(const char* str) {
    strcat(comment_buffer, str);
    comment_buffer_index += strlen(str);
}

%}

/* regex definitions */
WHITESPACE [ \t\f\r\v]+ 
LETTER [a-zA-Z]
DIGIT [0-9]
NEWLINE \n
ID ({LETTER}|_)({LETTER}|{DIGIT}|_)*
INTEGER {DIGIT}+
FLOAT {DIGIT}*\.{DIGIT}+([Ee][+-]?{DIGIT}+)?|{DIGIT}+\.([Ee][+-]?{DIGIT}+)?|{DIGIT}+[Ee][+-]?{DIGIT}+
ILLFORMED_NUMBER {DIGIT}*\.{DIGIT}+\.{DIGIT}+|{DIGIT}+[Ee][+-]?{DIGIT}+\.{DIGIT}+
INVALID_IP {DIGIT}+\.{DIGIT}+\.{DIGIT}+\.{DIGIT}+
INVALID_SUFFIX_PREFIX {DIGIT}+{ID}
CHAR_LITERAL '([^'\\\n]|\\.)'
MULTI_CHAR_ERROR '([^'\\\n]|\\.)([^'\\\n]|\\.)+\'
EMPTY_CHAR \'\'
UNFINISHED_CHAR \'([^'\n]|\\.)?

/* For String and Comment handling */
%x STRING
%x SINGLE_LINE_COMMENT
%x MULTI_LINE_COMMENT

%%

{NEWLINE} {line_count++;}

{WHITESPACE} {}

"if"        {
            tokenout_stream << "<IF> ";
            logout_stream << "Line no " << line_count << ": Token <IF> Lexeme " << yytext << " found\n\n";
           }

"else"      {
            tokenout_stream << "<ELSE> ";
            logout_stream << "Line no " << line_count << ": Token <ELSE> Lexeme " << yytext << " found\n\n";
           }

"for"       {
            tokenout_stream << "<FOR> ";
            logout_stream << "Line no " << line_count << ": Token <FOR> Lexeme " << yytext << " found\n\n";
           }

"while"     {
            tokenout_stream << "<WHILE> ";
            logout_stream << "Line no " << line_count << ": Token <WHILE> Lexeme " << yytext << " found\n\n";
           }

"do"        {
            tokenout_stream << "<DO> ";
            logout_stream << "Line no " << line_count << ": Token <DO> Lexeme " << yytext << " found\n\n";
           }

"break"     {
            tokenout_stream << "<BREAK> ";
            logout_stream << "Line no " << line_count << ": Token <BREAK> Lexeme " << yytext << " found\n\n";
           }

"int"       {
            tokenout_stream << "<INT> ";
            logout_stream << "Line no " << line_count << ": Token <INT> Lexeme " << yytext << " found\n\n";
           }

"char"      {
            tokenout_stream << "<CHAR> ";
            logout_stream << "Line no " << line_count << ": Token <CHAR> Lexeme " << yytext << " found\n\n";
           }

"float"     {
            tokenout_stream << "<FLOAT> ";
            logout_stream << "Line no " << line_count << ": Token <FLOAT> Lexeme " << yytext << " found\n\n";
           }

"double"    {
            tokenout_stream << "<DOUBLE> ";
            logout_stream << "Line no " << line_count << ": Token <DOUBLE> Lexeme " << yytext << " found\n\n";
           }

"void"      {
            tokenout_stream << "<VOID> ";
            logout_stream << "Line no " << line_count << ": Token <VOID> Lexeme " << yytext << " found\n\n";
           }

"return"    {
            tokenout_stream << "<RETURN> ";
            logout_stream << "Line no " << line_count << ": Token <RETURN> Lexeme " << yytext << " found\n\n";
           }

"switch"    {
            tokenout_stream << "<SWITCH> ";
            logout_stream << "Line no " << line_count << ": Token <SWITCH> Lexeme " << yytext << " found\n\n";
           }

"case"      {
            tokenout_stream << "<CASE> ";
            logout_stream << "Line no " << line_count << ": Token <CASE> Lexeme " << yytext << " found\n\n";
           }

"default"   {
            tokenout_stream << "<DEFAULT> ";
            logout_stream << "Line no " << line_count << ": Token <DEFAULT> Lexeme " << yytext << " found\n\n";
           }

"continue"  {
            tokenout_stream << "<CONTINUE> ";
            logout_stream << "Line no " << line_count << ": Token <CONTINUE> Lexeme " << yytext << " found\n\n";
           }

"goto"      {
            tokenout_stream << "<GOTO> ";
            logout_stream << "Line no " << line_count << ": Token <GOTO> Lexeme " << yytext << " found\n\n";
           }

"long"      {
            tokenout_stream << "<LONG> ";
            logout_stream << "Line no " << line_count << ": Token <LONG> Lexeme " << yytext << " found\n\n";
           }

"short"     {
            tokenout_stream << "<SHORT> ";
            logout_stream << "Line no " << line_count << ": Token <SHORT> Lexeme " << yytext << " found\n\n";
           }

"static"    {
            tokenout_stream << "<STATIC> ";
            logout_stream << "Line no " << line_count << ": Token <STATIC> Lexeme " << yytext << " found\n\n";
           }

"unsigned"  {
            tokenout_stream << "<UNSIGNED> ";
            logout_stream << "Line no " << line_count << ": Token <UNSIGNED> Lexeme " << yytext << " found\n\n";
           }

{INTEGER}   {
            tokenout_stream << "<CONST_INT, " << yytext << "> ";
            logout_stream << "Line no " << line_count << ": Token <CONST_INT> Lexeme " << yytext << " found\n\n";
            //insert in symbol table and print symbol table content(only non empty buckets)
            if(symbolTable->lookUp(yytext) == nullptr){
                symbolTable->insert(yytext, "CONST_INT");
                symbolTable->printAllScopeTables();
            }
            logout_stream << "\n";
           }

{FLOAT}     {
            tokenout_stream << "<CONST_FLOAT, " << yytext << "> ";
            logout_stream << "Line no " << line_count << ": Token <CONST_FLOAT> Lexeme " << yytext << " found\n\n";
            //insert in symbol table and print symbol table content(only non empty buckets)

            if(symbolTable->lookUp(yytext) == nullptr){
                symbolTable->insert(yytext, "CONST_FLOAT");
                symbolTable->printAllScopeTables();
            }
            logout_stream << "\n";
           }

{INVALID_IP} {
            logout_stream << "Error at line no " << line_count << ": Too many decimal points " << yytext << "\n\n";
            error_count++;
           }

{ILLFORMED_NUMBER} {
            logout_stream << "Error at line no " << line_count << ": Ill formed number " << yytext << "\n\n";
            error_count++;
           }

{INVALID_SUFFIX_PREFIX} {
            logout_stream << "Error at line no " << line_count << ": Invalid prefix on ID or invalid suffix on Number " << yytext << "\n\n";
            error_count++;
           }

{CHAR_LITERAL} {
            char ch;
            if(yytext[1] == '\\') {
                ch = getEscapedChar(yytext[2]);
            } else {
                ch = yytext[1];
            }
            tokenout_stream << "<CONST_CHAR, " << ch << "> ";
            logout_stream << "Line no " << line_count << ": Token <CONST_CHAR> Lexeme " << yytext << " found --> <CONST_CHAR, " << ch << ">\n\n";
            //insert in symbol table and print symbol table content(only non empty buckets)

            if(symbolTable->lookUp(yytext) == nullptr){
                symbolTable->insert(yytext, "CONST_CHAR");
                symbolTable->printAllScopeTables();
            }
            logout_stream << "\n";
           }

{MULTI_CHAR_ERROR} {
            logout_stream << "Error at line no " << line_count << ": Multi character constant error " << yytext << "\n\n";
            error_count++;
           }

{EMPTY_CHAR} {
    logout_stream << "Error at line no " << line_count << ": Empty character constant error " << yytext << "\n\n";
    error_count++;
}

{UNFINISHED_CHAR} {
    logout_stream << "Error at line no " << line_count << ": Unterminated character " << yytext << "\n\n";
    error_count++;
}


{ID}        {
            tokenout_stream << "<ID, " << yytext << "> ";
            logout_stream << "Line no " << line_count << ": Token <ID> Lexeme " << yytext << " found\n\n";
            //insert in symbol table and print symbol table content(only non empty buckets)

            if(symbolTable->lookUp(yytext) == nullptr){
                symbolTable->insert(yytext, "ID");
                symbolTable->printAllScopeTables();
            }
            logout_stream << "\n";
           }

"+"         |
"-"         {
            tokenout_stream << "<ADDOP, " << yytext << "> ";
            logout_stream << "Line no " << line_count << ": Token <ADDOP> Lexeme " << yytext << " found\n\n";
           }

"*"         |
"/"         |
"%"         {
            tokenout_stream << "<MULOP, " << yytext << "> ";
            logout_stream << "Line no " << line_count << ": Token <MULOP> Lexeme " << yytext << " found\n\n";
           }

"++"        {
            tokenout_stream << "<INCOP, ++> ";
            logout_stream << "Line no " << line_count << ": Token <INCOP> Lexeme " << yytext << " found\n\n";
           }

"--"        {
            tokenout_stream << "<INCOP, --> ";
            logout_stream << "Line no " << line_count << ": Token <INCOP> Lexeme " << yytext << " found\n\n";
           }

"<"         |
"<="        |
">"         |
">="        |
"=="        |
"!="        {
            tokenout_stream << "<RELOP, " << yytext << "> ";
            logout_stream << "Line no " << line_count << ": Token <RELOP> Lexeme " << yytext << " found\n\n";
           }

"="         {
            tokenout_stream << "<ASSIGNOP, => ";
            logout_stream << "Line no " << line_count << ": Token <ASSIGNOP> Lexeme " << yytext << " found\n\n";
           }

"&&"        |
"||"        {
            tokenout_stream << "<LOGICOP, " << yytext << "> ";
            logout_stream << "Line no " << line_count << ": Token <LOGICOP> Lexeme " << yytext << " found\n\n";
           }

"!"         {
            tokenout_stream << "<NOT, !> ";
            logout_stream << "Line no " << line_count << ": Token <NOT> Lexeme " << yytext << " found\n\n";
           }

"("         {
            tokenout_stream << "<LPAREN, (> ";
            logout_stream << "Line no " << line_count << ": Token <LPAREN> Lexeme " << yytext << " found\n\n";
           }

")"         {
            tokenout_stream << "<RPAREN, )> ";
            logout_stream << "Line no " << line_count << ": Token <RPAREN> Lexeme " << yytext << " found\n\n";
           }

"{"         {
            //Insert code to enter a new scope
            symbolTable->enterScope();

            tokenout_stream << "<LCURL, {> ";
            logout_stream << "Line no " << line_count << ": Token <LCURL> Lexeme " << yytext << " found\n\n";
           }

"}"         {
            //Insert code to exit from current scope
            symbolTable->exitScope();

            tokenout_stream << "<RCURL, }> ";
            logout_stream << "Line no " << line_count << ": Token <RCURL> Lexeme " << yytext << " found\n\n";
            
           }

"["         {
            tokenout_stream << "<LTHIRD, [> ";
            logout_stream << "Line no " << line_count << ": Token <LTHIRD> Lexeme " << yytext << " found\n\n";
           }

"]"         {
            tokenout_stream << "<RTHIRD, ]> ";
            logout_stream << "Line no " << line_count << ": Token <RTHIRD> Lexeme " << yytext << " found\n\n";
           }

","         {
            tokenout_stream << "<COMMA, ,> ";
            logout_stream << "Line no " << line_count << ": Token <COMMA> Lexeme " << yytext << " found\n\n";
           }

";"         {
            tokenout_stream << "<SEMICOLON, ;> ";
            logout_stream << "Line no " << line_count << ": Token <SEMICOLON> Lexeme " << yytext << " found\n\n";
           }

\"          {
    BEGIN STRING;
    reset_string_buffer();
    reset_original_string_buffer();
    string_start_line = line_count;  // Save the line where the string starts
}

<STRING>[^\"\n\\]+  {
    for(int i=0; i<yyleng; i++) {
        add_to_string_buffer(yytext[i]);
    }
    add_to_original_string_buffer(yytext); 
}

<STRING>\\n  {
    add_to_string_buffer('\n');
    add_to_original_string_buffer("\\n"); 
}

<STRING>\\t  {
    add_to_string_buffer('\t');
    add_to_original_string_buffer("\\t"); 
}

<STRING>\\\\  {
    add_to_string_buffer('\\');
    add_to_original_string_buffer("\\\\"); 
}

<STRING>\\\"  {
    add_to_string_buffer('\"');
    add_to_original_string_buffer("\\\""); 
}

<STRING>\\[^\n]  {
    add_to_string_buffer(getEscapedChar(yytext[1]));
    add_to_original_string_buffer(yytext); 
}

<STRING>\\\n  {
    line_count++;
    // Line continuation in string - nothing is added to the buffer
    add_to_original_string_buffer("\\\n"); 
}

<STRING>\n  {
    // Reporting error using the starting line number
    logout_stream << "Error at line no " << string_start_line << ": Unterminated string \"" << original_string_buffer << "\n\n";
    error_count++;
    line_count++;
    BEGIN INITIAL;
}

<STRING>\"  {
    tokenout_stream << "<STRING, " << string_buffer << "> ";
    // use both buffers
    logout_stream << "Line no " << line_count << ": Token <STRING> Lexeme \"" << original_string_buffer 
                  << "\" found --> <STRING, " << string_buffer << ">\n\n";
    BEGIN INITIAL;
}

"//"        {
            BEGIN SINGLE_LINE_COMMENT;
            reset_comment_buffer();
            add_string_to_comment_buffer("//");
           }

<SINGLE_LINE_COMMENT>[^\n\\]+  {
            add_string_to_comment_buffer(yytext);
           }

<SINGLE_LINE_COMMENT>\\\n  {
            add_to_comment_buffer('\\');
            add_to_comment_buffer('\n');
            line_count++;
           }

<SINGLE_LINE_COMMENT>\n  {
            logout_stream << "Line no " << line_count << ": Token <COMMENT> Lexeme " << comment_buffer << " found\n\n";
            line_count++;
            BEGIN INITIAL;
           }

"/*"        {
            BEGIN MULTI_LINE_COMMENT;
            reset_comment_buffer();
            add_string_to_comment_buffer("/*");
            comment_start_line = line_count;  // Save the line where the comment starts
        }

<MULTI_LINE_COMMENT>[^*\n]+  {
            add_string_to_comment_buffer(yytext);
           }

<MULTI_LINE_COMMENT>"*"[^/\n]*  {
            add_string_to_comment_buffer(yytext);
           }

<MULTI_LINE_COMMENT>\n  {
            add_to_comment_buffer('\n');
            line_count++;
           }

<MULTI_LINE_COMMENT>"*/"  {
            add_string_to_comment_buffer("*/");
            logout_stream << "Line no " << line_count << ": Token <COMMENT> Lexeme " << comment_buffer << " found\n\n";
            BEGIN INITIAL;
           }

<MULTI_LINE_COMMENT><<EOF>>  {
            logout_stream << "Error at line no " << comment_start_line << ": Unterminated comment " << comment_buffer << "\n\n";
            error_count++;
            BEGIN INITIAL;
        }
<<EOF>> {
    symbolTable->printAllScopeTables();
    return 0;
}

.           {
            logout_stream << "Error at line no " << line_count << ": Unrecognized character " << yytext << "\n\n";
            error_count++;
           }

%%

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Please provide input file name and try again\n");
        return 0;
    }

    const int ID = 2105009;
    
    // Extract the base name from the input file
    char base_name[256];
    char *input_file = argv[1];
    
    // Find the last slash or backslash to handle directory paths
    char *last_slash = strrchr(input_file, '/');
    char *last_backslash = strrchr(input_file, '\\');
    char *filename_start = input_file;
    
    if (last_slash != NULL && (last_backslash == NULL || last_slash > last_backslash)) {
        filename_start = last_slash + 1;
    } else if (last_backslash != NULL) {
        filename_start = last_backslash + 1;
    }
    
    // Find the last dot to handle file extension
    char *dot_position = strrchr(filename_start, '.');
    
    if (dot_position != NULL) {
        // Copy characters from filename start up to the dot
        size_t length = dot_position - filename_start;
        strncpy(base_name, filename_start, length);
        base_name[length] = '\0'; // Add null terminator
    } else {
        // No extension found, use the entire filename
        strcpy(base_name, filename_start);
    }
    
    // Create output filenames by concatenating base name with extensions
    char log_filename[300];
    char token_filename[300];
    sprintf(log_filename, "%d_%s_log.txt", ID, base_name);
    sprintf(token_filename, "%d_%s_token.txt", ID, base_name);
    
    FILE *fin = fopen(argv[1], "r");
    if(fin == NULL) {
        printf("Cannot open specified file\n");
        return 0;
    }

    // Open file streams
    logout_stream.open(log_filename);
    tokenout_stream.open(token_filename);

    /* const int NUM_BUCKETS = 7; */
    symbolTable = new SymbolTable(NUM_BUCKETS, logout_stream, SDBMHash);
        
    yyin = fin;
    yylex();
    
    // Print line count and error count at the end of log file
    logout_stream << "\nTotal lines: " << line_count << "\nTotal errors: " << error_count << endl;
    
    fclose(yyin);
    
    // Close file streams
    tokenout_stream.close();
    logout_stream.close();

    delete symbolTable;
    
    return 0;
}