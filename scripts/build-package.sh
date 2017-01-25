#!/bin/sh 

if [ $# != 2 ] ; then 
echo "USAGE: $0 INSTALL_DIR VERSION" 
echo " e.g.: $0 /usr/local/myapp/ 1.0.0" 
exit 1; 
fi 

APPNAME=$1
INSTALL_DIR=$2
VERSION=$3
TARGET_DIR=$INSTALL_DIR/$APPNAME_$VERSION
mkdir $TARGET_DIR
mkdir $TARGET_DIR/bin
mkdir $TARGET_DIR/tools
mkdir $TARGET_DIR/scripts
mkdir $TARGET_DIR/etc
cp ../$APPNAME $TARGET_DIR/bin
cp ../etc/* $TARGET_DIR/etc
cp my-install.sh $TARGET_DIR/scripts
cp app-ctrl-scripts $TARGET_DIR/scripts/$APPNAME
#cp ../test/$APPNAME_test $TARGET_DIR/tools


cd $INSTALL_DIR && mv $APPNAME_$VERSION/scripts/my-install.sh . && tar -zcf  $APPNAME_$VERSION.tar.gz $APPNAME_$VERSION my-install.sh
rm -r $TARGET_DIR
