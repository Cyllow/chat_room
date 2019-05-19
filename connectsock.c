#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

extern int errno;

int errexit(const char *format,...);


int connectsock(const char *host,const char *service,const char *transport)
/*
*	host:地址信息
*	service:端口信息
*	transport:TCP OR UPD
*/
{
	struct hostent *phe;//gethostbyname(域名->IP)返回此结构体，在h_addr里有主机IP地址
	struct servent *pse;//getservbyname返回此结构体，端口号存在s_port
	struct protoent *ppe;//getprotobyname返回此结构体，协议协议号存在p_proto
	struct sockaddr_in sin;//tcp/udp专用地址结构
	int s,type;
	
	memset(&sin,0,sizeof(sin));//初始化地址结构全为0
	sin.sin_family = AF_INET;
	
	if(pse = getservbyname(service,transport))
		sin.sin_port = pse->s_port;
	else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
		errexit("can't get \"%s\" service entry\n",service);
	
	
	if(phe = gethostbyname(host))
		memcpy(&sin.sin_addr,phe->h_addr,phe->h_length);
	else if((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
		errexit("can't get \"%s\" protocol entry\n",host);
	
	
	if(strcmp(transport,"udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;
	
	
	s = socket(PF_INET,type,0);
	if(s < 0)
		errexit("can't create socket:%s\n",strerror(errno));
	
	
	if(connect(s,(struct sockaddr *)&sin,sizeof(sin)) < 0)
		errexit("can't connect to %s.%s:%s\n",host,service,strerror(errno));
	
	
	return s;
}