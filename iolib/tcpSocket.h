#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <iostream>
using namespace std;
struct ServerLocation{
	ServerLocation() : hostname(""), port(-1){}
	ServerLocation(string name, int p) : hostname(name), port(p){}
	ServerLocation(ServerLocation &other) : hostname(other.hostname), port(other.port){}
	ServerLocation &operator=(ServerLocation &other){
		hostname = other.hostname;
		port = other.port;
		return *this;
	}
	string hostname;
	int port;
};

//
class TcpSocket{
public:
	TcpSocket():mSockFD(-1){}
	TcpSocket(int fd):mSockFD(fd){}
	~TcpSocket(){Close();}
	int Connect(const ServerLocation &location);
	int GetSockFd(){return mSockFD;}
	int Bind(const ServerLocation &location);
	void SetupSocket(int bufSize = 65536);
	//create socket, bind, and listen
	
	int Listen(int port, bool reuseport = false);
	int Accept(ServerLocation &location);
	ssize_t Write(const char *buf, size_t nbytes);
	ssize_t Read(char *buf, size_t nbytes);
	bool IsGood(){return mSockFD > 0 ? true : false;}
	void Close();
private:
	int mSockFD;
};

#endif
