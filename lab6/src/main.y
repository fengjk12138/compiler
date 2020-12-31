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

%token left_br_small right_br_small left_br_mid right_br_mid left_br_big right_br_big

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
:  statement {$$= new TreeNode(lineno, NODE_STMT); $$->stype = STMT_BLOCK; $$->addChild($1);}
|  statements statement {$$=$1; $$->addSibling($2);}
;

program_block : left_br_big statements right_br_big{
	TreeNode* node = new TreeNode($2->lineno, NODE_STMT);
    	node->stype = STMT_BLOCK;
    	node->addChild($2);
    	$$ = node;
}
| left_br_big right_br_big{
	TreeNode* node = new TreeNode(lineno, NODE_STMT);
    	node->stype = STMT_BLOCK;
    	$$ = node;
}
;

program_sentense: program_block {$$=$1;}
| statement {$$=$1;}
;

declaration_or_empty: declaration {$$=$1;}
| {$$=new TreeNode(lineno, NODE_EMPTY);}
;

expr_or_empty:expr {$$=$1;}
| {$$=new TreeNode(lineno, NODE_EMPTY);}
;
ASSIGN_or_empty: ASSIGN {$$=$1;}
| {$$=new TreeNode(lineno, NODE_EMPTY);}
;

statement
: SEMICOLON  {$$ = new TreeNode(lineno, NODE_STMT); $$->stype = STMT_SKIP;}
| declaration SEMICOLON {$$ = $1;}
| ASSIGN SEMICOLON {$$ = $1;}
| BREAK SEMICOLON {$$ = new TreeNode(lineno, NODE_STMT); $$->stype = STMT_BREAK;}
| CONTINUE SEMICOLON {$$ = new TreeNode(lineno, NODE_STMT); $$->stype = STMT_CONTINUE;}
| STRUCT_DEFINE SEMICOLON {$$ = $1;}
| FUNCTION {$$ = $1;}
| Return expr_or_empty SEMICOLON {
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

//函数声明定义
parameter_list: T IDENTIFIER {
	$$ =new TreeNode($2->lineno, NODE_FORMT);
	$$->ftype=PARAM_LIST;
	$$ -> addChild($1);
	$$ -> addChild($2);
}
| T IDENTIFIER Interval parameter_list {
	$$ = $4;
	$$ -> addChild($1);
        $$ -> addChild($2);
}
;
parameter_list_or_empty: parameter_list {$$ = $1;}
| {$$=new TreeNode(lineno, NODE_EMPTY);}
;


FUNCTION: T IDENTIFIER left_br_small parameter_list_or_empty right_br_small program_sentense{
	$$ = new TreeNode($1->lineno,NODE_FUNC);
	$$ -> var_name=$2->var_name;
	$$ -> addChild($1);
	$$ -> addChild($2);
	$$ -> addChild($4);
	$$ -> addChild($6);
}
;

//结构体定义
STRUCT_DEFINE: T_STRUCT IDENTIFIER program_block{
	$$ = new TreeNode($1->lineno,NODE_STRUCT);
	$$ -> var_name=$2->var_name;
	$$ -> addChild($2);
	$$ -> addChild($3);
}
;


//变量定义
T: T_INT {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_INT;}
| T_CHAR {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_CHAR;}
| T_BOOL {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_BOOL;}
| T_CONST T_INT {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_INT_CONST;}
| T_CONST T_CHAR {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_CHAR_CONST;}
| T_INT mul {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_INT_POINTER;}
| T_CHAR mul {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_CHAR_POINTER;}
| T_STRUCT IDENTIFIER {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_COMPOSE_STRUCT;}
| Void {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_VOID;}
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
Array_Dim: left_br_mid expr right_br_mid Array_Dim{
	if($4->nodeType==NODE_EMPTY){
		$$=new TreeNode($2->lineno, NODE_FORMT);
		$$->ftype=ARRAY_DIM;
		$$->addChild($2);
	}else{
		$$=new TreeNode($2->lineno, NODE_FORMT);
		$$->ftype=ARRAY_DIM;
		$$->addChild($2);
		$$->addChild($4->child);
	}
}
| {$$ =new TreeNode(lineno, NODE_EMPTY);}
;


//左值  右值 可以被复制 如 a[100] a[i]  a b  a[c+d] a.c这种
IDENTIFIER_val: IDENTIFIER Array_Dim{
	$$ =new TreeNode($1->lineno, NODE_VAR);
	$$->addChild($1);
	$$->vartype=VAR_TYPE;
	if($2->nodeType!=NODE_EMPTY){
		$$->addChild($2);
		$$->vartype=ARRAY_TYPE;
	}

}
| IDENTIFIER_val Get_Member IDENTIFIER_val{
	$$ =new TreeNode($1->lineno, NODE_VAR);
	$$->addChild($1);
	$$->addChild($3);
	$$->vartype=STRUCT_TYPE;
}
;

define_list_inter: IDENTIFIER_val LOP_ASSIGN expr Interval define_list_inter{
	TreeNode* node = new TreeNode($1->lineno, NODE_FORMT);
	node->ftype = DEFINE_LIST;
	TreeNode* node2 = new TreeNode($1->lineno, NODE_FORMT);
        node2->ftype = DEFINE_FORMAT_INIT;
	node2->addChild($1);
	node2->addChild($3);
	node->addChild(node2);
	node->addChild($5->child);
	$$ = node;
}
| IDENTIFIER_val Interval define_list_inter{
	TreeNode* node = new TreeNode($1->lineno, NODE_FORMT);
	node->ftype = DEFINE_LIST;
	TreeNode* node2 = new TreeNode($1->lineno, NODE_FORMT);
	node2->ftype = DEFINE_FORMAT;
	node2->addChild($1);
	node->addChild(node2);
	node->addChild($3->child);
	$$ = node;
}
| IDENTIFIER_val LOP_ASSIGN expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_FORMT);
	node->ftype = DEFINE_LIST;
	TreeNode* node2 = new TreeNode($1->lineno, NODE_FORMT);
	node2->ftype = DEFINE_FORMAT_INIT;
	node2->addChild($1);
	node2->addChild($3);
	node->addChild(node2);
	$$ = node;
}
| IDENTIFIER_val{
	TreeNode* node = new TreeNode($1->lineno, NODE_FORMT);
	node->ftype = DEFINE_LIST;
	TreeNode* node2 = new TreeNode($1->lineno, NODE_FORMT);
	node2->ftype = DEFINE_FORMAT;
	node2->addChild($1);
	node->addChild(node2);
	$$ = node;
}
;

