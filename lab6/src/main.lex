%option nounput
%{
#include "common.h"
#include "main.tab.h"  // yacc header
//int lineno=1;
//int beginDef=0;
//VarNode *root_var = new VarNode;
//VarNode *now = root_var;
%}
BLOCKCOMMENT \/\*([^\*^\/]*|[\*^\/*]*|[^\**\/]*)*\*\/
LINECOMMENT \/\/[^\n]*
EOL	(\r\n|\r|\n)
WHILTESPACE [[:blank:]]

INTEGER [0-9]+

CHAR \'.?\'
STRING \".+\"
BOOL_CONST "true"|"false"

IDENTIFIER [[:alpha:]_][[:alpha:][:digit:]_]*
%%

{BLOCKCOMMENT}  /* do nothing */
{LINECOMMENT}  /* do nothing */

"[" return left_br_mid;
"]" return right_br_mid;
"+" {return add;}
"++" {return add_self;}
"-" return sub;
"*" return mul;
"/" return div_char;
"%" return mod_char;
"+=" return add_assgin;
"-=" return sub_assgin;
"*=" return mul_assgin;
"/=" return div_assgin;
"%=" return mod_assgin;

"void" return Void;
"break" return BREAK;
"CONTINUE" return CONTINUE;


"==" return eql;
"!=" return noteql;
">=" return bigeql;
"<=" return smalleql;
">" return big;
"<" return small;


"||" return OR;
"&&" return AND;
"!" return NOT;


"int" {return T_INT;}
"char" {return T_CHAR;}
"const" {return T_CONST;}
"struct" {return T_STRUCT;}

"=" return LOP_ASSIGN;

";" {return  SEMICOLON;}

"(" return left_br_small;
")" return right_br_small;

"{" {return left_br_big;}

"}" {return right_br_big;}

"return" return Return;
"if" return If;
"else" return Else;
"while" return While;
"for" return For;
"scanf" return Scanf;
"printf" return Printf;
"&" return Get_Addr;
"," return Interval;
"." return Get_Member;



{INTEGER} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->vartype = CONST_INT_TYPE;
    node->int_val = atoi(yytext);
    yylval = node;
    return INTEGER;
}

{CHAR} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->vartype = CONST_CHAR_TYPE;
    node->ch_val = yytext[0];
    yylval = node;
    return CHAR;
}

{BOOL_CONST} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->vartype = CONST_BOOL_TYPE;
    node->b_val = string(yytext)==string("true");
    yylval = node;
    return BOOL_CONST;
}

{IDENTIFIER} {
    TreeNode* node = new TreeNode(lineno, NODE_VAR);
    node->var_name = string(yytext);
    yylval = node;
    return IDENTIFIER;
}


{STRING} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->vartype = CONST_STRING_TYPE;
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