//---------------------------------------------------------- -*- Mode: C++ -*-
// $Id: Acceptor.cc 351 2009-05-27 05:40:23Z sriramsrao $
//
// Created 2006/03/23
// Author: Sriram Rao
//
// Copyright 2008 Quantcast Corp.
// Copyright 2006-2008 Kosmix Corp.
//
// This file is part of Kosmos File System (KFS).
//
// Licensed under the Apache License, Version 2.0
// (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language governing
// permissions and limitations under the License.
//
// 
// modified 2013/06/22
// Author: Winter Chen
//----------------------------------------------------------------------------

#include "acceptor.h"
#include "eventManager.h"
#include "log.h"
using namespace myframework; 

///
/// Create a TCP socket, bind it to the port, and listen for incoming connections.
///
Acceptor::Acceptor(int port, IAcceptorOwner *owner, ProcessorBase *processor):mAcceptorOwner(owner), mProcessor(processor){
	TcpSocket *sock;
    int res;
    sock = new TcpSocket();
	if(mProcessor != NULL)
    	res = sock->Listen(port, true);//multiple processors
    else
		res = sock->Listen(port);
    mConn = new NetConnection(sock, this, true);
    
    if (res < 0) {
        MY_LOG_STREAM_ERROR << "Unable to bind to port: "<< port << " error:" <<  res << MY_LOG_EOM;
        return;
    }

    if(mProcessor != NULL)
    	mProcessor->AddConnectionToEpoll(mConn);
}

Acceptor::~Acceptor()
{
	if(mConn){
		mConn->Close();
		delete mConn;

	}
    //mConn.reset();
}

///
/// Event handler that gets called back whenever a new connection is
/// received.  In response, the AcceptorOwner object is first notified of
/// the new connection and then, the new connection is added to the
/// list of connections owned by the NetManager. @see NetManager 
///
int
Acceptor::RecvConnection(int code, void *data)
{
    /*if (! data || code != EVENT_NEW_CONNECTION) {
        assert(code == EVENT_NET_ERROR || code == EVENT_INACTIVITY_TIMEOUT);
        return 0;
    }*/
    
	MY_LOG_STREAM_DEBUG << "callback RecvConnection..." << MY_LOG_EOM;
    if(data == NULL){
		MY_LOG_STREAM_ERROR << "data is null, return" << MY_LOG_EOM;
		return 0;
	}

    NetConnection *conn = (NetConnection *) data;
    CallbackObj *callbackObj;

    callbackObj = mAcceptorOwner->CreateKfsCallbackObj(conn);//callback is ClientSM
    conn->SetCallbackObj(callbackObj);

    ///
    /// Add the connection to the net manager's list of "polling"
    /// fd's. 
    ///
	//gEventManager.DispatchConnectionToProcessor(conn);
	mProcessor->AddConnectionToEpoll(conn);
    return 0;
}


/*int
Listener::RecvConnection(int code, void *data)
{
	MY_LOG_STREAM_DEBUG << "callback RecvConnection..." << MY_LOG_EOM;
    if(data == NULL){
		MY_LOG_STREAM_ERROR << "data is null, return" << MY_LOG_EOM;
		return 0;
	}

    NetConnection *conn = (NetConnection *) data;
    CallbackObj *callbackObj;

    callbackObj = mAcceptorOwner->CreateKfsCallbackObj(conn);//callback is ClientSM
    conn->SetCallbackObj(callbackObj);

    if(mProcessorOwner != NULL)
		mProcessorOwner.AddConnectionToEpoll(conn);
    return 0;
}*/

