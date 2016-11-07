#include "helper.h"
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
#define FILENAME "work_file.txt"
using namespace std;
#define PORTNO 7500
int main()
{
	int sd,ls;
	socklen_t clen;
	char buf[8];
	char *t;
	FILE *fp;
	struct sockaddr_in sadd,cadd;
	
	//create server socket
	ls=socket(AF_INET, SOCK_STREAM,0);
	sadd.sin_family=AF_INET;
	sadd.sin_addr.s_addr=INADDR_ANY;
	sadd.sin_port=htons(PORTNO);
	bind(ls,(struct sockaddr *)&sadd, sizeof(sadd));
	clen=sizeof(cadd);
	listen(ls,5);
	sd=accept(ls,(struct sockaddr *)&cadd,&clen);
	int n = recv(sd,&buf,sizeof(buf),0);
	
	//after receiving count in buffer, write the content on file
	fp = fopen(FILENAME,"w");
	long int nIterations = getIterationCount(buf); //receive number of bytes and buffer,return the no. iteration 
	long int file_size=0;
	printf("%ld",nIterations);
	while(nIterations--)
	{
		n = recv(sd,&buf,sizeof(buf),0);
		fseek(fp,file_size,SEEK_SET);
		if(n>0&&n<9)
			cout<<n<<" ";
		fwrite (buf,sizeof(char),n,fp);
			//printf("%s",buf);
		file_size+=n;
	}
	close(sd);
	return 0;
}
