#ifndef CLIENTSM_H
#define CLIENTSM_H

//#include "request.h"
#include "callbackObj.h"
#include "netConnection.h"
//#include "myframework.pb.h"
//#include "myframework.pb.helper.h"
//using namespace google::protobuf;//::Message

class ClientSM : public CallbackObj {
public:

    ClientSM(NetConnection *conn);

    ~ClientSM(); 

    int HandleEvent(int code, void *data){return HandleRequest(code, data);}
	int HandleRequest(int code,void * data);
    //int HandleTerminate(int code, void *data);
    
	
private:
    /// A handle to a network connection
    NetConnection	*mNetConnection;


	/// log out client ip addr. for debugging purposes
	std::string	mClientIP;
	//google::protobuf::Message *mRequest;
};


#endif
