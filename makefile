CFLAGS+=-g -Wall 
INC+= -I. -I./src/. -I./iolib/. -I./protobuf/.  
#INC+= -I/data/code/mysql_api/include/
LIB+= /usr/local/lib/libprotobuf.a
LIB+= /usr/local/lib/liblog4cpp.a
#LIB+= /data/code/mysql_api/lib/libmysqlcppconn-static.a
LIB+= -lpthread -ldl
vpath %.cc  src iolib protobuf 
OBJ+= main.o\
	  myserver.o\
	  clientSM.o\
	  eventManager.o\
	  processorManager.o\
	  processor.o\
	  config.o\
	  cpu.o\
	  counter.o\
	  tcpSocket.o\
	  mutex.o\
	  fdEpoll.o\
	  threadPool.o\
	  tcpSocket.o\
	  mutex.o\
	  netKicker.o\
	  acceptor.o\
	  netConnection.o\
	  bufferPool.o\
	  ioBuffer.o\
	  log.o\
	  commonFuncation.o\
	  fileLengthRollingFileAppender.o\
	  
	  
	  
	
TARGET=myframework


#############################################################
$(TARGET):$(OBJ)
	g++ $(CFLAGS) -o $@ $^ $(LIB) 
	rm -f *.o

%.o: %.cpp
	g++ $(CFLAGS) $(INC) -c -o $@ $< 
%.o: %.cc
	g++ $(CFLAGS) $(INC) -c -o $@ $< 

clean:
	rm -f *.o
	rm -f $(TARGET) 

