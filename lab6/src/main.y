%{
    #include "common.h"
    #define YYSTYPE TreeNode *  
    TreeNode* root;
    extern int lineno;
    int yylex();
    int yyerror( char const * );
%}
%token T_CHAR T_INT T_STRING T_BOOL T_CONST T_STRUCT

%token LOP_ASSIGN 

%token SEMICOLON

%token IDENTIFIER INTEGER CHAR BOOL STRING

%token BOOL_CONST
%token Else
%token Void Scanf Printf Get_Addr Interval

%token left_br_small right_br_small left_br_mid rigth_br_mid left_br_big right_br_big

%token Return If While For Main BREAK CONTINUE

%left LOP_EQ
%left add_assgin sub_assgin mul_assgin div_assgin mod_assgin
%left NOT
%left OR
%left AND
%left eql noteql
%left bigeql smalleql big small
%left add sub
%left mod_char mul div_char Get_Addr
%left UMINUS
%left add_self
%left left_br_small right_br_small left_br_mid rigth_br_mid Get_Member

%%

program
: statements {root = new TreeNode(0, NODE_PROG); root->addChild($1);};



statements
:  statement {$$=$1;}
|  statements program_sentense {$$=$1; $$->addSibling($2);}
;

program_block : left_br_big statements right_br_big{
	TreeNode* node = new TreeNode($2->lineno, NODE_STMT);
    	node->stype = STMT_BLOCK;
    	node->addChild($2);
    	$$ = node;
}
| left_br_big right_br_big{
	TreeNode* node = new TreeNode(-1, NODE_STMT);
    	node->stype = STMT_BLOCK;
    	$$ = node;
}
;

program_sentense: program_block {$$=$1;} | statement {$$=$1;}

declaration_or_empty: declaration
|
;

expr_or_empty:expr
|
;
ASSIGN_or_empty: ASSIGN
|
;

statement
: SEMICOLON  {$$ = new TreeNode(lineno, NODE_STMT); $$->stype = STMT_SKIP;}
| declaration SEMICOLON {$$ = $1;}
| ASSIGN SEMICOLON {$$ = $1;}
| BREAK SEMICOLON
| CONTINUE SEMICOLON
| Return expr SEMICOLON {
	TreeNode* node = new TreeNode($2->lineno, NODE_STMT);
	node->stype = STMT_RET;
	node->addChild($2);
	$$ = node;
}
| Scanf left_br_small STRING Scanf_format right_br_small SEMICOLON{
	TreeNode* node = new TreeNode($3->lineno, NODE_STMT);
	node->stype = STMT_SCANF;
	node->addChild($3);
	node->addChild($4);
	$$ = node;
}
| Printf left_br_small STRING Print_format right_br_small SEMICOLON{
	TreeNode* node = new TreeNode($3->lineno, NODE_STMT);
	node->stype = STMT_PRINT;
	node->addChild($3);
	node->addChild($4);
	$$ = node;
}
| While left_br_small expr right_br_small program_sentense{
	TreeNode* node = new TreeNode($3->lineno, NODE_STMT);
	node->stype = STMT_WHILE;
	node->addChild($3);
	node->addChild($5);
	$$ = node;
}
| For left_br_small declaration_or_empty  SEMICOLON  expr_or_empty SEMICOLON ASSIGN_or_empty right_br_small program_sentense{
	TreeNode* node = new TreeNode($3->lineno, NODE_STMT);
	node->stype = STMT_FOR;
	node->addChild($3);
	node->addChild($5);
	node->addChild($7);
	node->addChild($9);
	$$ = node;
}
| For left_br_small ASSIGN_or_empty  SEMICOLON  expr SEMICOLON ASSIGN right_br_small program_block{
 	TreeNode* node = new TreeNode($3->lineno, NODE_STMT);
 	node->stype = STMT_FOR;
 	node->addChild($3);
 	node->addChild($5);
 	node->addChild($7);
 	node->addChild($9);
 	$$ = node;
}
| If left_br_small expr right_br_small program_sentense Else program_sentense %prec UMINUS{
	TreeNode* node = new TreeNode($3->lineno, NODE_STMT);
 	node->stype = STMT_IF_ELSE;
 	node->addChild($3);
 	node->addChild($5);
 	node->addChild($7);
 	$$ = node;
}
| If left_br_small expr right_br_small program_sentense{
	TreeNode* node = new TreeNode($3->lineno, NODE_STMT);
 	node->stype = STMT_IF;
 	node->addChild($3);
 	node->addChild($5);
 	$$ = node;
}
;
//结构体定义
STRUCT_DEFINE: T_STRUCT IDENTIFIER program_block

