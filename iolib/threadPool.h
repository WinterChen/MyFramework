//---------------------------------------------------------- -*- Mode: C++ -*-
//
//
// Created 2013/08/30
// Author: Winter Chen
//
//
//
//----------------------------------------------------------------------------
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <string>
#include <iostream>

using namespace std;

//class Runnable is a thread interface,
//when use thread in another class, such as class X. you can do such as:
//(1) just define X inherited from Runnable, then redefine Run().
//(2) define Y inherited from Runnable as a number of X, redefine Run() in Y.
class Runnable{
public:
	virtual void Run() = 0;

protected:
	Runnable() {}
	virtual ~Runnable() {}
};

class ThreadPool : public Runnable {
public:
	ThreadPool();
	ThreadPool(string name, unsigned int count, Runnable *runnablePtr) : mStartedFlag(0), mName(name), mCount(count), mRunnablePtr(runnablePtr) {mId = new pthread_t[mCount];}
	~ThreadPool(){}
	void Init (string name, unsigned int num, Runnable *runnablePtr);

	static void *Runner(void *arg);
	int Start();
	void Run();
	int Join();
	int Cancel(pthread_t thread);
private:
	bool mStartedFlag;
	string mName;
	unsigned int mCount;
	pthread_t *mId;
	Runnable *mRunnablePtr;

};

#endif
