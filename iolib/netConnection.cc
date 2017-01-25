#include "netConnection.h"

#include "type.h"
#include <sys/epoll.h>
#include <iostream>
#include <errno.h>
#include "log.h"
#include "processor.h"
using namespace myframework; 

using namespace std;

void NetConnection::SetProcessor(Processor *processor){
	mProcessor = processor;
}

int NetConnection::HandleEvent(int op){
	int res = 0;
	if(IsGood()){
		if(IsListen()){
			MY_LOG_STREAM_DEBUG << "handle new connection, op: " << op << MY_LOG_EOM;
			HandleNewConnection();
			return EVENT_NEW_CONNECTION;
		}
		if ((op & EPOLLIN) != 0) {
			
			MY_LOG_STREAM_DEBUG << "handle read event, op: " << op << MY_LOG_EOM;
        	HandleReadEvent();
			res = res | EPOLLIN;
    	}
    	if ((op & EPOLLOUT) != 0) {
			
			MY_LOG_STREAM_DEBUG << "handle write event, op: " << op << MY_LOG_EOM;
        	HandleWriteEvent();
			
    	}
    	if ((op & (EPOLLERR | EPOLLHUP)) != 0) {
			
			MY_LOG_STREAM_DEBUG << "handle error event, op: " << op << MY_LOG_EOM;
        	HandleErrorEvent();
		}
    }
	else {
		
		MY_LOG_STREAM_DEBUG << "handle close event, op: " << op << MY_LOG_EOM;
		HandleCloseEvent();
		return EVENT_NET_CLOSE;
	}

}
void NetConnection::HandleNewConnection(){

	if (! IsGood()) {
        MY_LOG_STREAM_INFO << "Read event ignored: socket closed!" << MY_LOG_EOM;
    } 
	else if (mListenOnly) {
		int fd = mSock->Accept(mClient);
		if(fd < 0){
			MY_LOG_STREAM_ERROR << "accept failure, create new socket error: " << fd << MY_LOG_EOM;
			return;
		}
        TcpSocket* sock = new TcpSocket(fd);
		sock->SetupSocket();
		NetConnection *connect = new NetConnection(sock, NULL);
		connect->SetClient(mClient);
		MY_LOG_STREAM_DEBUG << "############new socket: " << fd << MY_LOG_EOM;
        if (connect == NULL) {
            MY_LOG_STREAM_ERROR << "# new NetConnection failure, maybe there is no enought memery." << MY_LOG_EOM;
            return;
        }
		//callback is Acceptor
		mCallbackObj->HandleEvent(EVENT_NEW_CONNECTION, (void *)connect);//(void *) &conn);
    }

}


void NetConnection::HandleReadEvent(){
	ssize_t res = mBuffer.Read(mSock);
	if(res > 0){
		mCallbackObj->HandleEvent(EVENT_NET_READ,(void *)&mBuffer);
		/*if(res < BUFFERLEN){
			//TODO: parse and process request
			int bodyLen = 0;
			if(IsMsgAvail(mInBuffer, bodyLen)){
				ParseAndProcessRequest(mInBuffer, bodyLen);
			}
			else{
				cout << "the content of the request msg error, close the client socket!" << endl;
			}
		}*/
		
		
	}
	else if(res == 0){
		HandleCloseEvent();
	}
	else{
		if(errno != EAGAIN && errno != EINTR)
			HandleErrorEvent();
	}
}

void NetConnection::HandleWriteEvent(){
	if(mBuffer.OutBufferEmpty()){
		//no data in out buffer, so do not need listen write event
		mProcessor->ModConnectionToEpoll(this, EPOLLIN);
		return;
	}
	int res = mBuffer.Write(mSock);
	if(res == 0){
		MY_LOG_STREAM_DEBUG << "all data writed to the socket" << MY_LOG_EOM;
		mProcessor->ModConnectionToEpoll(this, EPOLLIN);
		return;
	}
	if(res < 0){
		MY_LOG_STREAM_ERROR << "write error: " << errno << MY_LOG_EOM;
		return;
	}	
	
}


void NetConnection::HandleCloseEvent(){
	
	MY_LOG_STREAM_DEBUG << "delete from epoll and close the socket: " << MY_LOG_EOM;
	if(mSock){
		mProcessor->DeleteFromEpoll(this);
		mSock->Close();
		//mProcessor->RecliamNetConnection(this);
	}
	
}

void NetConnection::HandleErrorEvent(){
	
	MY_LOG_STREAM_ERROR << "error net event, close the socket " << MY_LOG_EOM;
	HandleCloseEvent();
}
/*
int NetConnection::Write(int len){
	int res = mBuffer.Write(mSock, len);
}
*/

int NetConnection::Write(char *buf, int len){
	if(mSock == NULL){
		//maybe never come here
		MY_LOG_STREAM_ERROR << "no socekt, write error! " << MY_LOG_EOM;
		return -1;
	}
	int res = mSock->Write(buf, len);
	if(res == len)
		return res;
	if(res < 0 && errno != EINTR  && errno != EAGAIN){
		
		MY_LOG_STREAM_ERROR << "write error, not EINTR and EAGAIN! errno:" << errno << MY_LOG_EOM;
		HandleErrorEvent();
	}
	else if(res < 0)
	{
		MY_LOG_STREAM_INFO << "write error, EINTR or EAGAIN! errno:" << errno << MY_LOG_EOM;
		//append to buf,
		mBuffer.Append(buf, len);
		// add to epoll and  wait for writing out
		mProcessor->ModConnectionToEpoll(this, EPOLLIN|EPOLLOUT);
	}
	else{
		MY_LOG_STREAM_INFO << "need write " << len << ", but " << res  << ". errno: " << errno << MY_LOG_EOM;
		//append to buf,
		mBuffer.Append(buf + res, len - res);
		// add to epoll and  wait for writing out
		mProcessor->ModConnectionToEpoll(this, EPOLLIN|EPOLLOUT);
	}
}

