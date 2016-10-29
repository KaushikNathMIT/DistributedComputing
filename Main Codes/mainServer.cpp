#include<iostream>
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
#define WORKERS 1000

#include "helper.h"

using namespace std;



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
	int sd,ls;
	socklen_t clen;
	char buf[BUF_SIZE];
	char *t;
	FILE *fp;
	struct sockaddr_in sadd,cadd;
	ls=socket(AF_INET, SOCK_STREAM,0);
	sadd.sin_family=AF_INET;
	sadd.sin_addr.s_addr=INADDR_ANY;
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
			
				int n = recv(sd,&buf,sizeof(buf),0);;
				fseek(fp,file_size,SEEK_SET);	
				fwrite (buf,sizeof(char),n,fp);
				file_size+=n;
			}
			
			//now we have received the file, 
			file_size = getFileSize(fp);  
			fclose(fp);
			int split_size= file_size/WORKERS; //in bytes
			for(int partno=1;partno<=WORKERS;partno++)
			{
				string buffer = getPartition("dataset_main_server.txt",split_size,partno); 
				cout<<buffer;
			}
			close(sd);
			exit(0);
		}
		close(sd);
	}
	return 0;
}
