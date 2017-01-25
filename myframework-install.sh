#!/bin/bash


#
# Script to install/upgrade a server package on a machine
#

tarProg="tar"
serverDir="/usr/local/myframework"
serverBinary="myframework"
mode="install"
tarFile=$1

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
    if [ ! -d $serverDir/bin/ ];then
    	mv $tarFile/bin $serverDir/
	else
		mv $serverDir/bin/myframework $serverDir/bin/myframework.bak
		mv $tarFile/bin/myframework $serverDir/bin/
    fi
    #tools
	mv $server/tools $server/tools.bak
	mv $tarFile/tools $server/tools
	#scripts
	mv $server/scripts $server/scripts.bak
	mv $tarFile/scripts $server/scripts
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
    if [ ! -f $serverDir/etc/ipdb ]
    	then
    	tar -zvf $serverDir/etc/ipdb.tar.gz $serverDir/etc/
    fi
    
    chmod 755 $serverDir/bin/postsattion

    chmod 755 -R $serverDir/scripts/*   
 
    #cp $serverDir/scripts/myframework /etc/init.d/
 		
    
    ldconfig
    if [ -f /etc/init.d/myframework ]
    	then
    	mv -f /etc/init.d/myframework  /etc/init.d/myframework.bak
    fi
    cp $serverDir/scripts/myframework /etc/init.d/
    
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
