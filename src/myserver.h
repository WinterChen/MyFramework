#ifndef MYSERVER_H
#define MYSERVER_H
#include <vector>
#include <fstream>
#if(__cplusplus == 201103L)
#include <unordered_map>
#else
#include <tr1/unordered_map>
namespace std
{
    using std::tr1::unordered_map;
}
#endif

#include "clientManager.h"
#include "config.h"
#include "processorManager.h"
#include "mutex.h"
#include "timeout.h"


using namespace std;

class MyServer{
public:
	MyServer():mPort(-1), mProcessorCount(-1){}
	~MyServer(){}
	int Init(string configfile);
	int Start();
	
private:
	ClientManager mClientManager;
	short mPort;
	short mProcessorCount;
	
	Config mConfig;
	ProcessorManager mProcessorManager;

	
};
extern MyServer gMyServer;

unsigned IpStringToU32(const string& strAddr);


#endif
