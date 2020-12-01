%{
    #include "common.h"
    #define YYSTYPE TreeNode *  
    TreeNode* root;
    extern int lineno;
    int yylex();
    int yyerror( char const * );
%}
%token T_CHAR T_INT T_STRING T_BOOL 

%token LOP_ASSIGN 

%token SEMICOLON

%token IDENTIFIER INTEGER CHAR BOOL STRING

%token left_br_small right_br_small left_br_mid rigth_br_mid left_br_big right_br_big

%token Return If While For Main

%left LOP_EQ
%left NOT
%left OR
%left AND
%left eql noteql
%left bigeql smalleql big small
%left add sub
%left mod mul div



%%

program
: statements {root = new TreeNode(0, NODE_PROG); root->addChild($1);};

statements
:  statement {$$=$1;}
|  statements statement {$$=$1; $$->addSibling($2);}
;

statement
: SEMICOLON  {$$ = new TreeNode(lineno, NODE_STMT); $$->stype = STMT_SKIP;}
| declaration SEMICOLON {$$ = $1;}
| ASSIGN SEMICOLON {$$ = $1;}
;

declaration
: T IDENTIFIER LOP_ASSIGN expr{  // declare and init
    TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($4);
    $$ = node;   
} 
| T IDENTIFIER {
    TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    $$ = node;   
}
;

expr
: IDENTIFIER {
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
| expr add expr{
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
    	node->exptype = SUB;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}
| expr div expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
    	node->exptype = DIV;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}
| expr mod expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
    	node->exptype = MOD;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}
| expr eql expr{
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
| sub expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
    	node->exptype = NEG;
    	node->addChild($2);
    	$$ = node;
}
| add expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
	node->exptype = POS;
	node->addChild($2);
	$$ = node;
}
| left_br_small expr right_br_small{
	TreeNode* node = new TreeNode($1->lineno, NODE_EXPR);
    	node->exptype = BR;
    	node->addChild($2);
    	$$ = node;
}
;

T: T_INT {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_INT;} 
| T_CHAR {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_CHAR;}
| T_BOOL {$$ = new TreeNode(lineno, NODE_TYPE); $$->type = TYPE_BOOL;}
;

ASSIGN: IDENTIFIER LOP_ASSIGN expr{
	TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
   	node->stype = STMT_ASSIGN;
    	node->addChild($1);
    	node->addChild($3);
    	$$ = node;
}

%%

int yyerror(char const* message)
{
  cout << message << " at line " << lineno << endl;
  return -1;
}