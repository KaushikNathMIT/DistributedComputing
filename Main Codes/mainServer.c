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
	
	if(fp==NULL)
	{
		printf("File doensn't exist\n");
		return NULL;
	}
	
	int byteToReach = split_size*partno;
	FILE *start,*end,*temp;
	start=end=fp;
	int i=0;
	fseek(end,byteToReach,SEEK_SET);
	if(partno!=WORKERS)
	{
			while(fgetc(end)!='\n')
			{			
				i++;
				fseek(end,byteToReach-i,SEEK_SET);
			}   //this is the end pointer
	}
	else //pickup the last record
	{
		fseek(end,0,SEEK_END);
		
	}
	long int end_point=ftell(end);
	
	
	//now setting the start pointer
	byteToReach = byteToReach - split_size;
	i=0;
	fseek(start,byteToReach-i,SEEK_SET);
	if(byteToReach!=0)
	{
		while(fgetc(start)!='\n')
		{
			i++;
			fseek(start,byteToReach-i,SEEK_SET);
			
		}
		fseek(start,0,SEEK_CUR);
	}
	int start_point=ftell(start);
	FILE *j=start;
	
	int c=end_point-start_point-1;
	while(c)
	{
		printf("%c",fgetc(j));
		c--;
		fseek(j,0,SEEK_CUR);  //* j increments automatically
	}
	
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
				fwrite (buf,sizeof(char),n,fp);
				file_size+=n;
			}
			
			//now we have received the file, 
			file_size = getFileSize(fp);  
			int splilt_size= file_size/WORKERS; //in bytes
			
			for(int partno=1;partno<=WORKERS;partno++)
			{
				char *buffer = getPartition("dataset_main_server.txt",split_size,partno); 
				printf("\n$\n");
			}
			close(sd);
			exit(0);
		}
		close(sd);
	}
	return 0;
}
