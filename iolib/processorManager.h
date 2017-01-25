#ifndef _EVENT_MANAGER_
#define _EVENT_MANAGER_
#include "fdEpoll.h"
#include "timeout.h"
#include "processor.h"


class EventBase{
public:
	EventBase(){}
	~EventBase(){}
	virtual int GetFd() = 0;
};

class ProcessorManager: public Timeout, public CallbackObj{
public:
	ProcessorManager():mCallbackObj(NULL), mProcesserList(NULL),mProcessorCount(0),mMaxEventCount(10000),mCurrentProcessorIndex(0){}
	virtual ~ProcessorManager(){}
	void Init(int processorCount = 4, IAcceptorOwner* owner = NULL);
	void ProcessTimeout(){}
	//void Run();
	void MainLoop();
	//void TaskBindSocket();
	int AddOtherEventToEpoll(EventBase *event);
	int DeleteFromEpoll(NetConnection *connection){}
	int HandleEvent(int eventType, void *conn);
	int AddNetConnectionToListen(NetConnection *connection);
	int DispatchConnectionToProcessor(NetConnection *connection);
private:
	FDEpoll	mEpoll;
	CallbackObj *mCallbackObj;
	Processor **mProcesserList;
	int mProcessorCount;
	int mMaxEventCount;
	int mCurrentProcessorIndex;
	
	
	
};
#endif
