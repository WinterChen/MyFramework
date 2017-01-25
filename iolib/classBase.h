#ifndef CLASS_BASE_H
#define CLASS_BASE_H


class ProcessorBase{
public:
	ProcessorBase(){}
	virtual ~ProcessorBase(){}
	virtual void MainLoop() = 0;
	virtual void AddConnectionToEpoll(NetConnection *conn) = 0;
	virtual int DeleteFromEpoll(NetConnection *conn) = 0;
};


#endif
