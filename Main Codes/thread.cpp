#include<stdio.h>
#include<pthread.h>
#include <unistd.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
using namespace std;
#define WORKERS 10
#define PORTNO 7000
#include "helper.h"

typedef struct p
{
	int partno;
	const char *filename;
	int split_size;
	char * ip;
	int port;
	int recv_value;
}parameters;

void *runner(void * param)
{
	parameters * p=(parameters *)param;
	string buffer = getPartition(p->filename,p->split_size,p->partno); 
	//send(b
	//recv
	cout<<buffer;
	pthread_exit(0);
}

void split(int split_size,const char *filename)
{
	
}

int main()
{
	int i;
	const char *filename="dataset.txt";
	parameters *param=(parameters *)malloc(WORKERS*sizeof(parameters));
	pthread_t *thread=(pthread_t *)malloc(WORKERS*sizeof(pthread_t));
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	for(i=0;i<WORKERS;i++)
	{
		param[i].partno=i+1;
		param[i].filename=filename;
		param[i].split_size=51300;
		param[i].port=PORTNO+i+1;
		pthread_create(&thread[i],&attr,runner,&param[i]);
		//pthread_join(&thread[i],NULL);
		sleep(2);//synchronize
	}return 0;
}
