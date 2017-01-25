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

class EventManager: public Timeout, public CallbackObj{
public:
	EventManager():mCallbackObj(NULL), mProcesserList(NULL),mProcessorCount(0),mMaxEventCount(10000),mCurrentProcessorIndex(0){}
	virtual ~EventManager(){}
	void Init(int processorCount = 4);
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


extern EventManager gEventManager;

#endif
