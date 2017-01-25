#ifndef CALLBACKOBJ_H
#define CALLBACKOBJ_H

class CallbackObj{

public:
	virtual int HandleEvent(int code, void *data) = 0;
	//int HandleRequest(int ,void * data)
};


#endif
