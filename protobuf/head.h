#ifndef PROTO_HEAD
#define PROTO_HEAD


#define DEFAULT_CMD   0










#define PARSE_REQUEST_ERROR -30001
#define PARSE_RESPONSE_ERROR -30002
#define CMD_ERROR -30003
#define HEAD_LEN_ERROR -30004
#define BODY_LEN_ERROR -30005



struct Head{
	unsigned short mCmd;
	//char mVersion;
	short mErrorNo;
	unsigned mSeq;
	unsigned mBodyLen;
	Head():mCmd(DEFAULT_CMD),mErrorNo(0),mSeq(0),mBodyLen(0){}
	Head(unsigned short cmd, unsigned short error, unsigned seq, unsigned len):mCmd(cmd),mErrorNo(error),mSeq(seq),mBodyLen(len){}
};
#endif

