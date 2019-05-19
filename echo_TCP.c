#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>

extern int errno;

int TCPecho(const char *host,const char *service);
int errexit(const char *format,...);
int connectTCP(const char *host,const char *service);

#define LINELEN 128

int main(int argc,char *argv[])
{
	char *host="localhost";
	char *service="echo";
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
		fprintf(stderr,"usage:TCPecho[host[port]]\n");
		exit(1);
	}
	TCPecho(host,service);
	exit(0);
}
int TCPecho(const char *host,const char *service)
{
	char buf[LINELEN+1];
	int s,n;
	int outchars,inchars;
	
	s = connectTCP(host,service);
	
	while(fgets(buf,sizeof(buf),stdin))
	{
		buf[LINELEN] = '\0';
		outchars = strlen(buf);
		(void)write(s,buf,outchars);
	
		for(inchars = 0;inchars < outchars;inchars += n)
		{
			n = read(s,&buf[inchars],outchars - inchars);
			if(n < 0)
				errexit("socket read failed: %s\n",strerror(errno));
		}
		fputs(buf,stdout);
	}
}
