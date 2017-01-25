#include "processor.h"
#include <errno.h>
#include <string.h>
#include "log.h"
#include "type.h"
#include "cpu.h"
using namespace myframework; 
#define MAXEVENTCOUNT 10000
#define EPOLLTIMEOUT 2000000
void Processor::AddConnectionToEpoll(NetConnection * conn){
	MY_LOG_STREAM_DEBUG << "processor [" << mIndex << "], " << "add to epoll  " << MY_LOG_EOM;
	if(conn == NULL){
		MY_LOG_STREAM_INFO << "conn is null" << MY_LOG_EOM;
		return;
	}
	unsigned eventType = EPOLLIN;//¶Á
	int res = mEpoll.Add(conn->GetSockFD(), conn, eventType);
	if(res){
		MY_LOG_STREAM_ERROR << "processor [" << mIndex << "], " << "add to epoll error:  " << strerror(errno) << MY_LOG_EOM;
	}
	res = conn->SetBufferPool(&mBufferPool);
	if(res == -1){
		MY_LOG_STREAM_ERROR << "processor [" << mIndex << "], " << "buffer pool is NULL, why???  " << MY_LOG_EOM;
	}
	conn->SetProcessor(this);
	//TODO:add to timewheel
		
}

/*int Processor::AddNetConnectionToListen(NetConnection *connect){
	int res = mEpoll.Add(connect->GetSockFD(), connect);
	if(res){
		MY_LOG_STREAM_ERROR << "add to epoll error: " << res << MY_LOG_EOM;
	}
	return res;
}*/

void Processor::ModConnectionToEpoll(NetConnection * conn, int eventType){
	MY_LOG_STREAM_DEBUG << "processor [" << mIndex << "], " << "mod " << conn->GetSockFD() << " to epoll  " << MY_LOG_EOM;
	if(conn == NULL){
		MY_LOG_STREAM_INFO << "conn is null" << MY_LOG_EOM;
		return;
	}
	int res = mEpoll.Mod(conn->GetSockFD(), conn, eventType);
	if(res){
		MY_LOG_STREAM_ERROR << "processor [" << mIndex << "], " << "add to epoll error:  " << strerror(errno) << MY_LOG_EOM;
	}
	//conn->SetProcessor(this);
	//TODO:add to timewheel
}

int Processor::DeleteFromEpoll(NetConnection *conn){
	if(conn == NULL){
		MY_LOG_STREAM_INFO << "conn is null" << MY_LOG_EOM;
		return -1;
	}
	
	MY_LOG_STREAM_DEBUG << "processor [" << mIndex << "], " << "delete " << conn->GetSockFD() << " from epoll  " << MY_LOG_EOM;
	//unsigned eventType = EPOLLIN;//¶Á
	int res = mEpoll.Del(conn->GetSockFD());
	if(res){
		MY_LOG_STREAM_ERROR << "processor [" << mIndex << "], " << "delete to epoll error:  " << strerror(errno) << MY_LOG_EOM;
	}
	return res;
	//TODO:delete to timewheel
}

// delete the NetConnection obj after a little seconds, or  
// current step: delete the NetConnection obj after a little seconds
// TODO: using NetConnection pool,  and reclaim the obj to the NetConnection pool
int Processor::RecliamNetConnection(NetConnection *conn){
	
}

void Processor::AddToPendingConnectionList(NetConnection * conn){
	
	MY_LOG_STREAM_DEBUG << "processor [" << mIndex << "], " << "add to pending list  " << MY_LOG_EOM;
	mPendingConnectionList[mFilling].push(conn); 
	int res = conn->SetBufferPool(&mBufferPool);
	if(res == -1){
		MY_LOG_STREAM_ERROR << "processor [" << mIndex << "], " << "buffer pool is NULL, why???  " << MY_LOG_EOM;
	}
	mKicker.Kick();
	
}

void Processor::MainLoop(){
	//bind to cpu
	gCPUManager.BindToProcessor();
	//add kicker to epoll
	mEpoll.Add(mKicker.GetFd(), NULL, EPOLLIN);
	while(mRunFlag){
		//exchange
		mUsing = mUsing^mFilling;
        mFilling = mFilling^mUsing;
        mUsing = mUsing^mFilling;
        //add new connections to epoll
        while(!mPendingConnectionList[mUsing].empty())  {
            NetConnection* ptr1 = mPendingConnectionList[mUsing].front();
            AddConnectionToEpoll(ptr1);
            mPendingConnectionList[mUsing].pop();    
        }
		MY_LOG_STREAM_DEBUG << "processor [" << mIndex << "], " << "waiting for event! " << MY_LOG_EOM;
		mEpoll.EpollWait(MAXEVENTCOUNT, EPOLLTIMEOUT);
		//TODO:ÅÐ¶Ïepoll wait·µ»Ø
		mKicker.Drain();
		int   op;
        void* ptr;
		
        while (mEpoll.Next(op, ptr)) {
            if (op == 0 || ! ptr) {
                continue;
            }
            NetConnection* conn = reinterpret_cast<NetConnection*>(ptr);
			int res = conn->HandleEvent(op);
			if(res == EVENT_NET_CLOSE){
				MY_LOG_STREAM_DEBUG << "processor [" << mIndex << "] delete connection." << MY_LOG_EOM;
				delete conn;
			}
			/*
			if ((op & EPOLLIN) != 0 && conn.IsGood()) {
                conn.HandleReadEvent();
            }
            if ((op & EPOLLOUT) != 0 && conn.IsGood()) {
                conn.HandleWriteEvent();
            }
            if ((op & (EPOLLERR | EPOLLHUP)) != 0 && conn.IsGood()) {
                conn.HandleErrorEvent();
            }*/

		}
	}
	MY_LOG_STREAM_INFO << "processor [" << mIndex << "] exit!" << MY_LOG_EOM;
}
