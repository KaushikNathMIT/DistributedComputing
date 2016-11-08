#include "helper.h"
#include <bits/stdc++.h>

int main(){

	socklen_t clen;
	char buf[BUF_SIZE];
	char ch[1];
	char *t;

	int ls=socket(AF_INET, SOCK_STREAM,0);
	sadd.sin_family=AF_INET;
	sadd.sin_addr.s_addr=INADDR_ANY;
	sadd.sin_port=htons(MAIN_SERVER_PORT);
	bind(ls,(struct sockaddr *)&sadd, sizeof(sadd));
	listen(ls,5);
	clen=sizeof(cadd);
	int sd=accept(ls,(struct sockaddr *)&cadd,&clen);

	columnExtractor("dataset.txt", 3);
	char *number = toArray(123.45);
	*number=toArray(123);
	char iplist[WORKERS];getIpList(iplist);
	string part = getPartition("dataset.txt",500,1);
	int fsize(fopen("dataset.txt","r"));
	char buf[]={'1','2','3','4','\0'};int itCount = getIterationCount(buf);
	sendIterationCount(fopen("dataset.txt","r"),sd,"");
	readSend(fp,sd);
	
	return 0;
}
