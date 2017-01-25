#ifndef FDEPOLL_H
#define FDEPOLL_H

#include<sys/epoll.h>
//

//

class FDEpoll{
public:
	FDEpoll();
	~FDEpoll();
	/**the next three founctions are  encapsulated of  
	        int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
	   the op has three values:EPOLL_CTL_ADD EPOLL_CTL_MOD EPOLL_CTL_DEL
	**/
	//EPOLL_CTL_ADD
	/*@return 0 success
	               -1 fail
	 */
	int Add(int fd, void *ptr, int eventType = EPOLLIN);
	//EPOLL_CTL_MOD
	/*@return 0 success
	               -1 fail
	 */
	int Mod(int fd, void *ptr, int eventType);
	//EPOLL_CTL_DEL
	/*@return 0 success
	               -1 fail
	 */
	int Del(int fd);
	//this founction invoke the epoll_ctl
	/*@return 0 success
	               -1 fail
	 */
	int Ctl(int fd, void *ptr, int opType, int eventType);
	/**this founction invoke:
	    int epoll_wait(int epfd,struct epoll_event * events,int maxevents,int timeout);
	**/
	bool Next(int &outOpType, void * &ptr);
	int  EpollWait(int maxEventCount, int waitMilliSec);	
private:
	int mEpollFD;
	int mMaxEventCount;
	int mEventingCount;
	int mNextEventIdx;
	struct epoll_event *mEventsPtr;
};

#endif
