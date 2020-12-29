#include<stdio.h>
int input;
int a=0,b=1;
int i=1;
int result;
int Fibonacci(int n)
{
    while(i<n)
    {
        result=a+b;
        a=b;
        b=result;
        i++;
    }
    return result;
}

int main()
{
    scanf("%d",&input);
    printf("%d",Fibonacci(input));
    return 0;
}
