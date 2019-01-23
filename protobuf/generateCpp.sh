#!/bin/sh
/usr/local/bin/protoc --proto_path=./ -I=./ --cpp_out=./ ./*.proto

