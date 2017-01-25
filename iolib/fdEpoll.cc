#include "fdEpoll.h"
#include <unistd.h>
#include<iostream>
#include "log.h"
using namespace myframework;
using namespace std;
#define MAXEPOLLSIZE 10000


FDEpoll::FDEpoll():mEpollFD(epoll_create(MAXEPOLLSIZE)),mMaxEventCount(10000),mEventsPtr(0){
    if(mEpollFD < 0)
		MY_LOG_STREAM_ERROR << "create mEpollFD fail" << MY_LOG_EOM;
}
FDEpoll::~FDEpoll(){
   	if(mEpollFD >= 0)
		close(mEpollFD);
	delete [] mEventsPtr;
}

int
FDEpoll::Add(int fd, void * ptr, int eventType){
	MY_LOG_STREAM_DEBUG << "add " << fd << " to epoll"  << " ptr: " << (void*)ptr<< MY_LOG_EOM;
    return Ctl(fd, ptr, EPOLL_CTL_ADD, eventType);
}

int
FDEpoll::Mod(int fd, void * ptr, int eventType){
	MY_LOG_STREAM_DEBUG << "mod " << fd << " to epoll"  << " ptr: " << (void*)ptr << MY_LOG_EOM;
    return Ctl(fd, ptr, EPOLL_CTL_MOD,eventType);
}

int
FDEpoll::Del(int fd){
	MY_LOG_STREAM_DEBUG << "del " << fd << " from epoll"  << MY_LOG_EOM;
    return Ctl(fd, NULL, EPOLL_CTL_DEL, 0);
}

int
FDEpoll::Ctl(int fd, void * ptr, int opType, int eventType){
    struct epoll_event ev;
	ev.data.fd = fd;
	ev.data.ptr = ptr;
	ev.events = eventType;
    return epoll_ctl(mEpollFD, opType, fd, &ev);
}

int
FDEpoll::EpollWait(int maxEventCount, int waitMilliSec){
	mMaxEventCount = maxEventCount;
	if(mEventsPtr == NULL || maxEventCount > mMaxEventCount){
		if(mEventsPtr != NULL) delete [] mEventsPtr;
		mMaxEventCount = maxEventCount;
		mEventsPtr = new struct epoll_event[mMaxEventCount];
	}
	
	mEventingCount = epoll_wait(mEpollFD, mEventsPtr, mMaxEventCount, waitMilliSec);
	mNextEventIdx = 0;
	return mEventingCount;
}

bool
FDEpoll::Next(int&   outOpType, void* &ptr){
     if(mNextEventIdx >=  mEventingCount)
	 	return false;
	 ptr = mEventsPtr[mNextEventIdx].data.ptr;
	 outOpType = mEventsPtr[mNextEventIdx].events;
	 //cout << " ptr: " << (void*)ptr<< endl;
	 /*if(mEventsPtr[mNextEventIdx].events&EPOLLIN)
	 	cout << "read event!" << endl;
	 if(mEventsPtr[mNextEventIdx].events&EPOLLOUT)
	 	cout << "write event!" << endl;*/
	 mNextEventIdx++;
	 return true;
}

