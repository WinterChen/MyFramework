#include "mutex.h"
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#ifdef QC_OS_NAME_DARWIN
#define pthread_mutex_timedlock(m, l) pthread_mutex_lock(m)
#endif

static int
GetAbsTimeout(Time inTimeoutNanoSec, struct timespec& outAbsTimeout){
#if 0 //defined(_POSIX_TIMERS) && ! defined(QC_OS_NAME_DARWIN)
    const int theErr = clock_gettime(CLOCK_REALTIME, &outAbsTimeout);
    if (theErr != 0) {
        return theErr;
    }
#else
    struct timeval theTimeVal;
    const int theErr = gettimeofday(&theTimeVal, 0);
    if (theErr != 0) {
        return theErr;
    }
    outAbsTimeout.tv_sec  = theTimeVal.tv_sec;
    outAbsTimeout.tv_nsec = (long)theTimeVal.tv_usec * 1000;
#endif
    const Time k2NanoSec  = Time(1000) * 1000000;
    const Time theNanoSec = outAbsTimeout.tv_nsec + inTimeoutNanoSec;
    outAbsTimeout.tv_nsec = long(theNanoSec % k2NanoSec);
    outAbsTimeout.tv_sec += time_t(theNanoSec / k2NanoSec);
    return theErr;
}

Mutex::Mutex(): mLockCnt(0), mOwner(), mMutex(){
	/*
	int theErr;
	pthread_mutexattr_t theAttr;
	if ((theErr = pthread_mutexattr_init(&theAttr)) != 0) {
	RaiseError("Mutex: pthread_mutex_attr_init", theErr);
	}
	if ((theErr = pthread_mutexattr_settype(
            &theAttr, PTHREAD_MUTEX_RECURSIVE)) != 0) {
	RaiseError("Mutex: pthread_mutexattr_settype", theErr);
	}
	if ((theErr = pthread_mutex_init(&mMutex, &theAttr)) != 0) {
	RaiseError("Mutex: pthread_mutex_init", theErr);
	}
	if ((theErr = pthread_mutexattr_destroy(&theAttr)) != 0) {
	RaiseError("Mutex: pthread_mutexattr_destroy", theErr);
	}*/
}

Mutex::~Mutex(){
    const int theErr = pthread_mutex_destroy(&mMutex);
    if (theErr != 0) {
        RaiseError("Mutex::~Mutex: pthread_mutex_destroy", theErr);
    }
}

    bool
Mutex::Lock(Time inTimeoutNanoSec)
{
    struct timespec theAbsTimeout;
    int theErr = GetAbsTimeout(inTimeoutNanoSec, theAbsTimeout);
    if (theErr != 0) {
        RaiseError("Mutex::Lock: clock_gettime", theErr);
    }
    theErr = pthread_mutex_timedlock(&mMutex, &theAbsTimeout);
    if (theErr == ETIMEDOUT) {
        return false;
    }
    if (theErr != 0) {
        RaiseError("Mutex::Lock: pthread_mutex_timedlock", theErr);
    }
    return true;
}

void
Mutex::RaiseError(const char* inMsgPtr,int inSysError){
    //QCUtils::FatalError(inMsgPtr, inSysError);
    cout <<  inMsgPtr << inSysError << endl;
}

CondVar::CondVar(): mCond(){
	const int theErr = pthread_cond_init(&mCond, 0);
	if (theErr) {
		RaiseError("CondVar::CondVar: pthread_cond_init", theErr);
	}
}

CondVar::~CondVar(){
	const int theErr = pthread_cond_destroy(&mCond);
	if (theErr) {
		RaiseError("CondVar::~CondVar: pthread_cond_destroy", theErr);
	}
}

bool
CondVar::Wait(Mutex& inMutex, Time inTimeoutNanoSec){
	struct timespec theAbsTimeout;
	int theErr = GetAbsTimeout(inTimeoutNanoSec, theAbsTimeout);
	if (theErr != 0) {
		RaiseError("CondVar::Wait: clock_gettime", theErr);
	}
	inMutex.Unlocked();
	theErr = pthread_cond_timedwait(&mCond, &inMutex.mMutex, &theAbsTimeout);
	if (theErr == ETIMEDOUT) {
		return false;
	}
	if (theErr != 0) {
		RaiseError("CondVar::Wait: pthread_cond_timedwait", theErr);
	}
	return inMutex.Locked(theErr);
}

void
CondVar::RaiseError(const char* inMsgPtr, int inSysError){
    //QCUtils::FatalError(inMsgPtr, inSysError);
}



Semaphore::Semaphore(): mShared(0){
	int res = sem_init(&mSem, mShared, 0);
	if(res != 0) {
		perror("sem_init");
	}

}

Semaphore::Semaphore(int shared, int defautVal): mShared(shared){
	int res = sem_init(&mSem, mShared, defautVal);
	if(res != 0) {
		perror("sem_init");
	}
}

int
Semaphore::SemPost(){
	return sem_post(&mSem);
}

int
Semaphore::SemTryWait(){
	return sem_trywait(&mSem);
}

int
Semaphore::SemWait(){
	return sem_wait(&mSem);
}

int
Semaphore::SemDestory(){
	return sem_destroy(&mSem);
}


//return: 0, succ
int
Semaphore::SemToZero(){
	while(SemTryWait() == 0)
		;
	return 0;
}
