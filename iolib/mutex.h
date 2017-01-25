#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
typedef int64_t Time;

//mutex
class Mutex{
public:
	Mutex();
	~Mutex();
	bool Lock(){
		return Locked(pthread_mutex_lock(&mMutex));
	}
	bool Lock(Time inTimeoutNanoSec);
	bool TryLock() {
		const int theErr = pthread_mutex_trylock(&mMutex);
		return (theErr != EBUSY && Locked(theErr));
	}
	bool Unlock(){
		const bool theUnlockedFlag = Unlocked();
		const int theErr = pthread_mutex_unlock(&mMutex);
		if (theErr) {
			RaiseError("QCMutex::Unlock", theErr);
		}
		return theUnlockedFlag;
	}
	bool IsOwned(){ return (::pthread_equal(mOwner, ::pthread_self()) != 0); }

private:
	int             mLockCnt;
	pthread_t       mOwner;
	pthread_mutex_t mMutex;

	void RaiseError(
		const char* inMsgPtr,
		int         inSysError = 0);
	bool Locked(int inErr)
	{
		if (inErr) {
			RaiseError("QCMutex::Locked", inErr);
		}
		if (mLockCnt < 0) {
			RaiseError("QCMutex::Locked mLockCnt < 0");
		}
		if (mLockCnt++ == 0) {
			mOwner = ::pthread_self();
		}
		return true;
	}
	bool Unlocked(){
		if (mLockCnt <= 0) {
		RaiseError("QCMutex::Unlocked mLockCnt <= 0");
		}
		const bool theUnlockedFlag = --mLockCnt == 0;
		if (theUnlockedFlag) {
			mOwner = pthread_t();
		}
		return theUnlockedFlag;
	}
	friend class CondVar;

private:
    // No copies.
	Mutex(const Mutex& inMutex);
	Mutex& operator=(const Mutex& inMutex);
};


//conditional variable
class CondVar{
public:

	CondVar();
	~CondVar();
	bool Wait(Mutex& inMutex){
		inMutex.Unlocked();
		const int theErr = pthread_cond_wait(&mCond, &inMutex.mMutex);
		if (theErr) {
			RaiseError("QCCondVar::Wait", theErr);
		}
		return inMutex.Locked(theErr);
	}
	bool Wait(Mutex& inMutex, Time inTimeoutNanoSec);
	void Notify(){
		const int theErr = pthread_cond_signal(&mCond);
		if (theErr) {
			RaiseError("CondVar::Notify", theErr);
		}
	}
	void NotifyAll(){
		const int theErr = pthread_cond_broadcast(&mCond);
		if (theErr) {
			RaiseError("CondVar::Notify", theErr);
		}
	}

private:
	pthread_cond_t mCond;

	void RaiseError(const char* inMsgPtr, int inSysError = 0);

private:
    // No copies.
	CondVar(const CondVar& inCondVar);
	CondVar& operator=(const CondVar& inCondVar);
};


//semaphore

class Semaphore{
public:
	Semaphore();
	 Semaphore(int shared, int defautVal);
	~Semaphore(){}
	int SemWait();
	int SemTryWait();
	int SemPost();
	int SemDestory();
	int SemToZero();

private:
	sem_t mSem;
	int mShared;// 0: shared between threads and threads, others: shared between process
};

#endif /* QCMUTEX_H */

