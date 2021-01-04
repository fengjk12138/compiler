int a=10;
int c=12;
int b;
int tem[123];
int main(){
   c=a;
  int a;
  int t[10][10];
  for(int i=0;i<10;i++)
    for(int j=0;j<10;j++)
      t[i][j]=0;
  scanf("%d %d", &a,&b);
  for(int i=1;i<=a;i++)
    for(int j=1;j<=b;j++)
      t[i][j]=a-b+t[i-1][j-1];
  printf("%d\n",a);
  for(int i=1;i<=a;i++)
    for(int j=1;j<=b;j++)
    {
    	// printf("%d %d\n",i, a);
      // printf("%d %d\n",j, b);
      c+=t[i][j]*23;
      // printf("%d\n", c);
  }
  printf("%d\n",c);
// ; pushl (%eax)
// ; pushl $STR1
// ; call printf
// ; addl $8, %esp

// ; pushl (%eax)
// ; pushl $STR1
// ; call printf
// ; addl $8, %esp

  return 0;
}