%{
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#ifndef YYSTYPE
#define YYSTYPE char*
#endif
char idStr[50];
char numStr[50];
int yylex();
extern int yyparse();
FILE* yyin;
void yyerror(const char* S);

%}

%token ADD
%token MUL
%token DIV
%token SUB
%token ID
%token LEFT_BR
%token RIGHT_BR
%token NUMBER
%left ADD SUB
%left MUL DIV
%right UMINUS
%%

lines   :   lines expr ';' {printf("%s\n", $2);}
        |   lines ';'
        |
        ;
expr    :   expr ADD expr {$$ =(char *)malloc(50*sizeof(char));  strcpy($$,$1);  strcat($$, $3);strcat($$,"+ ");}
        |   expr SUB expr {$$ =(char *)malloc(50*sizeof(char));  strcpy($$,$1);  strcat($$, $3);strcat($$,"- ");}
        |   expr MUL expr {$$ =(char *)malloc(50*sizeof(char));  strcpy($$,$1);  strcat($$, $3);strcat($$,"* ");}
        |   expr DIV expr {$$ =(char *)malloc(50*sizeof(char));  strcpy($$,$1);  strcat($$, $3);strcat($$,"/ ");}
        |   LEFT_BR expr RIGHT_BR  {$$ = $2;}
        |   SUB expr {$$ =(char *)malloc(50*sizeof(char));  strcpy($$,"0 ");  strcat($$, $2);strcat($$,"- ");}
        |   NUMBER {$$ =(char *)malloc(50*sizeof(char));  strcpy($$,$1);  strcat($$, " ");}
        |   ID  {$$ =(char *)malloc(50*sizeof(char));  strcpy($$,$1);  strcat($$, " ");}
        ;
%%

int yylex()
{
    
    int t;
    while(1)
    {
    t=getchar();
    if(t=='+')
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
        yylval=(char *)malloc(50*sizeof(char)); 
        int len=0;
        while(isdigit(t))
            {
                yylval[len]=t;
                len++;
                t=getchar();
            }
        ungetc(t,stdin);
        yylval[len]='\0';
        return NUMBER;
    }
    else if(isalpha(t)||isdigit(t)||t=='_')
    {
         yylval=(char *)malloc(50*sizeof(char)); 
        int len=0;
        while(isalpha(t)||isdigit(t)||t=='_')
            {
                yylval[len]=t;
                len++;
                t=getchar();
            }
        ungetc(t,stdin);
        yylval[len]='\0';
        return ID;

    }
    else return t;
    }
}

int main(void)
{
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