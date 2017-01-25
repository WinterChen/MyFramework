//

#include "bufferPool.h"
#include <errno.h>
#include <iostream>
#include <sys/mman.h>
#include <stddef.h>
#include <unistd.h>

using namespace std;




int 
BufferPool::Creat(unsigned int poolSize, unsigned int pageSize, bool mmapMemory, bool lockMemory){
	if(pageSize <= 0)
		mPageSize = sysconf(_SC_PAGESIZE);
	else
		mPageSize = pageSize;
	mTotalPagesAmount = poolSize/mPageSize + 1;
	mPoolSize = mTotalPagesAmount*mPageSize;
	cout << "mPageSize = " << mPageSize << " mTotalPagesAmount = " << mTotalPagesAmount <<" mPoolSize = "<< mPoolSize << endl;
	if(mmapMemory){
		mPool = mmap(0, mPoolSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	    if (mPool == MAP_FAILED) {
			mPool = 0;
			return errno;
		}
	}
	else 
		mPool = new char[mPoolSize];
	//mlock:通过mlock可以将进程使用的部分
	//或者全部的地址空间锁定在物理内存中，防止其被交换到swap空间
	if(lockMemory && mlock(mPool, mPoolSize) != 0){
		Destroy();
		return errno;

	}
	//mStartPtr = (char*)mPool;
	mFreeListPtr = new int[mTotalPagesAmount+ 1];
	mFreePagesCount = 0;
	while(mFreePagesCount < mTotalPagesAmount){
		mFreeListPtr[mFreePagesCount] = mFreePagesCount+1;
		mFreePagesCount++;
	}
	mFreeListPtr[mTotalPagesAmount] = 0;
	return 0;
}

bool
BufferPool::Reclaim(char *ptr){
	if(IsFull())
		return 0;
	//res = mStartPtr;
	//mStartPtr += mPageSize;
	unsigned int pageNum = (ptr - (char*)mPool)/mPageSize + 1;
	if(pageNum >= mTotalPagesAmount)
		return 0;
	mFreeListPtr[pageNum] = mFreeListPtr[0];
	mFreeListPtr[0] = pageNum;
	mFreePagesCount++;
	//ptr = 0;
	return 1;
}

char*
BufferPool::Get(){
	if(IsEmpty()){
		cout << "empty!!!" << endl;
		return NULL;

	}
	char *res;
	res = (char*)mPool + (mFreeListPtr[0]-1) * mPageSize;
	mFreeListPtr[0] = mFreeListPtr[mFreeListPtr[0]];
	mFreePagesCount--;
	//cout << "[iolib] get a buffer, address is :" << (void*)res << endl;
	return res;
}

void
BufferPool::Destroy(){
	delete [] mFreeListPtr;
	mFreeListPtr = 0;
	if (mPool && munmap(mPool, mPoolSize) != 0) {
		cout <<"munmap error: " << errno <<endl;
	}
	mPool = 0;
	mPoolSize = 0;
	mTotalPagesAmount  = 0;
	mFreePagesCount = 0;
}


int 
BufferPoolQueue::Creat(unsigned int poolSize, unsigned int datalet, bool mmapMemory, bool lockMemory){
	unsigned int pageSize = sysconf(_SC_PAGESIZE);
	mPoolSize = (poolSize/pageSize + 1)*pageSize;
	if(datalet <= 0)
		return 0;
	mDatalet = datalet;
    //cout << "mPageSize = " << mPageSize << " mTotalPagesAmount = " << mTotalPagesAmount <<" mPoolSize = "<< mPoolSize << endl;
	if(mmapMemory){
		mPool = mmap(0, mPoolSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
		if (mPool == MAP_FAILED) {
			mPool = 0;
			return errno;
		}
	}
	else 
		mPool = new char[mPoolSize];
    //mlock:??mlock??????????
    //?????????????????,???????swap??
	if(lockMemory && mlock(mPool, mPoolSize) != 0){
		Destroy();
		return errno;
	}
//create the head node of queue
	mHead = new BufferQueueNode();
	unsigned int pos = 0;
	char *ptr = (char*) mPool;
	while(pos < (mPoolSize - datalet)){
		BufferQueueNode *p = new BufferQueueNode();
		ptr += pos;
		p->mBufferPtr = ptr;
		Enqueue(p);
		pos += datalet;
	}
	return 1;
}


void
BufferPoolQueue::Reclaim(BufferQueueNode *inNode){
	Enqueue(inNode);
}




BufferQueueNode *
BufferPoolQueue::Allocate(){
	return Dequeue();
}


void
BufferPoolQueue::Enqueue(BufferQueueNode *inNode){
	inNode->mPrev = mHead->mPrev;
	inNode->mNext = mHead;
	mHead->mPrev->mNext = inNode;
	mHead->mPrev = inNode;
}


BufferQueueNode *
BufferPoolQueue::Dequeue(){
	if(IsEmpty())
		return NULL;
	BufferQueueNode *p = mHead->mNext;
	p->mPrev->mNext = p->mNext;
	p->mNext->mPrev= p->mPrev;
	p->mPrev = p->mNext = 0;
	return p;
}


void
BufferPoolQueue::Destroy(){
	while(!IsEmpty()){
		delete Dequeue();
	}
	delete mHead;
	mHead = NULL;
	if(mIsMmap){
		if (mPool && munmap(mPool, mPoolSize) != 0) {
			cout <<"munmap error: " << errno <<endl;
		}
	}
	else 
		delete [] (char*)mPool;
	mPool = 0;
	mPoolSize = 0;
}


/*unsigned int 
BufferPoolQueue::GetFreepoolsize(){

}
*/

/*
int main(){
	cout << "buffer pool test......" << endl;
	unsigned int size = 32 << 20;
	cout << "alloc buffer size : " << size << endl;
	BufferPool *bufferPool = new BufferPool();
	bufferPool->Creat(size);
	char *buf1 = bufferPool->Get();
	cout << "get a buffer, buf1 : " << (void*)buf1 <<endl;
	char *buf2 = bufferPool->Get();
	cout << "get a buffer, buf2 : " << (void*)buf2 << endl;
	if(bufferPool->Reclaim(buf1))
		buf1 = 0;
	cout << "after reclaim buffer, buf1 : " << (void*)buf1 <<endl;
	char *buf3 = bufferPool->Get();
	cout << "get a buffer, buf3 : " << (void*)buf3 << endl;



}

*/
