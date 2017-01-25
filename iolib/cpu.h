#ifndef CPU_H
#define CPU_H
#include <vector>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <iostream>
using namespace std;
class CPU{
public:
	CPU();
	~CPU();
	void BindToProcessor(string threadName = "");
	int GetProcessorCount();
	int GetAvailableCPU();
	int SetAffinityToProcessor(int CPUId);
	int GetAffinityToProcessor();
	void ReleaseCPUAffinity();
private:
	int mProcessorCount;
	int *mCPUList;
	pthread_mutex_t mMutex;
};

extern CPU gCPUManager;

#endif
