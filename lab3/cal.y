%{
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>
#ifndef YYSTYPE
#define YYSTYPE double
#endif
#define HASH_LEN 114514
double val_of_if[HASH_LEN];
void save_val(double,double);
double get_val(double);
int yylex();
int yyparse();
FILE* yyin;
void yyerror(const char* S);

%}

%token ADD
%token MUL
%token DIV
%token SUB
%token ID
%token GIVEN
%token LEFT_BR
%token RIGHT_BR
%token NUMBER
%right GIVEN
%left ADD SUB
%left MUL DIV
%right UMINUS
%%

lines   :   lines expr ';' {printf("%f\n", $2);}
        |   lines ';'
        |   lines factor ';' {printf("%f\n", get_val($2));}
        |
        ;
expr    :   expr ADD expr {$$ = $1 + $3;}
        |   expr SUB expr {$$ = $1 - $3;}
        |   expr MUL expr {$$ = $1 * $3;}
        |   expr DIV expr {$$ = $1 / $3;}
        |   LEFT_BR expr RIGHT_BR  {$$ = $2;}
        |   SUB expr {$$ = -$2;}
        |   NUMBER
        |   ID  {$$ = get_val($1);}
        ;
factor  :   ID GIVEN expr {$$=$1;save_val($1, $3);}
        ;

%%

void save_val(double item, double val)
{
    // extern double val_of_if[HASH_LEN];
    int tmp=item+0.5;
    val_of_if[tmp]=val;
}
double get_val(double item)
{
    // extern double val_of_if[HASH_LEN];
    int tmp=item+0.5;
    return val_of_if[tmp];
}
int yylex()
{
    
    int t;
    while(1)
    {
    t=getchar();
    if(t=='=')
        return GIVEN;
    else if(t=='+')
        return ADD;
    else if(t=='-')
        return SUB;
    else if(t=='*')
        return MUL;
    else if(t=='/')
        return DIV;
    else if(t=='(')
        return LEFT_BR;
    else if(t==')')
        return RIGHT_BR;
    else if(isspace(t))
        continue;
    else if(isdigit(t)) {
        yylval=0;
        while(isdigit(t))
            {
                yylval=yylval*10+t-'0';
                t=getchar();
            }
        ungetc(t,stdin);
        return NUMBER;
    }
    else if(isalpha(t)||isdigit(t)||t=='_')
    {
        int tmp_hash=0;
       
        int len=0;
        while(isalpha(t)||isdigit(t)||t=='_')
            {
                tmp_hash=(tmp_hash*64+'t')%HASH_LEN;
                t=getchar();
            }
        ungetc(t,stdin);
        yylval=tmp_hash;
        return ID;

    }
    else return t;
    }
    return t;
}

int main(void)
{ 
    for(int i=0;i<HASH_LEN;i++)
        val_of_if[i]=0;
    yyin = stdin;
    do{
        yyparse();
    }while(!feof(yyin));
    return 0;
}
void yyerror(const char *s)
{
    fprintf(stderr, "Parse error: %s\n", s);
    exit(1);
}