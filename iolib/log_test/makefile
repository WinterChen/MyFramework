CFLAGS+=-g -Wall 
INC+= -I. -I../
LIB+= /usr/local/lib/liblog4cpp.a
LIB+=-lpthread  
vpath %.cc  ../protobuf  ../

OBJ+= logTest.o\
	  log.o\
	  fileLengthRollingFileAppender.o\
	  
	
TARGET=log_test
#############################################################
$(TARGET):$(OBJ)
	g++ $(CFLAGS) -o $@ $^ $(LIB) 

%.o: %.cpp
	g++ $(CFLAGS) $(INC) -c -o $@ $< 	
%.o: %.cc
	g++ $(CFLAGS) $(INC) -c -o $@ $< 	 

clean:
	rm -f *.o
	rm -f $(TARGET) 

