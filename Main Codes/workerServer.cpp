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
#include <sstream>
using namespace std;






int main()
{
	int sd,ls;
	socklen_t clen;
	char buf[8];
	char ch[1];
	char *resultBuf;
	char *t;
	double *value=NULL;
	string str[100000];
	FILE *fp;
	struct sockaddr_in sadd,cadd;
	
	//create server socket
	ls=socket(AF_INET, SOCK_STREAM,0);
	sadd.sin_family=AF_INET;
	sadd.sin_addr.s_addr=INADDR_ANY;
	sadd.sin_port=htons(WORKER_SERVER_PORT);
	bind(ls,(struct sockaddr *)&sadd, sizeof(sadd));
	clen=sizeof(cadd);
	listen(ls,5);
	sd=accept(ls,(struct sockaddr *)&cadd,&clen);
	int n = recv(sd,&buf,sizeof(buf),0);	
	long int nIterations = getIterationCount(buf); //receive number of bytes and buffer,return the no. iteration 
	
	//now receive choice
	recv(sd,&ch,sizeof(ch),0);
	
	//after receiving count in buffer, write the content on file
	fp = fopen(WORKER_SERVER_SOURCE,"w");

	printf("%ld iterations\n",nIterations);
	long int file_size=0;
	while(nIterations--)
	{
		n = recv(sd,&buf,sizeof(buf),0);
		fseek(fp,file_size,SEEK_SET);
		fwrite (buf,sizeof(char),n,fp);
		file_size+=n;
	}
	fflush(fp);
	printf("%ld file size=\n",file_size);
	
	int sizeArray=columnExtractor(WORKER_SERVER_SOURCE,1,&value);
	double resp=response(ch[0]-'0',value,sizeArray);
	//double resp=123578875578;
//	double resp=121278988.832322;
	resultBuf=toArray(resp);
	cout<<"Result buf="<<resultBuf<<endl;
	//send length
	//sendIterationCount(NULL,sd,response)
	send(sd,resultBuf,sizeof(resultBuf),0);
	close(sd);
	return 0;
}
