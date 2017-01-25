#!/bin/sh
#SRC_DIR=/data/code/poststation/protobuf
#DST_DIR=/data/code/poststation/protobuf
/usr/local/bin/protoc --proto_path=./ -I=./ --cpp_out=./ ./*.proto

