#ifndef _COUNTER_H_
#define _COUNTER_H_
#include <sys/time.h>
using namespace std;
class Counter{
public:
	Counter():mTotalRequestCount(0),mPrevRequestCount(0),mTotalResponseCount(0),mPrevResponseCount(0),
				mTotalTimeuse(0),mPrevTimeuse(0){gettimeofday(&mStartTime,0);gettimeofday(&mEndTime,0);}
	~Counter(){}
	void AddRequestCount(unsigned count = 1);
	void AddResponsecount(unsigned count = 1);
	void AddTotalTimeuse(unsigned long  usecond);
	unsigned GetRequestCountAfterPrev();
	unsigned GetResponseCountAfterPrev();
	unsigned GetTotalRequestCount(){return mTotalRequestCount;}
	unsigned GetTotalResponseCount(){return mTotalResponseCount;}
	unsigned long long GetAvgTime();
	unsigned long long GetAvgTimeAfterPrev();
private:
	unsigned mTotalRequestCount;
	unsigned mPrevRequestCount;
	unsigned mTotalResponseCount;
	unsigned mPrevResponseCount;
	unsigned long long mTotalTimeuse;
	unsigned long long mPrevTimeuse;
	struct timeval  mStartTime;
	struct timeval  mEndTime;
};

#endif
