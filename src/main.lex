%option nounput
%{
#include "common.h"
#include "main.tab.h"  // yacc header
int lineno=1;
%}
BLOCKCOMMENT \/\*([^\*^\/]*|[\*^\/*]*|[^\**\/]*)*\*\/
LINECOMMENT \/\/[^\n]*
EOL	(\r\n|\r|\n)
WHILTESPACE [[:blank:]]

INTEGER [0-9]+

CHAR \'.?\'
STRING \".+\"


IDENTIFIER [[:alpha:]_][[:alpha:][:digit:]_]*
%%

{BLOCKCOMMENT}  /* do nothing */
{LINECOMMENT}  /* do nothing */


"+" return  add
"-" return sub
"*" return mul
"/" return div
"%" return mod

"==" return eql
"!=" return noteql
">=" return bigeql
"<=" return smalleql
">" return big
"<" return small

"||" return OR
"&&" return AND
"!" return NOT


"int" return T_INT;
"char" return T_CHAR;

"=" return LOP_ASSIGN;

";" return  SEMICOLON;

"(" return left_br_small
")" return left_br_small
"[" return left_br_mid
"]" return right_br_mid
"{" return left_br_big
"}" return right_br_big
"return" return Return
"if" return If
"while" return While
"for" return For
"main" return Main




{INTEGER} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_INT;
    node->int_val = atoi(yytext);
    yylval = node;
    return INTEGER;
}

{CHAR} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_CHAR;
    node->int_val = yytext[1];
    yylval = node;
    return CHAR;
}

{IDENTIFIER} {
    TreeNode* node = new TreeNode(lineno, NODE_VAR);
    node->var_name = string(yytext);
    yylval = node;
    return IDENTIFIER;
}

{STRING}{
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->str_val = string(yytext);
    yylval = node;
    return STRING;
}



{WHILTESPACE} /* do nothing */

{EOL} lineno++;

{}


. {
    cerr << "[line "<< lineno <<" ] unknown character:" << yytext << endl;
}
%%