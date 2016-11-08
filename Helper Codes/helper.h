#ifndef helper
#define helper
#include <bits/stdc++.h>
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
using namespace std;
#define WORKERS 2
#define BUF_SIZE 8

struct WorkerParams  //we will send a pointer to structure while creating thread
{
	int split_size;
	int partno;
	int portno;
	
};


int columnExtractor(const char* filePath, int columnNumber,int *values)
{
	char c;
	values = new int[100000];
	int k=0;
	ifstream in;
	in.open(filePath);
	string line;
	if(!in)
	{
		cout<<"File Does not Exist";
		return 0;
	}
	while(getline(in, line))
    {
		int count =0;
		int j=0;
		int temp =0;
		for(string::iterator it = line.begin(); it!= line.end(); it++){
			if(*it == 44) count++;
			if(count == columnNumber){
				if(*it >= '0' && *it <= '9') {
					temp*=10;
					temp += *it - '0';					
				}	
			}
		}
		//cout<<temp<<"\n";
		values[k++] = temp;
	}
	for(int i=0;i<k;i++){
		cout<<values[i]<<"\n";
	}
	return k;
}


string getPartition(const char* filename,int split_size,int partno)
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
	i=0;
	string split="";
	while(c)
	{
	   	split+=fgetc(j);
		//printf("%c",fgetc(j));
		c--;
		fseek(j,0,SEEK_CUR);  //* j increments automatically
	}
	split+='\n';
	return split;
}
	
int getFileSize(FILE *fp)
{
	fseek(fp,0,SEEK_END);
	return ftell(fp);
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

char * toArray(long int number)
{
        long int n = log10(number) + 1;
        long int i;
        char *numberArray = (char*)calloc(n, sizeof(char));
        for (i = 0; i < n; ++i, number /= 10 )
        {
            numberArray[i] = (number % 10)+'0';   //sending number in reverse, in character form
        }
        numberArray[i]='\0';
        return numberArray;

}

void sendIterationCount(FILE *fp,int sd,string partition)
{
	char *arr=NULL;
	long int size;
	if(fp!=NULL) 
	{
		fseek(fp,0,SEEK_END);
		size = ftell(fp);   //total no. of bytes in file	
	}  
	else //for string
	{
		size = partition.length();
	}
	int rem=size%BUF_SIZE;
	if(rem!=0)
		size=(size/BUF_SIZE)+1;
	else
		size=(size/BUF_SIZE);
	cout<<"value - "<<size<<endl;
	arr=toArray(size);
	send(sd,arr,sizeof(arr),0);
}



#endif
