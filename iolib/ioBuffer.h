#ifndef IOBUFFER_H
#define IOBUFFER_H
#include <queue>

#include "tcpSocket.h"
#include "bufferPool.h"
using namespace std;

#define BUFFERPOOLPAGESIZE 4096
class IOBuffer{
public:
	IOBuffer():
				mInBuffer(NULL),
				mInBufferLen(0),
				mBufferPool(NULL),
				mNeedWrite(0),
				mLeftOfWriteBuffer(0),
				mRightOfWriteBuffer(0),
				mLeftOfReadBuffer(0),
				mRightOfReadBuffer(0),
				mReadBufferLen(0),
				
				mSocketFd(0){}
	~IOBuffer(){}
	char *GetReadData(int len);
	int SetBufferPool(BufferPool *bp);
	int Read(TcpSocket *socket);/*{
		if(socket){
			char *buf = mInBufferQueue.back() + mRightOfReadBuffer;
			int readLen = BUFFERPOOLPAGESIZE - mRightOfReadBuffer;
			int res = socket->Read(buf, readLen);
			if(res )
			return mDataLen;
		}
		else
			return -1;
	}*/
	int Write(TcpSocket *socket);/*{
		if(socket){
			ssize_t res = socket->Write(mBuffer, mDataLen);
			if(res == len){
				return res;
			}
			else{
				// TODO:write fail, do soming 
				//if
				//mDataLen = len
				if(res < 0 && (errno == EINTR || errno == EAGAIN) ){
					
				}
			}
			//return mDataLen;
		}
		else
			return -1;
	}*/
	//type: 
	//   1, write
	//   0, read
	int Append(char *buf, int len, bool type = true);/*{
		queue<char *> *bqPtr;
		if(type){
			bqPtr = &mOutBufferQueue;
		}
		else
			bqPtr = &mInBufferQueue;
		
		int pos = 0;
		int remainder = len;
		while(remainder > 0){
			pos = mNeedWrite % BUFFERPOOLPAGESIZE;
			if(pos + len <= BUFFERPOOLPAGESIZE){
				memcpy(bqPtr.front() + pos, buf, len);
				remainder = 0;
				mNeedWrite += len;
			}
			else{
				int copyLen = BUFFERPOOLPAGESIZE - pos;
				memcpy(bqPtr.front() + pos, buf, copyLen);
				remainder -= copyLen;
				mNeedWrite += copyLen;
				bqPtr.push(mBufferPool->Get());
			}
		}
	}*/

	//bool Empty(){return mDataLen == 0;}
	bool OutBufferEmpty(){return mNeedWrite==0;}
public:
	char *mInBuffer;//信令一般都小于1024个字节。这里可以扩展为从内存池分配内存
	int mInBufferLen;
	BufferPool *mBufferPool;
	queue<char *> mOutBufferQueue;
	queue<char *> mInBufferQueue;
	int mNeedWrite;
	int mLeftOfWriteBuffer;
	int mRightOfWriteBuffer;
	
	int mLeftOfReadBuffer;
	int mRightOfReadBuffer;
	
	int mLeftOfWriteInFirstBuffer;
	int mLeftOfReadInFirstBuffer;
	//int mNeedRead;
	int mReadBufferLen;
	int mSocketFd;
	
};



#endif
