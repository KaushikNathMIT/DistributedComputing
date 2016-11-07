#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<time.h>
#include<iostream>
#include<string>
#define PORTNO 6969
#define BUF_SIZE 8
using namespace std;
#include "helper.h"

void printString(char* buf,int size)
{
	for(int i=0;i<size;i++)
		printf("%c",buf[i]);
}

int writeContentsToBuf(FILE **fp,char *buf,long int bytes_written,int size)
{

	fread(buf,size,1, *fp); /* Read 1 chunk of size bytes from fp into buffer */
	return size;
}

void readSend(FILE* fp,int sd)
{

	long int bytes_written=0;
	fseek(fp,0,SEEK_END);
	long int fsize = ftell(fp);   //total no. of bytes in file
	rewind(fp);
	char *buf = (char*) malloc(BUF_SIZE * sizeof(*buf)); //using this same buf
	for(long int i=0;i<fsize/BUF_SIZE;i++) //full size buffers
	{
	 	 if(writeContentsToBuf(&fp,buf,bytes_written,BUF_SIZE) != BUF_SIZE)  //error checking
	 	 	printf("Bytes read unsuccesful\n");
	 	 //sleep(0.01);	 	 	
	 	 int n=send(sd,buf,BUF_SIZE,0);
	 	 if(n!=BUF_SIZE)
	 	 	printf("Send unsucessful\n");
	 	 bytes_written+=BUF_SIZE;
	 	 fseek(fp,bytes_written,SEEK_SET);
	}
	long int leftover = fsize%BUF_SIZE;
	if(writeContentsToBuf(&fp,buf,bytes_written,leftover)!=leftover)
		printf("Bytes read unsuccesful\n");	 	 	
	int n=send(sd,buf,leftover,0);
	if(n!=leftover)
	 	printf("Send unsucessful\n");
	 bytes_written+=BUF_SIZE;
	 free(buf);
}

int main()
{
	int sd,i;
	struct sockaddr_in addrs;
	sd=socket(AF_INET, SOCK_STREAM,0);
	addrs.sin_family=AF_INET;
	addrs.sin_addr.s_addr=inet_addr("127.0.0.1");    //main server ip address
	addrs.sin_port=htons(PORTNO);
	int len=sizeof(addrs);
	int result=connect(sd,(struct sockaddr *)&addrs,len);
	
	//open file
	FILE *fp = fopen("dataset.txt","r");
	
	//send size of file
	sendIterationCount(fp,sd,"");
	
	//read a file, 8 bytes at a time then send them one by one
	readSend(fp,sd);
	//getting timestamp in return
	//int n = recv(sd,&buf,sizeof(buf),0);
//	printf("The server time is\n");
	//printString(buf,n);
	//printf("%d\n",n);
	//for(i=0;i<n;i++)
	//	printf("%c",buf[i]);
	return 0;
}
