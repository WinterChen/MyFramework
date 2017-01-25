#include <unistd.h>
//#include <syswait.h>
#include "threadPool.h"

ThreadPool::ThreadPool(){
	mCount = 1;
	mName = "testing";
	mId = new pthread_t[1];
	mRunnablePtr = this;
}

void
ThreadPool::Init(string name, unsigned int count, Runnable *runnablePtr){
	mName = name;
	mCount = count;
	mRunnablePtr = runnablePtr;
}



int
ThreadPool::Start(){
	int theErr;
	if(mCount == 0){
		cout << "no need threads to start!" << endl;
		return -999;
	}	
	for(unsigned int i = 0; i < mCount; i++){
		theErr = pthread_create(&mId[i], NULL, &Runner, mRunnablePtr);
		if (theErr != 0) {
			cout << "creat thread error: " << theErr << endl;
			return theErr;
		}
	}
	mStartedFlag = 1;
	return theErr;
}

void*
ThreadPool::Runner(void *arg){
	reinterpret_cast<Runnable*>(arg)->Run();
}

void
ThreadPool::Run(){
	cout << "run thread as testing! sleep 10S, then exit!" << endl;
	sleep(10);
}

int
ThreadPool::Join(){
	if (! mStartedFlag) {
		return 0;
	}
	for(unsigned int i = 0; i < mCount; i++){
		int theErr = pthread_join(mId[i], 0);
		if (theErr != 0) {
			cout << "join thread " << mId[i] << "  error: " << theErr << endl;
			return theErr;
		}
	}
	mStartedFlag = 0;
	return 1;
}

