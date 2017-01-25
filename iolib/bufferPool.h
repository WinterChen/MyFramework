//Author:Winter Chen
//Created:2013-4-18
//此文件设计一个内存池
//
#ifndef MYBUFFERPOOL_H
#define MYBUFFERPOOL_H

#include <stddef.h>
#include<stdio.h>

class BufferPool{
	public:
		BufferPool():mPoolSize(0),
			mPageSize(0),mTotalPagesAmount(0),
			mFreePagesCount(0),mFreeListPtr(0){}
		~BufferPool(){}

		int Creat(unsigned int poolSize, unsigned int pageSize = 0, bool mmapMemory = 0, bool lockMemory = 0);
		void Destroy();
		char* Get();
		char* Get(unsigned int size);
		bool Reclaim(char *ptr);
		bool Reclaim(char *ptr, unsigned int size);
		bool IsEmpty(){ return mFreePagesCount <= 0 ? 1 : 0;}
		bool IsFull(){ return mFreePagesCount ==  mTotalPagesAmount? 1 : 0;}
		unsigned int GetFreePagesCount(){return mFreePagesCount;}
		unsigned int GetFreePoolSize(){return mPoolSize;}
		
		
	private:
		void *mPool;
		unsigned int mPoolSize;
		unsigned int mPageSize;
		unsigned int mTotalPagesAmount;
		unsigned int mFreePagesCount;
		int *mFreeListPtr;
		//char *mStartPtr;
};




class BufferQueueNode{
public:
	BufferQueueNode():mBufferPtr(0), mNext(this), mPrev(this){}
	~BufferQueueNode(){
		if(mBufferPtr)
		delete mBufferPtr;
		mBufferPtr = 0;
		mNext = 0;
		mPrev = 0;
	}
	char *mBufferPtr;
	BufferQueueNode *mNext;
	BufferQueueNode *mPrev;
};


class BufferPoolQueue{
public:
	BufferPoolQueue():mPoolSize(0),
		mDatalet(0), mHead(0), mPool(0){}
	~BufferPoolQueue(){Destroy();}


	int Creat(unsigned int poolSize, unsigned int datalet, bool mmapMemory = 0, bool lockMemory = 0);
	void Destroy();
	void Enqueue(BufferQueueNode *inNode);
	BufferQueueNode *Dequeue();
	BufferQueueNode *Allocate();
	void Reclaim(BufferQueueNode *inNode);
	bool IsEmpty(){ return ((mHead == NULL) || (mHead->mNext == mHead))  ? 1 : 0;}
	unsigned int GetPoolSize(){return mPoolSize;}
//unsigned int GetFreepoolsize();


private:
	unsigned int mPoolSize;
	unsigned int mDatalet;
	BufferQueueNode *mHead;
	void *mPool;
	bool mIsMmap;
};


#endif
