#include "log.h"

using namespace std;
using namespace myframework; 


int main(){

	myframework::MsgLogger::Init("/usr/local/myframework/log/test.log");
	MY_LOG_STREAM_DEBUG << "winter chen test!" << MY_LOG_EOM;
	return 0;
}
