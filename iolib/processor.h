#include <iostream>
#include <string>
#include <queue>
#include "netKicker.h"
#include "threadPool.h"
#include "mutex.h"
#include "netConnection.h"
#include "counter.h"
#include "fdEpoll.h"
#include "log.h"
#include "classBase.h"
#include "acceptor.h"
using namespace myframework;
using namespace std;



class Processor: public Runnable, public ProcessorBase {
public:
	Processor(int index, IAcceptorOwner* owner = NULL):mIndex(index), mThread(NULL), mUsing(0), mFilling(1), mRunFlag(1){
		mThread = new ThreadPool("processor", 1, this);
		int res = mBufferPool.Creat(4096*4096, 4096);
		if(res){
			cout << "creat buffer pool fail, errno: " << res << endl;
		}
		if(owner == NULL){
			cout << "no listened port for processor " << endl;
		}
		if(owner->GetPort()){
			cout << "processor " << index << " start listen " << owner->GetPort() << endl;
			mListener = new Acceptor(owner->GetPort(), owner, this);
		}
		
	}
	~Processor(){}
	void Run(){MainLoop();}
	void Start(){
		if(mThread)
			mThread->Start();
		else
			MY_LOG_STREAM_ERROR << "thread is null!!!" << MY_LOG_EOM;

	}
	void MainLoop();
	int RecliamNetConnection(NetConnection *conn);
	//void Init(ServerLocation &server, int index, string taskName);
	void AddConnectionToEpoll(NetConnection *conn);
	int DeleteFromEpoll(NetConnection *conn);
	void ModConnectionToEpoll(NetConnection * conn, int eventType);
	void AddToPendingConnectionList(NetConnection * conn);
	//void SetSocket(TcpSocket *sock){mSocket = sock;}
	//int GetSocketFd(){return mSocket != NULL ? mSocket->GetSockFd():-1;}
	Mutex *GetMutex(){return &mMutex;}
public:
	Counter mCounter;
private:
	//string mThreadName;
	int mIndex;
	ThreadPool *mThread;
	FDEpoll mEpoll;
	BufferPool mBufferPool;
	queue<NetConnection *> mPendingConnectionList[2];//add for the connections will add to epoll
    int mUsing;
    int mFilling;
	bool mRunFlag;
	Mutex mMutex;
	NetKicker mKicker;
	//add by winter, 20160621
	Acceptor *mListener;
	
};
