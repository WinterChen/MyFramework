#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "acceptor.h"
#include "callbackObj.h"
#include "clientSM.h"
#include "log.h"
using namespace myframework; 



class ClientManager : public IAcceptorOwner {
public:
    ClientManager() {
        mAcceptor = NULL;
    };
    virtual ~ClientManager() {
		if(mAcceptor)
        	delete mAcceptor;
    };
    void StartAcceptor(unsigned short port) {
		MY_LOG_STREAM_DEBUG << "start listen " << port << MY_LOG_EOM;
        mAcceptor = new Acceptor(port, this, NULL);
    };
	void SetAcceptorPort(unsigned short port){mPort = port;}
	unsigned short GetPort(){return mPort;}
    CallbackObj *CreateKfsCallbackObj(NetConnection *conn) {
        // XXX: Should we keep a list of all client state machines?
        return new ClientSM(conn);
    }
private:
    // The socket object which is setup to accept connections.
    Acceptor *mAcceptor;
	unsigned short mPort;
};



#endif
