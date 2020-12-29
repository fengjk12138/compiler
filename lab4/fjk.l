%option noyywrap
%top{
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<vector>
#include<string>
#include<map>
using namespace std;
}

%{
int begin_def=0;
int tot_user=0;
struct block{
    map<string,int>user; 
    block* fa=NULL;
};
block* root=new block;


%}

allnotebegin "/*"
allnoteend "*/"
allnote .|\n
linenotebegin "//"
linenoteend \n
linenote .



lines \n
tab \t
space " "

left_br_small "("
right_br_small ")"
left_br_mid "["
right_br_mid "]"
left_br_big "{"
rigth_br_big "}"

return "return"
if "if"
const "const"
int "int"
while "while"
for "for"
do "do"
char "char"
end ";"
main "main"

ID [a-zA-Z_][a-zA-Z0-9_]*
number "0"|([1-9][0-9]*)

add "+"
sub "-"
mul "*"
div "/"
mod "%"


equal "=="
smallthan "<"
smalleq "<="
bigthan ">"
bigequal ">="

and "&&"
or "||"
not "!"

assign "="

%x ALLNOTE
%x LINENOTE

%%

{allnotebegin}  {BEGIN ALLNOTE; printf("allnotebegin \t /* \n");}
<ALLNOTE>{allnote}  {printf("allnote \t  %s \n",yytext);}
<ALLNOTE>{allnoteend}   {BEGIN INITIAL;printf("allnoteend \t */  \n");}
{linenotebegin} {BEGIN LINENOTE; printf("linenotebegin \t // \n");}
<LINENOTE>{linenote}    {printf("linenote \t  %s \n",yytext);}
<LINENOTE>{linenoteend} {BEGIN INITIAL;}

{lines} {}
{tab}   {}
{space} {}

{left_br_small} {printf("left_br_small \t  %s \n",yytext);}
{right_br_small} {printf("right_br_small \t  %s \n",yytext);}
{left_br_mid} {printf("left_br_mid \t  %s \n",yytext);}
{right_br_mid} {printf("right_br_mid \t  %s \n",yytext);}
{left_br_big} {printf("left_br_big \t  %s \n",yytext);
    block* nxtblock=new block;
    nxtblock->fa=root;
    root=nxtblock;
    begin_def=0;
}
{rigth_br_big} {printf("rigth_br_big \t  %s \n",yytext);
    block* tmp=root;
    root=root->fa;
    tmp->user.clear();
    delete tmp;
}


{return} {printf("return \t  %s \n",yytext);}
{if} {printf("if \t  %s \n",yytext);}
{const} {printf("const \t  %s \n",yytext);}
{int} {printf("int \t  %s \n",yytext); begin_def=1;}
{while} {printf("while \t  %s \n",yytext);}
{for} {printf("for \t  %s \n",yytext);}
{do} {printf("do \t  %s \n",yytext);}
{char} {printf("char \t  %s \n",yytext);}
{end} {printf("end \t  %s \n",yytext);begin_def=0;}
{main} {printf("main \t  %s \n",yytext);}

{add} {printf("add \t  %s \n",yytext);}
{sub} {printf("sub \t  %s \n",yytext);}
{mul} {printf("mul \t  %s \n",yytext);}
{div} {printf("div \t  %s \n",yytext);}
{mod} {printf("mod \t  %s \n",yytext);}


{equal} {printf("equal \t  %s \n",yytext);}
{smallthan} {printf("smallthan \t  %s \n",yytext);}
{smalleq} {printf("smalleq \t  %s \n",yytext);}
{bigthan} {printf("bigthan \t  %s \n",yytext);}
{bigequal} {printf("bigequal \t  %s \n",yytext);}

{and} {printf("and \t  %s \n",yytext);}
{or} {printf("or \t  %s \n",yytext);}
{not} {printf("not \t  %s \n",yytext);}

{assign} {printf("assign \t  %s \n",yytext);}

{number} {printf("number \t  %s \n",yytext);}

{ID} {
    string nowuser(yytext);
    block* nowsearch=root;
    if(begin_def)
    {
        root->user[nowuser]=++tot_user;
        printf("ID \t  %s \t %d\n",yytext,root->user[nowuser]);
    }else{
        int outnum;
        do
        {
            if(nowsearch->user.find(nowuser)==nowsearch->user.end())
                nowsearch=nowsearch->fa;
            else{
                printf("ID \t  %s \t %d\n",yytext,nowsearch->user[nowuser]);
                break;
            }
        }while(nowsearch!=NULL);
        if(nowsearch==NULL)
        {
            root->user[nowuser]=++tot_user;
            printf("ID \t  %s \t %d\n",yytext,root->user[nowuser]);
        }
    }
}

%%

int main(){
    yyFlexLexer lexer(&cin);
    lexer.yylex();
    return 0;
}