//表达式

Cal_expr: expr add expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = ADD;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr sub expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = SUB;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr mul expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = MUL;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr div_char expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = DIV;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr mod_char expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = MOD;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| sub expr %prec UMINUS{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = NEG;
	node->addChild($2);
	$$ = node;
}
| add expr %prec UMINUS{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = POS;
	node->addChild($2);
	$$ = node;
}
| mul IDENTIFIER_val %prec UMINUS{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = POINTER;
	node->addChild($2);
	$$ = node;
}
| Get_Addr IDENTIFIER_val %prec UMINUS{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = ADDRESS;
	node->addChild($2);
	$$ = node;
}
;

Bool_expr: expr eql expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = EQL;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr noteql expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = NOTEQL;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr bigeql expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = BIGEQL;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr smalleql expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = SMALLEQL;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr big expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = BIG;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr small expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = SMALL;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
;

Bool_Cal_expr: expr OR expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = OR_BOOL;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| expr AND expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = AND_BOOL;
	node->addChild($1);
	node->addChild($3);
	$$ = node;
}
| NOT expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = NOT_BOOL;
	node->addChild($2);
	$$ = node;
}
;

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
| left_br_small expr right_br_small	{$$ = $2;}
| Cal_expr	{$$ = $1;}
| Bool_expr	{$$ = $1;}
| Bool_Cal_expr	{$$ = $1;}
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
	TreeNode* node = new TreeNode($2->lineno, NODE_FORMT);
   	node->ftype = SCANF_FORMAT;
   	node->addChild($2);
   	if($3!=nullptr){
    		node->addSibling($3);
    	}
    	$$ = node;
}
| {$$=nullptr;}

Print_format : Interval expr Print_format{
	TreeNode* node = new TreeNode($2->lineno, NODE_FORMT);
	node->ftype = PRINT_FORMAT;
	node->addChild($2);
	if($3!=nullptr){
		node->addSibling($3);
	}
	$$ = node;
}
| { $$=nullptr;}

%%

int yyerror(char const* message)
{
  cout << message << " at line " << lineno << endl;
  return -1;
}