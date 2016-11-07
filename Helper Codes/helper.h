#ifndef helper
#define helper
#include <bits/stdc++.h>
using namespace std;

int columnExtractor(char* filePath, int columnNumber)
{
	char c;
	int arr[100000];
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
		arr[k++] = temp;
	}
	for(int i=0;i<k;i++){
		cout<<arr[i]<<"\n";
	}
	return 0;
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
	
int getFileSize(FILE *fp)
{
	fseek(fp,0,SEEK_END);
	return ftell(fp);
}



#endif
