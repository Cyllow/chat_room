#define _USE_BSD
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define QLEN 32
#define BUFSIZE 4096

extern int errno;
	
int sockarrt[2];
int socknum = 0;

int TCPechod(int fd);
int errexit(const char *format,...);
int passiveTCP(const char *service,int qlen);


int main(int argc,char *argv[])
{
	pthread_t th;
	pthread_attr_t ta;
	
	char *service = "echo";
	struct sockaddr_in fsin;
	unsigned int alen;
	int msock;
	int ssock;
	switch(argc)
	{
	case 1:
		break;
	case 2:
		service = argv[1];
		break;
	default:
		errexit("usage:TCPechod [port]\n");
	}
	
	msock = passiveTCP(service,QLEN);
	
	(void)pthread_attr_init(&ta);
	(void)pthread_attr_setdetachstate(&ta,PTHREAD_CREATE_DETACHED);
	
	while(1)
	{
		alen = sizeof(fsin);
		ssock = accept(msock,(struct sockaddr *)&fsin,&alen);
		if(ssock < 0)
		{
			if(errno == EINTR)
				continue;
			errexit("accept:%s\n",strerror(errno));
		}
		if(socknum == 0)
		{
			sockarrt[0] = ssock;
			socknum++;
		}
		else if(socknum == 1)
		{
			sockarrt[1] = ssock;
			socknum++;
		}
		if(pthread_create(&th,&ta,(void * (*)(void *))TCPechod,(void *)ssock) < 0)
			errexit("pthread_create:%s\n",strerror(errno));
	}
}




int TCPechod(int fd)
{
	char buf[BUFSIZE];
	int cc;
	
	while(cc = read(fd,buf,sizeof(buf)))
	{
		if(cc < 0)
			errexit("echo read:%s\n",strerror(errno));
		if(sockarrt[0] == fd && write(sockarrt[1],buf,cc) < 0)
			errexit("echo write:%s\n",strerror(errno));
		else if(sockarrt[1] == fd && write(sockarrt[0],buf,cc) < 0)
			errexit("echo write:%s\n",strerror(errno));
	}
}