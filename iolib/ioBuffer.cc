#include <string.h>

#include "ioBuffer.h"
#include "log.h"

using namespace myframework;


int IOBuffer::SetBufferPool(BufferPool *bp){
	if(bp == NULL){
		
		MY_LOG_STREAM_ERROR << "BufferPool is null  " << MY_LOG_EOM;
		return -1;

	}
	mBufferPool = bp;
	return 0;
}

char* IOBuffer::GetReadData(int len){
	char *buf;
	if(len <= 0){
		MY_LOG_STREAM_ERROR << "read len error, need  " << len << MY_LOG_EOM;
		return NULL;
	}
	
	MY_LOG_STREAM_DEBUG << "fd: " << mSocketFd << ". ReadBufferLen len:  " << mReadBufferLen  << ", read len: " << len << MY_LOG_EOM;
	if(mInBufferQueue.empty() || mReadBufferLen < len){
		MY_LOG_STREAM_DEBUG << "read from buffer error, need  " << len << ", but " << mReadBufferLen << MY_LOG_EOM;
		return NULL;
	}
	if(len <= BUFFERPOOLPAGESIZE - mLeftOfReadBuffer){
		mReadBufferLen -= len;
		buf = mInBufferQueue.front() + mLeftOfReadBuffer;
		if(mReadBufferLen == 0){
			mRightOfReadBuffer = mLeftOfReadBuffer = 0;
		}
		else
			mLeftOfReadBuffer += len;
		return buf;
	}
	else{
		if(mInBuffer == NULL){
			mInBuffer  = new char[len];
			mInBufferLen = len;
		}
		else if(mInBufferLen < len){
			delete mInBuffer;
			mInBuffer = new char[len];
			mInBufferLen = len; 
		}
		int remainder = len;
		int readLen = 0;
		char *srcBuf = NULL;
		while(remainder > 0){
			srcBuf = mInBufferQueue.front() + mLeftOfReadBuffer;
			buf = mInBuffer + readLen;
			if(remainder <= BUFFERPOOLPAGESIZE - mLeftOfReadBuffer)
				readLen = remainder;
			else
				readLen = BUFFERPOOLPAGESIZE - mLeftOfReadBuffer;
			memcpy(buf, srcBuf, readLen);//there is bug, will coredump here
			remainder -= readLen;
			mLeftOfReadBuffer += readLen;
			mReadBufferLen -= readLen;
			//no data in buffer, break
			if(mReadBufferLen == 0){
				mRightOfReadBuffer = mLeftOfReadBuffer = 0;
				break;
			}
			//current buffer of front queue node read over, so read from next node
			if(BUFFERPOOLPAGESIZE == mLeftOfReadBuffer){
				mBufferPool->Reclaim(srcBuf);
				mInBufferQueue.pop();
				mLeftOfReadBuffer = 0;
			}	
		}
	}
	return buf;// 
}

//return 0: socket close
//return <0: read error
//return >0: read succ
int IOBuffer::Read(TcpSocket *socket){
	if(socket){
		int sock = socket->GetSockFd();
		if(sock != mSocketFd){
			
			MY_LOG_STREAM_INFO << "diff socket fd. new socket fd: " << sock << ". mSocketFd: " << mSocketFd << MY_LOG_EOM;
			mSocketFd = sock;
		}
		int readLen = 0;
		int res = 0;
		char *buf = NULL;
		int readed = 0;
		while(1){
			if(mInBufferQueue.empty()){
				if(mBufferPool->IsEmpty()){
					MY_LOG_STREAM_ERROR << "***-------------- buffer pool is full!!! ----------------***  " << MY_LOG_EOM;
					return -3;
				}					
				else{ 
					mInBufferQueue.push(mBufferPool->Get());
				}
			}
			buf = mInBufferQueue.back() + mRightOfReadBuffer;
			readLen = BUFFERPOOLPAGESIZE - mRightOfReadBuffer;
			res = socket->Read(buf, readLen);
			if(res > 0 && res < readLen){//read succ and compeleted
				mRightOfReadBuffer += res;
				mReadBufferLen += res;
				readed += res;
				return readed;
	
			}
			if((readLen != 0 && res == 0) || res < 0){//socket close
				break;
			}
			if(res == readLen){//buffer full, need read more
				mInBufferQueue.push(mBufferPool->Get());
				mRightOfReadBuffer = 0;
				mReadBufferLen += res;
				readed += res;
			}//
		}
		
		return res;
	}
	else
		return -2;//socket is null
}

