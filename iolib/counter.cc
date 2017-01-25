#include "counter.h"


void Counter::AddRequestCount(unsigned count){
	mTotalRequestCount += count;
}
void Counter::AddResponsecount(unsigned count){
	mTotalResponseCount += count;
}
void Counter::AddTotalTimeuse(unsigned long  usecond){
	mTotalTimeuse += usecond;
}
unsigned Counter::GetRequestCountAfterPrev(){
	unsigned tmp = mTotalRequestCount;
	unsigned res = tmp - mPrevRequestCount;
	mPrevRequestCount = tmp;
	return res;
}
unsigned Counter::GetResponseCountAfterPrev(){
	unsigned tmp = mTotalResponseCount;
	unsigned res = tmp - mPrevResponseCount;
	mPrevResponseCount = tmp;
	return res;
}
unsigned long long Counter::GetAvgTime(){
	//unsigned tmp = mTotalResponseCount;
	if(mTotalResponseCount == 0)
		return 0;
	else
		return mTotalTimeuse/mTotalResponseCount;
}

unsigned long long Counter::GetAvgTimeAfterPrev(){
	unsigned long long  tmp = mTotalTimeuse;
	unsigned long long res = tmp - mPrevTimeuse;
	mPrevTimeuse = tmp;
	if(mPrevResponseCount == 0)
		return 0;
	return res/mPrevResponseCount;
}




