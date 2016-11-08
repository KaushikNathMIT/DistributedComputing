#ifndef helper
#define helper
#include <bits/stdc++.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>
#define WORKERS 1
#define BUF_SIZE 8
#define MAIN_SERVER_PORT 6998
#define WORKER_SERVER_PORT 7810
#define MAIN_SERVER_SOURCE "dataset_main.txt"
#define CLIENT_SOURCE "dataset.txt"
#define WORKER_SERVER_SOURCE "dataset_worker.txt"
using namespace std;

struct WorkerParams  //we will send a pointer to this structure while creating thread
{
	int split_size;
	int partno;
	int portno;
	const char* ip;
	double result;
	char ch[1];
};

void getIpList(char* iplist[])
{
	iplist[0]="127.0.0.1";
}




int columnExtractor(const char* filePath, int columnNumber,double **values)
{
	char c;
	*values = (double *)malloc(100000*sizeof(double));
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
 		string temp="";
		int count =0;
		int j=0;
		for(string::iterator it = line.begin(); it!= line.end(); it++){
			if(*it == ',') count++;
			if(count == columnNumber){
				if((*it >= '0' && *it <= '9') || *it=='.') {
					temp+=*it;
				}	
			}
		}
		if(temp.compare("")!=0)
		{
			try
			{
				*(*values+k) = stod(temp);
				k++;
			}
			catch (const std::invalid_argument&) 
			{
				cout<<"here"<<endl;
				throw;
			} 
			catch (const std::out_of_range&) 
			{
				cout<<"here"<<endl;
				throw;
    		}
    	}
    	
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

char * toArray(double number)
{
	cout<<number<<endl;
	char *tmp=(char *)malloc(100*sizeof(char));
	sprintf(tmp, "%.2e", number);
	for(int i=0;tmp[i]!='\0';i++)
	{
		cout<<tmp[i]<<" ";
	}
	cout<<endl;
	//ostringstream strs;
	//string str = strs.str();
	//strncpy(tmp,str.c_str(),sizeof(tmp));
	//tmp[sizeof(tmp)-1]=0;
	return tmp;
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

//client
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

//main server

void *startWorker(void * param)
{
	cout<<"thread started\n";
	WorkerParams * p=(WorkerParams *)param;
	string partition = getPartition(MAIN_SERVER_SOURCE,p->split_size,p->partno); 
	cout<<"Parititon="<<partition<<endl;
	cout<<"partition in thread done\n";
	//create client socket to send data to worker server
	int sd;
	struct sockaddr_in addrs;
	sd=socket(AF_INET, SOCK_STREAM,0);
	addrs.sin_family=AF_INET;
	addrs.sin_port=htons(WORKER_SERVER_PORT);
	addrs.sin_addr.s_addr=inet_addr(p->ip);   //worker server ip address
	
	int len=sizeof(addrs);
	
	int result=connect(sd,(struct sockaddr *)&addrs,len);


	sendIterationCount(NULL,sd,partition);	
	
	send(sd,p->ch,sizeof(p->ch),0); //send choice to worker
	
	//sending in chunks of 8 bytes  
	cout<<partition.length()<<endl;
	char buf[BUF_SIZE];
	long int k=0,c=0;
	for(long int i=0;i<partition.length()/BUF_SIZE;i++)
	{
		for(int j=0;j<BUF_SIZE;j++)
		{
			buf[j]=partition[k];
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
	
	
	//receive result from individual threads in character array
	char localResult[100];
	recv(sd,&localResult,sizeof(localResult),0);
	printf("in char array %s",localResult);
	//get double value from char array
	p->result=strtod(localResult,NULL);
	cout<<"in double="<<p->result<<endl;
	
	pthread_exit(0);
}

double avgColumn(double* array,int sizeArray)
{
	double sum=0.0;
	for(int i=0;i<sizeArray;i++)
	{
		sum+=array[i];
	}
	return sum/sizeArray;
}

double addColumn(double* array,int sizeArray)
{
	double sum=0.0;
	for(int i=0;i<sizeArray;i++)
	{
		sum+=array[i];
	}
	return sum;
}


//worker
double response(int query,double *array,int sizeArray)
{

	switch(query)
	{
		case 1:
				return addColumn(array,sizeArray);
		case 2:
				return avgColumn(array,sizeArray);
	}
	
	
}




#endif
