//---------------------------------------------------------- -*- Mode: C++ -*-
// $Id: FileLengthRollingFileAppender.cc 371 2009-10-12 23:09:32Z sriramsrao $
//
// Created 2009/02/05
//
// This file is part of Kosmos File System (KFS).
//
// Licensed under the Apache License, Version 2.0
// (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language governing
// permissions and limitations under the License.
// 
// modified 2013/06/22
// Author: Winter Chen
//----------------------------------------------------------------------------


#include "fileLengthRollingFileAppender.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <iomanip>
//#include "meta/thread.h"

#include <sys/statvfs.h>
#if defined(__APPLE__) || defined(__sun__) || (!defined(__i386__))
typedef struct statvfs StatVfs;
 int GetVfsStat(const char* path, StatVfs* stat) {
    return statvfs(path, stat);
}
#else
typedef struct statvfs64 StatVfs;
 int GetVfsStat(const char* path, StatVfs* stat) {
    return statvfs64(path, stat);
}
#endif

extern "C" {
#include <unistd.h>
};

using std::string;

	
using namespace myframework;
namespace log4cpp {


static void *
DeletLogLoop(void *arg)
{
	
	FileLengthRollingFileAppender *appender= (FileLengthRollingFileAppender *)arg;
	while(1){
		appender->deletLog();
		sleep(300);
	}
}

void FileLengthRollingFileAppender::deletLog()
{
	std::ostringstream filename_stream;
	string lastFn, dirname;
	
	StatVfs buff_DiskSpace;
	long int  DiskSpaceFree=0;
	long int limitedDiskSpace=500;//MB
		
	struct dirent **entries;
	int nentries, res;
	string::size_type slash = _fileName.rfind('/');
	// prune away old files
    if (slash == string::npos)
        dirname = ".";
    else
        dirname.assign(_fileName, 0, slash);
	
	GetVfsStat(dirname.c_str(),&buff_DiskSpace);
	DiskSpaceFree =( (buff_DiskSpace.f_bavail * buff_DiskSpace.f_frsize)/1024)/1024 ;//Mbyte
	
	nentries = scandir(dirname.c_str(), &entries, 0, alphasort);
	
	if (nentries < 0)
		return;
	for (int i = 0; i < nentries; i++) {
		struct stat statBuf;
		string stat_log_filename = dirname + "/" + entries[i]->d_name;//must include Dir!
		res = ::stat(stat_log_filename.c_str(), &statBuf);	  
		if ((res < 0) || (!S_ISREG(statBuf.st_mode))) {
			free(entries[i]);
			continue;
		}

		//delete old files if disk space is small!	 Delete Old File
		if ( DiskSpaceFree < limitedDiskSpace ) {
			string fname = dirname + "/" + entries[i]->d_name;
			slash = fname.rfind("chunkserver.log-"); 
			//file name include "chunkserver.log-"		  
			if (slash != string::npos) {			
				::unlink(fname.c_str());
			}
	
			slash = fname.rfind("metaserver.log-"); 
			//file name include "metaserver.log-"		 
			if (slash != string::npos) {			
					::unlink(fname.c_str());
			}	  
		}	
		free(entries[i]);
	}
	free(entries);
}
FileLengthRollingFileAppender::FileLengthRollingFileAppender(const string &name, const string &fileName,
                                                   unsigned int maxDaysToKeep,
                                                   bool append, 
                                                   size_t maxFileSize,
                                                   mode_t mode) :
    log4cpp::FileAppender(name, fileName, append, mode),
    _maxDaysToKeep(maxDaysToKeep > 0 ? maxDaysToKeep : 7),
    _maxFileSize(maxFileSize > 0 ? maxFileSize : 1024*1024*10)
{
    struct stat statBuf;
    int res;
    time_t t;
    Start_process = 1;
    
    // if the file exists, determine when it was last modified so we know what to rename the file to
    res = ::stat(fileName.c_str(), &statBuf);
    if (res < 0) {
        t = time(NULL);
    } else {
        t = statBuf.st_mtime;
    }
    localtime_r(&t, &_logsTime);

	//mDeleteLogThread.start(DeletLogLoop, this);
}

void FileLengthRollingFileAppender::setMaxDaysToKeep(unsigned int maxDaysToKeep)
{
    _maxDaysToKeep = maxDaysToKeep;
}

unsigned int FileLengthRollingFileAppender::getMaxDaysToKeep() const
{
    return _maxDaysToKeep;
}

void FileLengthRollingFileAppender::setmaxFileSize(size_t maxFileSize)
{
    _maxFileSize = maxFileSize;
}

size_t FileLengthRollingFileAppender::getmaxFileSize() const
{
    return _maxFileSize;
}

void FileLengthRollingFileAppender::rollOver()
{
    std::ostringstream filename_stream;
    string lastFn,dirname;
    ::close(_fd);
    	
    filename_stream << _fileName << "-" << _logsTime.tm_year + 1900 << "-" 
                    << std::setfill('0') << std::setw(2) << _logsTime.tm_mon + 1 << "-" 
                    << std::setfill('0')<<std::setw(2) << _logsTime.tm_mday<< "_" <<std::setfill('0')<<std::setw(2)<< _logsTime.tm_hour
                    << ":"<< std::setfill('0')<<std::setw(2)<< _logsTime.tm_min<< ":"<< std::setfill('0')<<std::setw(2)  
                    << _logsTime.tm_sec << std::ends;

    lastFn = filename_stream.str();
    ::rename(_fileName.c_str(), lastFn.c_str());//
    _fd = ::open(_fileName.c_str(), _flags, _mode);

}


void FileLengthRollingFileAppender::_append(const log4cpp::LoggingEvent &event)
{
    struct tm now;
    time_t t = time(NULL);

    if(Start_process == 1) {
        Start_process=0;
    	localtime_r(&t, &now) ;
      	_logsTime = now;
      	rollOver(); 
    }
    off_t offset = ::lseek(_fd, 0, SEEK_END);
    if(offset < 0) {
    } else {
        if(static_cast<size_t>(offset) >= _maxFileSize) {
            localtime_r(&t, &now) ;
    	    _logsTime = now;
    	    rollOver();
    	}
    }
    log4cpp::FileAppender::_append(event);
}

}

