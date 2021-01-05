int a;
int c[23][34][3];
int main(){
    int tmp[43][43];
    scanf("%d",&a);
    for(int i=1;i<10;i++)
    for(int j=1;j<=8;j+=1)
    {
        c[i][j][1]=i-j;
    }
    while(a>0){
        a=a-1;
        for(int i=1;i<10;i++)
    for(int j=1;j<=8;j+=1)
    {
        c[i][j][1]++;
    }

        int a=0;
for(int x=1;x<8;x++)
    for(int j=1;j<=8;j+=1)
    {
        if(a==233){
        	a-=23;
        	continue;
		}
        tmp[x][j]=c[j][x][1];
        tmp[x][j]+=a;
        a-=tmp[x][j]-a/10;
        if(a<-3)
        	{
        		a=223;
        		break;
			}
		
			 
    }

        printf("%d\n",a);
    }
    printf("End Loop\n");
    return 0;
}