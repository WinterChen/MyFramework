#include "cpu.h"

CPU gCPUManager;

CPU::CPU():mProcessorCount(sysconf(_SC_NPROCESSORS_CONF)){
	if(mProcessorCount > 1){
	 	mCPUList = new int[mProcessorCount];
	    for(int i = 0; i < mProcessorCount; i++)
		    mCPUList[i] = 0;
	}
	pthread_mutex_init(&mMutex, NULL);
}

CPU::~CPU(){
	if(mProcessorCount > 1){
		if(mCPUList != NULL){
	 		delete [] mCPUList;
			mCPUList = NULL;
		}
	}
}


int
CPU::GetProcessorCount(){
    return mProcessorCount;
}

int 
CPU::GetAvailableCPU(){
	
	//return mCPUList[0];
	if(mProcessorCount == 1){
       return -1;
	}
	int availableCPUId = 0;
	int min = 1024;//此处应为系统允许的最大进程数除以mProcessorCount
	for(int i = 0; i < mProcessorCount; i++){
		if(mCPUList[i] == 0){
			availableCPUId = i;
			break;
		}
		if(mCPUList[i] < min){
			min = mCPUList[i];
			availableCPUId = i;
		}
	}
	//mCPUList[availableCPUId]++;
	return availableCPUId;
}

int
CPU::SetAffinityToProcessor(int CPUId){
    cpu_set_t mask;
	CPU_ZERO(&mask);
    CPU_SET(CPUId, &mask);
	int res = sched_setaffinity(0, sizeof(mask), &mask);
	if (res == -1){
		return -1;
	}
	mCPUList[CPUId]++;
	return res;
}

int
CPU::GetAffinityToProcessor(){
	cpu_set_t get;
	CPU_ZERO(&get);
	int res = sched_getaffinity(0, sizeof(get), &get);
	if (res == -1)
    {
         return -1;
    }
	int affiinityCPU = 0;
	while(affiinityCPU < mProcessorCount){
		if (CPU_ISSET(affiinityCPU, &get))
			break;
		affiinityCPU++;
	}
	if(affiinityCPU == mProcessorCount)
		return -1;
	return affiinityCPU;
    
}


void 
CPU::ReleaseCPUAffinity(){
	if(mProcessorCount == 1){
       return ;
	}
    int CPUId = GetAffinityToProcessor();
	if(CPUId == -1){
		return;
	}	
	mCPUList[CPUId]--;
}


void
CPU::BindToProcessor(string threadName){
	pthread_mutex_lock(&mMutex);
	int availableCPUId = GetAvailableCPU();
	if(availableCPUId == -1){
		pthread_mutex_unlock(&mMutex);
		return;
	}
	int res = SetAffinityToProcessor(availableCPUId);
	pthread_mutex_unlock(&mMutex);

	if(res == -1)
		cout << "can not set affinity to thread!" << endl;
	cout << "thread " << threadName << "  " << pthread_self() << " bind to cpu: " << availableCPUId << endl;
}




//for test
/*
void* ThreadFuncation(void * arg){
	gCPUManager.BindToProcessor();
	sleep(5);
	cout << "thread " << pthread_self() <<" exit, realse CPU..." << endl;
	gCPUManager.ReleaseCPUAffinity();
}
int main(int argc, char **argv){
	if(argc < 2){
		cout << "usage: BindCpu <thread num>" << endl;
		return -1;
	}
	int threadNum = atoi(argv[1]);
	if(threadNum < 1){
		cout << "thread num < 1" << endl;
		return -1;
	}
	int res;
	pthread_t pFd;
	for(int i = 0; i < threadNum; i++){
		res = pthread_create(&pFd, 0, ThreadFuncation, 0);
		if(res != 0){
			cout << "create thread error" << endl;
		}
	}
	while(1)
		sleep(20);
	return 0;

}
*/

