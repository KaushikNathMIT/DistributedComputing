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
using namespace std;
#include "helper.h"

int main()
{
	int sd,i;
	char result[1000];
	struct sockaddr_in addrs;
	sd=socket(AF_INET, SOCK_STREAM,0);
	addrs.sin_family=AF_INET;
	addrs.sin_addr.s_addr=inet_addr("127.0.0.1");    //main server ip address
	addrs.sin_port=htons(MAIN_SERVER_PORT);
	int len=sizeof(addrs);
	connect(sd,(struct sockaddr *)&addrs,len);
	
	//open file
	FILE *fp = fopen(CLIENT_SOURCE,"r");
	
	//send size of file
	sendIterationCount(fp,sd,"");
	
	//send choice 
	char ch[1];
	cout<<"1-Addition\n2-Average\nEnter choice\n";
	cin>>ch[0];
	send(sd,ch,sizeof(ch),0);
	
	//read a file, 8 bytes at a time then send them one by one
	readSend(fp,sd);
	recv(sd,&result,sizeof(result),0);
	
	result[8]='\0';
	printf("%s",result);
	//getting timestamp in return
	//int n = recv(sd,&buf,sizeof(buf),0);
//	printf("The server time is\n");
	//printString(buf,n);
	//printf("%d\n",n);
	//for(i=0;i<n;i++)
	//	printf("%c",buf[i]);
	return 0;
}
