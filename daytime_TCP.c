#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern int errno;

int TCPdaytime(const char *host,const char *service);
int errexit(const char *format,...);
int connectTCP(const char *host,const char *service);

#define LINELEN 128

int main(int argc,char *argv[])
{
	char *host = "localhost";
	char *service = "daytime";
	
	switch(argc)
	{
	case 1:
		host = "localhost";
		break;
	case 3:
		service = argv[2];
	case 2:
		host = argv[1];
		break;
	default:
		fprintf(stderr,"usage:TCPdaytime[host[port]]\n");
		exit(1);
	}
	TCPdaytime(host,service);
	exit(0);
}


int TCPdaytime(const char *host,const char *service)
{
	char buf[LINELEN+1];
	int s,n;
	
	s = connectTCP(host,service);
	
	while((n = read(s,buf,LINELEN)) > 0)
	{
		buf[n] = '\0';
		(void)fputs(buf,stdout);
	}
}