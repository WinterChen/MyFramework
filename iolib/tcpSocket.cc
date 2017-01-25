#include "tcpSocket.h"
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include "log.h"
using namespace myframework; 

using namespace std;
#define MAXLISTENCOUNT 1024

int 
TcpSocket::Listen(int port, bool reuseport){
    mSockFD = socket(AF_INET,  SOCK_STREAM, 0);
	if(mSockFD < 0){
		MY_LOG_STREAM_ERROR << "create socket error:  " << errno << MY_LOG_EOM;
               return -errno;
	}
	int reuseAddr = 1;
	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	if (setsockopt(mSockFD, SOL_SOCKET, SO_REUSEADDR, (char *) &reuseAddr, sizeof(reuseAddr)) < 0) {
        MY_LOG_STREAM_ERROR << "set sockopt reuseaddr error  " << errno << MY_LOG_EOM;
    }
	if(reuseport){
		int opt_val = 1;
		if(setsockopt(mSockFD, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof(opt_val))){
			MY_LOG_STREAM_ERROR << "set reuseport error:  " << errno << MY_LOG_EOM;
			return -errno;
		}
	}
	int res = bind(mSockFD, (sockaddr *)&serverAddr, (sizeof(serverAddr)) );
	if(res < 0){
              MY_LOG_STREAM_ERROR << "bind error:  " << errno << MY_LOG_EOM;
		return -errno;
	}
	res = listen(mSockFD, MAXLISTENCOUNT);
	if(res < 0){
              MY_LOG_STREAM_ERROR << "listen error:  " << errno << MY_LOG_EOM;
		return -errno;
	}
	return 0;
}


void TcpSocket::SetupSocket(int bufSize)
{

    //int bufSize = 65536;//»º³åÇø´óÐ¡64k
    int flag = 1;

    // get big send/recv buffers and setup the socket for non-blocking I/O
    if (setsockopt(mSockFD, SOL_SOCKET, SO_SNDBUF, (char *) &bufSize, sizeof(bufSize)) < 0) {
        MY_LOG_STREAM_ERROR << "set SO_SNDBUF error " << errno <<  MY_LOG_EOM;
    }
    if (setsockopt(mSockFD, SOL_SOCKET, SO_RCVBUF, (char *) &bufSize, sizeof(bufSize)) < 0) {
        MY_LOG_STREAM_ERROR << "set SO_RCVBUF error " << errno <<  MY_LOG_EOM;
    }
    // enable keep alive so we can socket errors due to detect network partitions
    if (setsockopt(mSockFD, SOL_SOCKET, SO_KEEPALIVE, (char *) &flag, sizeof(flag)) < 0) {
        MY_LOG_STREAM_ERROR << "set SO_KEEPALIVE error " << errno <<  MY_LOG_EOM;
    }

    fcntl(mSockFD, F_SETFL, O_NONBLOCK);// set the socket noblocking
    // turn off NAGLE
    if (setsockopt(mSockFD, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(flag)) < 0) {
        MY_LOG_STREAM_ERROR << "set TCP_NODELAY error " << errno <<  MY_LOG_EOM;
    }

}

int 
TcpSocket::Connect(const ServerLocation & location){
    mSockFD = socket(AF_INET,  SOCK_STREAM, 0);
	if(mSockFD < 0){
		MY_LOG_STREAM_ERROR << "create socket error:  " << errno << MY_LOG_EOM;
               return -errno;
	}
	
	struct sockaddr_in clientAddr;
	bzero(&clientAddr, sizeof(clientAddr));
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = inet_addr(location.hostname.c_str());
	clientAddr.sin_port = htons(location.port);
	int res = connect(mSockFD, (sockaddr *)& clientAddr, sizeof(clientAddr) );
	if(res < 0){
        MY_LOG_STREAM_ERROR << "connect error:  " << errno << MY_LOG_EOM;
		return -errno;
	}
	return 0;
}

int
TcpSocket::Accept(ServerLocation &location){
	struct sockaddr_in clientAddr;
	bzero(&clientAddr, sizeof(clientAddr));
	socklen_t len = sizeof(clientAddr);
	int res = accept(mSockFD, (struct sockaddr *)&clientAddr, &len);
	
	if(res < 0){
              MY_LOG_STREAM_ERROR << "accept error:  " << errno << MY_LOG_EOM;
		return -errno;
	}
	location.hostname = inet_ntoa(clientAddr.sin_addr);
	location.port = ntohs(clientAddr.sin_port);
	
	MY_LOG_STREAM_DEBUG<< "location.hostname:  " << location.hostname << "location.port:  " << location.port << MY_LOG_EOM;
	return res;
}

ssize_t
TcpSocket::Write(const char *buf, size_t nbytes){
	if(mSockFD < 0){
		MY_LOG_STREAM_ERROR << "socket fd error!!!" << MY_LOG_EOM;
	}
	ssize_t res = write(mSockFD, buf, nbytes);
	if(res < nbytes)
		MY_LOG_STREAM_ERROR << "write error: " << errno << MY_LOG_EOM;
	return res;
}

ssize_t
TcpSocket::Read(char *buf, size_t nbytes){
	if(mSockFD < 0){
		MY_LOG_STREAM_ERROR << "socket fd error!!!" << MY_LOG_EOM;
	}
	
	ssize_t res = read(mSockFD, buf, nbytes);
	if(res == 0)
		MY_LOG_STREAM_ERROR << "read completed or the fd is closed!!!" << MY_LOG_EOM;
	return res;
}

void
TcpSocket::Close(){
	if(mSockFD > 0)
		close(mSockFD);
	mSockFD = -1;
	
}

