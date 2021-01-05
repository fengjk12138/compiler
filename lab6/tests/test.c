// #include<stdio.h>
struct we{
    int a;
    int b;
    int c[25];
    int d[25][34];
};
struct we add,a[50];

int main(){
    // scanf("%d %d",&add.a, &a.a);
    // a.a-=add.a;
    // printf("%d %d\n",add.a,a.a);
    
    for(int i=0;i<=24;i++)
        add.c[i]=i;
    for(int i=0;i<=10;i++)
        a[i].c[i]=i;
    struct we optd,oo[45];
    int tmp=0;
    for(int i=0;i<=10;i++)
        for(int j=1;j<=23;j++){
            oo[i].d[i][j]=i*j;
            tmp+=oo[i].d[i][j];
        }
    printf("%d\n",tmp);
    scanf("%d",&add.a);
    for(int i=1;i<=9;i++)
        for(int j=2;j<=23;j++){ 
        printf("%d\n",add.a);
            add.a+=oo[i].d[i][j]*oo[i].d[i][j];
            tmp+=tmp-(-2)*tmp; 
            } 
            printf("%d\n",tmp);
    printf("%d\n",add.a);
}