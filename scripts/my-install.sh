#!/bin/bash


#
# Script to install/upgrade a server package on a machine
#

tarProg="tar"
serverDir=$1
serverBinary="poststation"
mode="install"
tarFile=$2

installServer()
{
    if [ ! -d $tarFile ]
    	then
    	echo "tar file not exist ,exit!"
    	exit -1
    fi
 

    if [ ! -d $serverDir ]; 
	then
	mkdir -p $serverDir
    fi

    if [ ! -d $serverDir ]; 
	then
	echo "Unable to create $serverDir"
	exit -1
    fi

    #bin
    if [ ! -d $serverDir/bin ];then
    	mv $tarFile/bin $serverDir/
	else
		echo "-------backup poststation"
		cp -rf $serverDir/bin/ $serverDir/bin.bak
		echo "-------mv new bin to $serverDir/bin"
		mv $tarFile/bin/* $serverDir/bin/
    fi
    #tools
    echo "-------backup tools"
	mv $serverDir/tools $serverDir/tools.bak
	echo "-------mv new tools"
	mv $tarFile/tools $serverDir/tools
	#scripts
	echo "-------backup scripts"
	if [ -d $serverDir/scripts ];then
		mv $serverDir/scripts $serverDir/scripts.bak
	fi
	echo "-------mv new scripts"
	mv $tarFile/scripts $serverDir/scripts
	#log
    if [ ! -d $serverDir/log ]
    	then 
    	mkdir -p $serverDir/log
    fi
	#etc
    if [ ! -d $serverDir/etc/ ]
    	then
    	mv $tarFile/etc $serverDir/
    fi
    
    
    chmod 755 $serverDir/bin/*

    chmod 755 -R $serverDir/scripts/*   
 
    
 		
    
    #ldconfig
    #if [ -f /etc/init.d/myapp ]
    	#then
    	#mv -f /etc/init.d/mmyapp  /etc/init.d/myapp.bak
    #fi
    cp $serverDir/scripts/* /etc/init.d/
    
    RETVAL=0
    return $RETVAL
}


# Process any command line arguments

case $mode in
    "install")
	installServer
	;;
    *)
	echo "Need to specify mode"
	exit 1
esac


exit $RETVAL
