#ifndef _TIMEOUT_H_
#define _TIMEOUT_H_

#include <sys/time.h>

class Timeout{
public:
    Timeout() : mIntervalMs(0), mDisabled(false), mLastCall(0) { }
    virtual ~Timeout() {}
    void Disable() {
        mDisabled = true;
    }
    void SetTimeoutInterval(int intervalMs, bool resetTimer = false) {
        mDisabled = false;
        mIntervalMs = intervalMs;
        if (resetTimer) {
            ResetTimer();
        }
    }

    int GetTimeElapsed() {
        return (NowMs() - mLastCall);
    }

    void ResetTimer() {
        mLastCall = NowMs();
    }

    static long long NowMs() {
        struct timeval timeNow;
        gettimeofday(&timeNow, 0);
        return ((long long)(timeNow.tv_sec) * 1000 + timeNow.tv_usec / 1000);
    }

    void TimerExpired(long long nowMs) {
        if (mDisabled)
            return;
        if (mIntervalMs <= 0 || nowMs >= mLastCall + mIntervalMs) {
            ProcessTimeout();
            mLastCall = nowMs;
        }
    }
    
    /// This method will be invoked when a timeout occurs.
    virtual void ProcessTimeout() = 0;
protected:
    int		mIntervalMs;
    bool        mDisabled;
private:
    long long 	mLastCall;
};

#endif