//变量定义
T: T_INT {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_INT;}
| T_CHAR {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_CHAR;}
| T_BOOL {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_BOOL;}
| T_CONST T_INT
| T_CONST T_CHAR
| T_INT mul
| T_CHAR mul
| T_STRUCT IDENTIFIER
;

declaration: T define_list_inter {
    TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    $$ = node;   
}
;

//数组维度
Array_Dim: left_br_mid expr right_br_mid Array_Dim
|
;


//左值  右值 可以被复制 如 a[100] a[i]  a b  a[c+d] a.c这种
IDENTIFIER_val: IDENTIFIER Array_Dim
| IDENTIFIER_val Get_Member IDENTIFIER_val
;

define_list_inter: IDENTIFIER_val LOP_ASSIGN expr Interval define_list_inter{
	TreeNode* node = new TreeNode($1->lineno, NODE_FORMT);
	node->ftype = DEFINE_FORMAT_INIT;
	node->addChild($1);
	node->addChild($3);
	node->addSibling($5);
	$$ = node;
}
| IDENTIFIER_valInterval define_list_inter{
	TreeNode* node = new TreeNode($1->lineno, NODE_FORMT);
	node->ftype = DEFINE_FORMAT;
	node->addChild($1);
	node->addSibling($3);
	$$ = node;
}
| IDENTIFIER_val LOP_ASSIGN expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_FORMT);
	node->ftype = DEFINE_FORMAT_INIT;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| IDENTIFIER_val{
	TreeNode* node = new TreeNode($1->lineno, NODE_FORMT);
	node->ftype = DEFINE_FORMAT_INIT;
	node->addChild($1);
	$$ = node;
}
;

//表达式

Cal_expr: expr add expr
| expr sub expr
| expr mul expr
| expr div_char expr
| expr mod_char expr
| sub expr %prec UMINUS
| add expr %prec UMINUS
| mul IDENTIFIER_val %prec UMINUS
| Get_Addr IDENTIFIER_val %prec UMINUS
;

Bool_expr: expr eql expr
| expr noteql expr
| expr bigeql expr
| expr smalleql expr
| expr big expr
| expr small expr
;

Bool_Cal_expr: expr OR expr
| expr AND expr
| NOT expr;


expr
: IDENTIFIER_val{
    $$ = $1;
}
| INTEGER {
    $$ = $1;
}
| CHAR {
    $$ = $1;
}
| STRING {
    $$ = $1;
}
| left_br_small expr right_br_small
| Cal_expr
| Bool_expr
| Bool_Cal_expr
;


//赋值语句
ASSIGN: IDENTIFIER_val LOP_ASSIGN expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
   	node->stype = STMT_ASSIGN;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}
| IDENTIFIER_val add_assgin expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
   	node->stype = STMT_ASSIGN_ADD;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}
| IDENTIFIER_val mul_assgin expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
   	node->stype = STMT_ASSIGN_MUL;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}
| IDENTIFIER_val sub_assgin expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
   	node->stype = STMT_ASSIGN_SUB;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}
| IDENTIFIER_val div_assgin expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
   	node->stype = STMT_ASSIGN_DIV;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}
| IDENTIFIER_val mod_assgin expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
   	node->stype = STMT_ASSIGN_MOD;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}
| IDENTIFIER_val add_self{
      TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
      node->stype = STMT_ASSIGN_ADD_SELF;
      node->addChild($1);
      $$ = node;
}
;

Scanf_format:Interval expr Scanf_format{
	TreeNode* node = new TreeNode($3->lineno, NODE_FORMT);
   	node->ftype = SCANF_FORMAT_ADDR;
   	node->addChild($3);
   	if($4!=nullptr){
    		node->addSibling($4);
    	}
    	$$ = node;
}
| {$$=nullptr;}

Print_format : Interval expr Print_format{
	TreeNode* node = new TreeNode($2->lineno, NODE_FORMT);
	node->ftype = PRINT_FORMAT;
	if($3!=nullptr){
		node->addSibling($3);
	}
	node->addChild($2);
	$$ = node;
}
| { $$=nullptr;}

%%

int yyerror(char const* message)
{
  cout << message << " at line " << lineno << endl;
  return -1;
}