//Created: 2013.05.02
//Author: Winter Chen
//This is the doubly-linked-queue data struct for high-performance disk IO, created by Witner.
#include "queue.h"

template <class T>
void
Queue<T>::Enqueue(QueueNode<T> *inNode){
	//BufferQueueNode *p = new BufferQueueNode();
	//p->mBufferPtr = ptr;
	inNode->mPrev = mHead->mPrev;
	inNode->mNext = mHead;
	mHead->mPrev->mNext = inNode;
	mHead->mPrev = inNode;
	//cout << "get a buffer, address is :" << (void*)res << endl;
	//return res;
}


template <class T>
QueueNode<T>* 
Queue<T>::Dequeue(){
	QueueNode<T> *p = mHead->mNext;
	p->mPrev->mNext = p->mNext;
	p->mNext->mPrev= p->mPrev;
	p->mPrev = p->mNext = 0;
	//cout << "get a buffer, address is :" << (void*)res << endl;
	return p;
}

template <class T>
bool
Queue<T>::IsEmpty(){
	return mHead->mPrev == mHead->mNext;
}
