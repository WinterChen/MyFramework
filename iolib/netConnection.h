#ifndef _NET_CONNECTION_
#include "tcpSocket.h"
#include "callbackObj.h"
#include "ioBuffer.h"
#include "threadPool.h"
#include "bufferPool.h"

//#include "classBase.h"

class Processor;
class NetConnection{
public:
	NetConnection(TcpSocket *sock, CallbackObj *c,  bool listenOnly = false):mSock(sock), mCallbackObj(c), mListenOnly(listenOnly){}
	~NetConnection(){}
	int HandleEvent(int op);
	void HandleReadEvent();
	void HandleNewConnection();
	void HandleWriteEvent();
	void HandleErrorEvent();
	void HandleCloseEvent();
	bool IsListen(){return mListenOnly;}
	void HandleTimeoutEvent();
	int SetBufferPool(BufferPool *bp){return mBuffer.SetBufferPool(bp);}
	void SetCallbackObj(CallbackObj *callbackObj){mCallbackObj = callbackObj;}
	void SetProcessor(Processor *processor);
	bool IsGood() const {
        return (mSock && mSock->IsGood());
    }
	//int Write(int len);
	int Write(char *buf, int len);
	void Close(){
		if(mSock){
			delete mSock;
		}
		if(mCallbackObj)
			delete mCallbackObj;
		mProcessor = NULL;

	}
	//char* GetOutBuffer(){return mOutBuffer.mBuffer+mOutBuffer.mDataLen;}
    int GetSockFD() const{
		return (IsGood() ? mSock->GetSockFd(): -1);
	}

	string GetClientIp(){
		return mClient.hostname;
	}
	void SetClient(ServerLocation &client){
		mClient = client;
	}
private:
	TcpSocket *mSock;
	bool mListenOnly;
	CallbackObj *mCallbackObj;
	//IOBuffer mInBuffer;
	//IOBuffer mOutBuffer;
	IOBuffer mBuffer;
	ServerLocation mClient;
	//int mProcessorIndex;
	Processor *mProcessor;
};
#define _NET_CONNECTION_
#endif
