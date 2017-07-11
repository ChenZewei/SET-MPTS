#ifndef NETWORK_H
#define NETWORK_H

//Socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "types.h"

#define MAX_BUFFER 1024

#define CONNECTED		0
#define DISCONNECTED	1

class NetWork
{
	private:
		int connectfd;
		struct sockaddr_in addr;
		int status;
	public:
		NetWork(int socket, struct sockaddr_in addr);
		NetWork(int socket, string ip, long port);
		~NetWork();
		int get_socket();
		int get_status();
		string recv();
		int send(string buffer);
		
};

NetWork::NetWork(int socketfd, struct sockaddr_in addr)
{
	connectfd = socketfd;
	this->addr = addr;
	status = CONNECTED;
}

NetWork::NetWork(int socketfd, string ip, long port)
{
	connectfd = socketfd;
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	status = CONNECTED;
}

NetWork::~NetWork() {}

int NetWork::get_socket()
{
	return connectfd;
}

int NetWork::get_status()
{
	return status;
}

string NetWork::recv()
{
	char recvbuf[MAX_BUFFER];

	if(recv(connectfd, recvbuf, sizeof(recvbuf), 0) == 0)
	{
		status = DISCONNECTED;
		return "";
	}

	return recvbuf;
}

int NetWork::send(string buffer)
{
	return send(connectfd, buffer.c_str(), strlen(buffer.c_str()), 0);
}


#endif
