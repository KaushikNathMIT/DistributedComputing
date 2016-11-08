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



long response(int query,int *array,int sizeArray,string str[])
{

	int i;
	char *g;
	long sum=0;
	cout<<sizeArray<<endl;
	if(array)
	{	
		if(query==1||query==2)
		{
			for(i=0;i<sizeArray;i++)
			{	
				sum+=(long)array[i];
			}
		}
		return sum;
	}
}
		
int main()
{
	int sd,ls;
	socklen_t clen;
	char buf[8];
	char ch[1];
	char *resultBuf;
	char *t;
	int *value=NULL;
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
	
	int sizeArray=columnExtractor(WORKER_SERVER_SOURCE,3,&value);
	double resp=response(ch[0]-'0',value,sizeArray,str);
	cout<<resp<<endl;
	resp=123.47548594;
	resultBuf=toArray(resp);
	cout<<resultBuf<<endl;
	send(sd,resultBuf,sizeof(resultBuf),0);
	close(sd);
	return 0;
}
