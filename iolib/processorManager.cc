#include "processorManager.h"
#include "type.h"
#include "log.h"
using namespace myframework; 

//EventManager gEventManager;

void ProcessorManager::Init(int processorCount, IAcceptorOwner* owner){
	mProcessorCount = processorCount;
	mProcesserList = new Processor*[mProcessorCount];
	Processor *processor = NULL;
	for(int i = 0; i < mProcessorCount; i++){
		processor = new Processor(i, owner);
		mProcesserList[i] = processor;
		if(processor){
			processor->Start();//run the thread
		}
	}
	
}

int ProcessorManager::HandleEvent(int eventType, void *conn){
	NetConnection *connection = (NetConnection*)conn;
	if(eventType == EVENT_NEW_CONNECTION){
		return DispatchConnectionToProcessor(connection);
	}
	return 0;
}


int ProcessorManager::DispatchConnectionToProcessor(NetConnection *connection){
	int index = mCurrentProcessorIndex++%mProcessorCount;
	
	MY_LOG_STREAM_DEBUG << "add to processor  " << index << MY_LOG_EOM;
	mProcesserList[index]->AddToPendingConnectionList(connection);
	return index;
}


int ProcessorManager::AddNetConnectionToListen(NetConnection *connect){
	//connect->SetCallbackObj(this);
	int res = mEpoll.Add(connect->GetSockFD(), connect);
	if(res){
		MY_LOG_STREAM_ERROR << "add to epoll error: " << res << MY_LOG_EOM;
	}
	return res;
}

void ProcessorManager::MainLoop(){
	
	while(1){
		//TimerExpired(NowMs());
		MY_LOG_STREAM_DEBUG << "event manager  waiting for event..." << MY_LOG_EOM;
		mEpoll.EpollWait(mMaxEventCount, 200000);
		void *ptr = NULL;
		int op;
		while(mEpoll.Next(op, ptr)){
			if(ptr != NULL){
				NetConnection &connection  = *reinterpret_cast<NetConnection*>(ptr);
				connection.HandleEvent(op);
			}
			else{
				//TODO:other events
			}
			
		}
	}
}

