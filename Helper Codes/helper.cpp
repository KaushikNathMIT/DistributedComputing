#include<bits/stdc++.h>
using namespace std;

class Helper{
	void columnExtractor(char[] filePath, int columnNumber)
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
}
