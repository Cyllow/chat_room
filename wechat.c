#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

extern int errno;

int errexit(const char *format,...);
int connectTCP(const char *host,const char *service);
int send_info(int fd);

#define LINELEN 128

char name[15];

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
	pthread_t th;
	pthread_attr_t ta;
	
	char buf[LINELEN+1];
	int s,n;
	
	printf("please input your name:");
	fgets(name,sizeof(name),stdin);
	printf("your login name is %s\n\n\n",name);
	
	(void)pthread_attr_init(&ta);
	(void)pthread_attr_setdetachstate(&ta,PTHREAD_CREATE_DETACHED);
	
	s = connectTCP(host,service);
	
	if(pthread_create(&th,&ta,(void * (*)(void *))send_info,(void *)s) < 0)
			errexit("pthread_create:%s\n",strerror(errno));
	
	while(1)
	{
		memset(&buf,'\0',sizeof(buf));
		n = read(s,buf,sizeof(buf));
		if(n > 0)
			fputs(buf,stdout);
	}
	close(s);
}

int send_info(int fd)
{
	char buf[LINELEN+1];
	char sendbuf[LINELEN+1+15];
	
	while(1)
	{
		strcpy(sendbuf,"From:");
		strcat(sendbuf,name);
		fgets(buf,sizeof(buf),stdin);
		strcat(sendbuf,buf);
		strcat(sendbuf,"\n");
		write(fd,sendbuf,strlen(sendbuf));
	}
}