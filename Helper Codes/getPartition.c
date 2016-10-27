#include<stdio.h>
#include<stdlib.h>
#define WORKERS 1000

int getFileSize(FILE *fp)
{
	fseek(fp,0,SEEK_END);
	return ftell(fp);
}

char* getPartition(const char* filename,int split_size,int partno)
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
	while(c)
	{
		printf("%c",fgetc(j));
		c--;
		fseek(j,0,SEEK_CUR);  //* j increments automatically
	}
	
	return NULL;
}

int main()
{
	int i;
	for( i=1;i<=WORKERS;i++)
	{
		getPartition("access_log",504941,i);
		printf("\n");
	}
	return 0;
}
