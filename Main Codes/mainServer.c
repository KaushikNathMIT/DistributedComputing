#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<time.h>
#define BUF_SIZE 8
#define PORTNO 7000
#define WORKERS 5


int getFileSize(FILE *fp)
{
	fseek(fp,0,SEEK_END);
	return ftell(fp);
}

char* getPartition(const char* filename,int split_size,int partno)
{
	FILE *fp = fopen(filename,"r");
	int byteToReach = split_size*partno;
	
	fseek(fp,byteToReach,SEEK_SET);
	/*while(fgetc(fp)!='\n')
	{	
		printf("%c",fgetc(fp));		
		fp--;
	} */  //this is the right pointer
	rp=fp;
	
	//now setting the left pointer
	byteToReach = byteToReach - split_size;
	printf("%d",byteToReach);
	fseek(fp,byteToReach,SEEK_SET);
/*	if(byteToReach!=0)
		while(fgetc(fp)!=',')
		{
			printf("%c",fgetc(fp));		
			fp--;	
		}*/
	lp=++fp;
	
//	char* partition = (char*)malloc(
	/*for(FILE* i=lp;i<=rp;i++)
	{
		printf("%c",fgetc(i));		
	}*/	
	
	return NULL;
}


long int getIterationCount(char buf[])
{
	int i,c=1;
	long int ans=0;
	for(int i=0;buf[i]!='\0';i++)   //buf stores number in reverse in character form
	{
	
		ans+=(buf[i]-'0')*c;
		c*=10;
	}	
	return ans;
}
int main()
{
	int sd,ls,clen;
	char buf[BUF_SIZE];
	char *t;
	FILE *fp;
	struct sockaddr_in sadd,cadd;
	ls=socket(AF_INET, SOCK_STREAM,0);
	sadd.sin_family=AF_INET;
	sadd.sin_addr.s_addr=inet_addr("127.0.0.1");
	sadd.sin_port=htons(PORTNO);
	
	bind(ls,(struct sockaddr *)&sadd, sizeof(sadd));
	
	listen(ls,5);
	
	clen=sizeof(cadd);
	for(;;)
	{
		sd=accept(ls,(struct sockaddr *)&cadd,&clen);
		if(!fork())
		{
			close(ls);
			int file_size=0;
			int nbytes=recv(sd,&buf,sizeof(buf),0);
			long int nIterations = getIterationCount(buf); //receive number of bytes and buffer,return the no. iteration 
			fp=fopen("dataset_main_server.txt","w");
			while(nIterations--)
			{
			
				int n = recv(sd,&buf,sizeof(buf),0);
				fseek(fp,file_size,SEEK_SET);	
				fwrite (buf , sizeof(char),n,fp);
				file_size+=n;
			}
			
			//now we have received the file, 
			file_size = getFileSize(fp);  
			int splilt_size= file_size/WORKERS; //in bytes
			char *buffer = getPartition("dataset_main_server.txt",split_size,partno); //partno starts from one
			
			
			close(sd);
			exit(0);
		}
		close(sd);
	}
	return 0;
}
