#include <stdlib.h>
#include <signal.h>

#include "myserver.h"






int main(int argc, char **argv){
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " <config file> " << endl;
		exit(0);
	}
	
	signal(SIGPIPE, SIG_IGN);
	
	int res = gMyServer.Init(argv[1]);
	if(res == -1)
		exit(0);
	gMyServer.Start();
	return 0;
}
