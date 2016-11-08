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
#include "helper.h"

using namespace std;



int main()
{
	int sd,ls;
	socklen_t clen;
	char buf[BUF_SIZE];
	char ch[1];
	char *t;
	FILE *fp;
	struct sockaddr_in sadd,cadd;
	
	//create server socket
	ls=socket(AF_INET, SOCK_STREAM,0);
	sadd.sin_family=AF_INET;
	sadd.sin_addr.s_addr=INADDR_ANY;
	sadd.sin_port=htons(MAIN_SERVER_PORT);
	
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
			
			recv(sd,&ch,sizeof(ch),0); //receve choice entered by user
			cout<<"Choice entered - "<<ch[0]<<endl;
			
			fp=fopen(MAIN_SERVER_SOURCE,"w");
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
			char* iplist[WORKERS];
			getIpList(iplist);
			WorkerParams workerinfo[WORKERS];			
			for(int partno=0;partno<WORKERS;partno++)
			{  
				workerinfo[partno].split_size=split_size;
				workerinfo[partno].partno=partno+1;
				workerinfo[partno].ip=iplist[0];
				workerinfo[partno].ch[0]=ch[0];
				pthread_create(&thread[partno],&attr,startWorker,&workerinfo[partno]);
			}
			
			for (int i=0;i<WORKERS;i++)
       			pthread_join(thread[i], NULL);
       			
       		//answer is contained workerinfo->result  of all structs
       		
       			
			
			close(sd);
			exit(0);
		}
		close(sd);
	}
	return 0;
}
