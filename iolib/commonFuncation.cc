#include <arpa/inet.h>
#include <sys/time.h>
#include <map>
#include <string>
#include <fstream>
#include <list>
#include <stdlib.h>  
#include <time.h>  

#include "commonFuncation.h"


bool gIsBigEndian = true;

/////////////////////////
//common function
inline unsigned IpStringToU32(const char* pAddr)
{
	in_addr in = {0};
	if(inet_pton(AF_INET, pAddr, &in) > 0)
	{
		return in.s_addr;
	}
	return 0;
}
inline unsigned IpStringToU32(const string& strAddr)
{
	return IpStringToU32(strAddr.c_str());
}


inline string IpU32ToString(unsigned ipv4)
{
    char buf[INET_ADDRSTRLEN] = {0};
    struct in_addr in;
    in.s_addr = ipv4;

    if(inet_ntop(AF_INET, &in ,buf, sizeof(buf)))
    {
        return string(buf);
    }
    else
    {
        return string("");
    }

}

bool IsBigEndian(){
    union w
    {
        int a;
        char b;
    } c;
    c.a = 1;
    return (c.b != 1);
} 
unsigned GetIPC(unsigned ip){
	if(gIsBigEndian)
		return ip&0xffffff00;
	else
		return ip&0x00ffffff;
}

unsigned GetIPC(string ip){
   return GetIPC(IpStringToU32(ip));
}

string GetIPCString(string ip){
    unsigned ipc = GetIPC(IpStringToU32(ip));
    return IpU32ToString(ipc);
}





