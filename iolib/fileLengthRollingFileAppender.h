//---------------------------------------------------------- -*- Mode: C++ -*-
// $Id: FileLengthRollingFileAppender.h 371 2009-10-12 23:09:32Z sriramsrao $
//
// Created 2009/02/05
//
// Copyright 2009 Quantcast Corporation. 
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
// \brief Enhance the rolling file appender to rotate logs on a daily basis.
//----------------------------------------------------------------------------

#ifndef COMMON_FileLengthRollingFileAppender_H
#define COMMON_FileLengthRollingFileAppender_H

#include "log.h"
#include <string>
#include <log4cpp/FileAppender.hh>
//#include "meta/thread.h"


namespace log4cpp
{
    /**
     * Enhance the file appender and roll the log file on each day.
     */
    class LOG4CPP_EXPORT FileLengthRollingFileAppender : public FileAppender {
    public:
        FileLengthRollingFileAppender(const std::string &name, const std::string &fileName,
                                          unsigned int maxDaysToKeep = 30,
                                          bool append = true,
                                          size_t maxFileSize=10485760,//10*1024*1024=10485760 --10MB
                                          mode_t mode = 00644);
        virtual void setMaxDaysToKeep(unsigned int maxDaysToKeep);
        virtual unsigned int getMaxDaysToKeep() const;
        virtual void setmaxFileSize(size_t maxFileSize);
        virtual size_t getmaxFileSize() const;
        virtual void rollOver();
		virtual void deletLog();
		//KFS::MetaThread mDeleteLogThread;
    protected:
        virtual void _append(const log4cpp::LoggingEvent &event);
        unsigned int _maxDaysToKeep;
        size_t _maxFileSize;
        // record the time at which the file that we are currently
        // logging to was created.  In case of a restart, this
        // variable records the last modified time of the file we are
        // currently logging to
        struct tm _logsTime;
        int Start_process;  
    };
}

#endif // COMMON_FileLengthRollingFileAppender_H