//return -1:write error or left some bytes need write next 
//return 0:writed all data
//
int IOBuffer::Write(TcpSocket *socket){
	if(socket){
		char *currentBuf;
		ssize_t res;
		int pos = 0;
		int len = 0;
		while(!mOutBufferQueue.empty()){
			currentBuf = mOutBufferQueue.front();
			pos = mLeftOfWriteBuffer;
			if(mNeedWrite + mLeftOfWriteBuffer < BUFFERPOOLPAGESIZE)
				len = mNeedWrite;
			else
				len = BUFFERPOOLPAGESIZE - mLeftOfWriteBuffer;
			res = socket->Write(currentBuf + pos, len);
			if(res == len){
				mLeftOfWriteBuffer = 0;
				mNeedWrite -= len;
				mBufferPool->Reclaim(currentBuf);
				mOutBufferQueue.pop();
			}
			else if(res < 0){
				return -1;
			}
			else{
				mLeftOfWriteBuffer += res;
				mNeedWrite -= res;
				return -1;
			}
		}
		//now all the data in the buffer were send out
		return 0;
	}
}
//type: 
//  1, write
//  0, read
int IOBuffer::Append(char *buf, int len, bool type){
	
	//int pos = 0;
	int remainder = len;
	if(type){
		while(remainder > 0){
			//pos = mRightOfWriteBuffer;
			if(mRightOfWriteBuffer + remainder <= BUFFERPOOLPAGESIZE){
				if(mOutBufferQueue.empty()){
					if(mBufferPool->IsEmpty()){
						MY_LOG_STREAM_ERROR << "***-------------- buffer pool is empty!!! ----------------***  " << MY_LOG_EOM;
						return -1;
					}					
					else{ 
						mOutBufferQueue.push(mBufferPool->Get());
					}
				}
				memcpy(mOutBufferQueue.back() + mRightOfWriteBuffer, buf, remainder);
				mRightOfWriteBuffer += remainder;
				remainder = 0;
			}
			else{
				int copyLen = BUFFERPOOLPAGESIZE - mRightOfWriteBuffer;
				memcpy(mOutBufferQueue.back() + mRightOfWriteBuffer, buf, copyLen);		
				remainder -= copyLen;
				//mNeedWrite += copyLen;
				
				if(mOutBufferQueue.empty()){
					if(mBufferPool->IsEmpty()){
						MY_LOG_STREAM_ERROR << "***-------------- buffer pool is empty!!! ----------------***  " << MY_LOG_EOM;
						return -1;
					}					
					else{ 
						mOutBufferQueue.push(mBufferPool->Get());
					}
				}
				mRightOfWriteBuffer = 0;
			}
		}
		mNeedWrite += len;
	}
	else{
		while(remainder > 0){
			//pos = mRightOfReadBuffer;
			if(mRightOfReadBuffer + remainder <= BUFFERPOOLPAGESIZE){
				if(mOutBufferQueue.empty()){
					if(mBufferPool->IsEmpty()){
						MY_LOG_STREAM_ERROR << "***-------------- buffer pool is full!!! ----------------***  " << MY_LOG_EOM;
						return -1;
					}					
					else{ 
						mOutBufferQueue.push(mBufferPool->Get());

					}
					//mOutBufferQueue.push(mBufferPool->Get());

				}
				memcpy(mInBufferQueue.back() + mRightOfReadBuffer, buf, remainder);
				mRightOfReadBuffer += remainder;
				remainder = 0;
			}
			else{
				int copyLen = BUFFERPOOLPAGESIZE - mRightOfReadBuffer;
				memcpy(mInBufferQueue.back() + mRightOfReadBuffer, buf, copyLen);
				remainder -= copyLen;
				//mNeedRead += copyLen;
				if(mBufferPool->IsEmpty()){
					MY_LOG_STREAM_ERROR << "***-------------- buffer pool is full!!! ----------------***  " << MY_LOG_EOM;
					return -1;
				}					
				else{ 
					
					mInBufferQueue.push(mBufferPool->Get());
				}
				mRightOfReadBuffer = 0;
			}
		}
		mReadBufferLen += len;
	}
		
	
}

