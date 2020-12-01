%option nounput
%{
#include "common.h"
#include "main.tab.h"  // yacc header
int lineno=1;
int beginDef=0;
VarNode *root_var = new VarNode;
VarNode *now = root_var;
%}
BLOCKCOMMENT \/\*([^\*^\/]*|[\*^\/*]*|[^\**\/]*)*\*\/
LINECOMMENT \/\/[^\n]*
EOL	(\r\n|\r|\n)
WHILTESPACE [[:blank:]]

INTEGER [0-9]+

CHAR \'.?\'
STRING \".+\"
BOOL_CONST "true"|"false"

left_br_mid "["
right_br_mid "]"

IDENTIFIER [[:alpha:]_][[:alpha:][:digit:]_]*
%%

{BLOCKCOMMENT}  /* do nothing */
{LINECOMMENT}  /* do nothing */


"+" return add;
"-" return sub;
"*" return mul;
"/" return div_char;
"%" return mod_char;

"==" return eql;
"!=" return noteql;
">=" return bigeql;
"<=" return smalleql;
">" return big;
"<" return small;

"||" return OR;
"&&" return AND;
"!" return NOT;


"int" {beginDef=1; return T_INT;}
"char" {beginDef=2;return T_CHAR;}

"=" return LOP_ASSIGN;

";" {beginDef=0;return  SEMICOLON;}

"(" return left_br_small;
")" return right_br_small;

"{" {VarNode *tmp=new VarNode;now->addChild(tmp);now = tmp;return left_br_big;}

"}" {now = now->fa;return right_br_big;}

"return" return Return;
"if" return If;
"while" return While;
"for" return For;
"main" return Main;
"scanf" return Scanf;
"printf" return Printf;
"&" return Get_Addr;
"," return Interval;

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

{BOOL_CONST} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_BOOL;
    node->b_val = string(yytext)==string("true");
    yylval = node;
    return BOOL_CONST;
}

{IDENTIFIER} {
    TreeNode* node = new TreeNode(lineno, NODE_VAR);
    node->var_name = string(yytext);


    if(beginDef){
        now -> var[node->var_name]=make_pair(++VarNode::nodeID, beginDef == 1 ? TYPE_INT : TYPE_CHAR);
        node->var_id = VarNode::nodeID;
    }else{
        VarNode *tmp = now;
        int thisNodeid=-1;
        while(now != nullptr){
            if(now->var.find(node->var_name) == now->var.end()){
                now=now->fa;
            }else{
                thisNodeid=now -> var[node->var_name].first;
                break;
            }
        }
        node->var_id = thisNodeid;
    }
    yylval = node;
    return IDENTIFIER;
}


{STRING} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_STRING;
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