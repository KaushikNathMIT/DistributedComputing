#include<iostream>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<time.h>
#define BUF_SIZE 8
#define PORTNO 6969
#define FILENAME "dataset_main_server.txt"
#include "helper.h"

using namespace std;


void *startWorker(void * param)
{
	cout<<"thread started\n";
	WorkerParams * p=(WorkerParams *)param;
	string partition = getPartition(FILENAME,p->split_size,p->partno); 
	cout<<"partition in thread done\n";
	
	//create client socket to send data to worker server
	int sd;
	struct sockaddr_in addrs;
	sd=socket(AF_INET, SOCK_STREAM,0);
	addrs.sin_family=AF_INET;
	
	if(p->partno==1)
	{
		addrs.sin_addr.s_addr=inet_addr("192.168.0.102");
		addrs.sin_port=htons(7500);
		    //worker server ip address
	}
	else
	{
		addrs.sin_addr.s_addr=inet_addr("127.0.0.1");    //worker server ip address
		addrs.sin_port=htons(7500);
	}

	int len=sizeof(addrs);
	
	int result=connect(sd,(struct sockaddr *)&addrs,len);


	sendIterationCount(NULL,sd,partition);	
	
	//sending in chunks of 8 bytes  
	cout<<partition.length()<<endl;
	char buf[BUF_SIZE];
	long int k=0,c=0;
	for(long int i=0;i<partition.length()/BUF_SIZE;i++)
	{
		for(int j=0;j<BUF_SIZE;j++)
		{
			//cout<<partition[k];
			buf[j]=partition[k];
//			cout<<buf[j];
			k++;
		}
		c++;
		send(sd,buf,sizeof(buf),0);
	}
	cout<<c<<endl;
	int leftover = partition.length()%BUF_SIZE; 
	for(int i=0;i<leftover;i++)
	{
		buf[i]=partition[k++];
	}
	cout<<"\n"<<send(sd,buf,leftover,0);
	cout<<"Sent";
	
	pthread_exit(0);
}


int main()
{
	int sd,ls;
	socklen_t clen;
	char buf[BUF_SIZE];
	char *t;
	FILE *fp;
	struct sockaddr_in sadd,cadd;
	
	//create server socket
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
			int nbytes=recv(sd,&buf,sizeof(buf),0);  //getting file size
			long int nIterations = getIterationCount(buf); //receive number of bytes and buffer,return the no. iteration 
			fp=fopen(FILENAME,"w");
			int n;
			while(nIterations--)
			{
			
				n = recv(sd,&buf,sizeof(buf),0);;
				fseek(fp,file_size,SEEK_SET);	
				fwrite (buf,sizeof(char),n,fp);
				file_size+=n;
			}
			
			cout<<"File Received\n";
			//now we have received the file, 
			file_size = getFileSize(fp);  
			fclose(fp);
			int split_size= file_size/WORKERS; //in bytes
			
			//thread related
			pthread_t *thread=(pthread_t *)malloc(WORKERS*sizeof(pthread_t));  //don't know why
			pthread_attr_t attr;
			pthread_attr_init(&attr);

			cout<<"Creating Threads\n";
			WorkerParams workerinfo[WORKERS];			
			for(int partno=1;partno<=WORKERS;partno++)
			{  
				workerinfo[partno-1].split_size=split_size;
				workerinfo[partno-1].partno=partno;
				pthread_create(&thread[partno-1],&attr,startWorker,&workerinfo[partno-1]);
			 //pthread_join(thread[partno-1], NULL);
			}
			
			//sleep(10);
			for (int i=0;i<WORKERS;i++)
       			pthread_join(thread[i], NULL);
			
			close(sd);
			exit(0);
		}
		close(sd);
	}
	return 0;
}
