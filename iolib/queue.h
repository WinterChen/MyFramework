//Created: 2013.05.02
//Author: Winter Chen
//This is the doubly-linked-queue data struct for high-performance disk IO, created by Witner.


#ifndef QUEUE_H
#define QUEUE_H
#include<stdio.h> //for NULL

template <class T>
class QueueNode{
public:
	QueueNode() : mPrev(NULL), mNext(NULL), mObj(NULL){}
	~QueueNode(){
		mPrev = mNext = NULL;
		mObj = NULL;
	}
	QueueNode *mPrev;
	QueueNode *mNext;
	T *mObj;

};

template <class T>
class Queue{
public:
	Queue();
	~Queue();
	QueueNode<T> *Dequeue();
	void Enqueue(QueueNode<T> *node);
	bool IsEmpty();
	
private:
	QueueNode<T> *mHead;
	
}; 


#endif
