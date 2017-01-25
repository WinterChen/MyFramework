#include <arpa/inet.h>
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <fstream>
#include <list>
#include <stdlib.h>  
#include <time.h>  
#include <algorithm>


#include "myserver.h"
#include "eventManager.h"
#include "log.h"
#include "head.h"


using namespace std;
using namespace myframework; 

MyServer gMyServer;


	
int MyServer::Init(string configFile){
	
	mConfig.InitConfig(configFile);
	
	cout << "config file:" << configFile << endl;
	string logFile = mConfig.GetValue("log", "");
	if(logFile == ""){
		cout  << "not config log file! exit!" << endl;
		return -1;
	}
	
	myframework::MsgLogger::Init(logFile.c_str());
	string logLevel = mConfig.GetValue("logLevel", "");
	if (logLevel == "INFO") {
		
		myframework::MsgLogger::SetLevel(log4cpp::Priority::INFO);
	} 
	else if (logLevel == "ERROR") {
		myframework::MsgLogger::SetLevel(log4cpp::Priority::ERROR);
	} 
	else if (logLevel == "FATAL") {
		myframework::MsgLogger::SetLevel(log4cpp::Priority::FATAL);
	} 
	else {
		myframework::MsgLogger::SetLevel(log4cpp::Priority::DEBUG);
	}
	MY_LOG_STREAM_INFO << "-------------------config info------------------" << MY_LOG_EOM;
	MY_LOG_STREAM_INFO << "log:" << logFile  << ", level:" << logLevel << MY_LOG_EOM;
	mPort = -1;
	mPort = mConfig.GetValue("port", mPort);
	if(mPort == -1){
		MY_LOG_STREAM_ERROR << "not config port! exit!" << MY_LOG_EOM;
		return -1;
	}	
	MY_LOG_STREAM_INFO << "port:" << mPort << MY_LOG_EOM;
	mProcessorCount = -1;
	mProcessorCount = mConfig.GetValue("processorCount", mProcessorCount);
	if(mProcessorCount == -1){
		MY_LOG_STREAM_WARN<< "not config the count of processor, use default value 2." << MY_LOG_EOM;
		mProcessorCount = 2;
	}
	MY_LOG_STREAM_INFO << "processor count:" << mProcessorCount << MY_LOG_EOM;

	//TODO: parse other things
	return 0;

}


int MyServer::Start(){
	//int and start the EventManager
	MY_LOG_STREAM_DEBUG << "myframework start..." << MY_LOG_EOM;
	
	mClientManager.SetAcceptorPort(mPort);
	mProcessorManager.Init(mProcessorCount, &mClientManager);
	mProcessorManager.MainLoop();
	
	return 0;
}

