//---------------------------------------------------------- -*- Mode: C++ -*-
// $Id: log.h 351 2009-05-27 05:40:23Z sriramsrao $
//
// Created 2007/10/17
//
// Copyright 2008 Quantcast Corp.
// Copyright 2007-2008 Kosmix Corp.
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
// \brief A logging facility that uses log4cpp.
//
//----------------------------------------------------------------------------

#ifndef POSTSTATION_LOG_H
#define POSTSTATION_LOG_H

#define LOG4CPP_FIX_ERROR_COLLISION 1
#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>
#include <libgen.h>
#include <sstream>

namespace myframework 
{
    // Have a singleton logger for an application
    class MsgLogger
    {
    private:
        MsgLogger();
        MsgLogger(const MsgLogger &other);
        MsgLogger& operator=(const MsgLogger &other);
        static log4cpp::Category *logger;
    public:
        static log4cpp::Category* GetLogger() { return logger; }
        static void Init(const char *filename,
                         log4cpp::Priority::Value priority =
#ifdef NDEBUG
                         log4cpp::Priority::INFO
#else
                         log4cpp::Priority::DEBUG
#endif        
                         );
        static void SetLevel(log4cpp::Priority::Value priority);
    };

#ifndef THIS_FILE
#define THIS_FILE basename((char *) __FILE__)
#endif

// The following if prevents arguments evaluation (and possible side effect).

#ifndef POSTSTATION_LOG_VA_PRIORITY
#   define POSTSTATION_LOG_VA_PRIORITY(priority, method, msg, ...) \
        if (MsgLogger::GetLogger() && \
                MsgLogger::GetLogger()->isPriorityEnabled(priority)) \
            MsgLogger::GetLogger()->method("(%s:%d) " \
                msg, THIS_FILE, __LINE__, __VA_ARGS__)
#endif

#ifndef POSTSTATION_LOG_PRIORITY
#   define POSTSTATION_LOG_PRIORITY(priority, method, msg) \
        POSTSTATION_LOG_VA_PRIORITY(priority, method, "%s", msg)
#endif

#ifndef POSTSTATION_LOG_DEBUG
#   define POSTSTATION_LOG_DEBUG(msg) \
        POSTSTATION_LOG_PRIORITY(log4cpp::Priority::DEBUG, debug, msg)
#endif
#ifndef POSTSTATION_LOG_VA_DEBUG
#   define POSTSTATION_LOG_VA_DEBUG(msg, ...) \
        POSTSTATION_LOG_VA_PRIORITY(log4cpp::Priority::DEBUG, debug, msg, __VA_ARGS__)
#endif

#ifndef POSTSTATION_LOG_INFO
#   define POSTSTATION_LOG_INFO(msg) \
        POSTSTATION_LOG_PRIORITY(log4cpp::Priority::INFO, info, msg)
#endif
#ifndef POSTSTATION_LOG_VA_INFO
#   define POSTSTATION_LOG_VA_INFO(msg, ...) \
        POSTSTATION_LOG_VA_PRIORITY(log4cpp::Priority::INFO, info, msg, __VA_ARGS__)
#endif

#ifndef POSTSTATION_LOG_WARN
#   define POSTSTATION_LOG_WARN(msg) \
        POSTSTATION_LOG_PRIORITY(log4cpp::Priority::WARN, warn, msg)
#endif
#ifndef POSTSTATION_LOG_VA_WARN
#   define POSTSTATION_LOG_VA_WARN(msg, ...) \
        POSTSTATION_LOG_VA_PRIORITY(log4cpp::Priority::WARN, warn, msg, __VA_ARGS__)
#endif

#ifndef POSTSTATION_LOG_ERROR
#   define POSTSTATION_LOG_ERROR(msg) \
        POSTSTATION_LOG_PRIORITY(log4cpp::Priority::ERROR, error, msg)
#endif
#ifndef POSTSTATION_LOG_VA_ERROR
#   define POSTSTATION_LOG_VA_ERROR(msg, ...) \
        POSTSTATION_LOG_VA_PRIORITY(log4cpp::Priority::ERROR, error, msg, __VA_ARGS__)
#endif

#ifndef POSTSTATION_LOG_FATAL
#   define POSTSTATION_LOG_FATAL(msg) \
        POSTSTATION_LOG_PRIORITY(log4cpp::Priority::FATAL, fatal, msg)
#endif
#ifndef POSTSTATION_LOG_VA_FATAL
#   define POSTSTATION_LOG_VA_FATAL(msg, ...) \
        POSTSTATION_LOG_VA_PRIORITY(log4cpp::Priority::FATAL, fatal, msg, __VA_ARGS__)
#endif

// The following is used instead of log4cpp::CategoryStream. This supports all
// std stream manipulators, has lower # of allocations, and free of possible
// problems with stream object scope / lifetime.
// The price for this is that insertion has to be always terminated with
// MY_LOG_EOM, otherwise you'll get possibly unintelligible compile time error.
#ifndef MY_LOG_STREAM
#   define MY_LOG_STREAM(priority) \
    if (MsgLogger::GetLogger() && \
            MsgLogger::GetLogger()->isPriorityEnabled(priority)) {\
        std::ostringstream _os_015351104260035312; \
        const log4cpp::Priority::Value _priority_015351104260035312(priority); \
        _os_015351104260035312 << "(" << THIS_FILE << ":" << __LINE__ << ") "
#   define MY_LOG_EOM \
        std::flush; \
        MsgLogger::GetLogger()->log(\
            _priority_015351104260035312, _os_015351104260035312.str()); \
    } (void)0
#endif

#ifndef MY_LOG_STREAM_DEBUG
#   define MY_LOG_STREAM_DEBUG MY_LOG_STREAM(log4cpp::Priority::DEBUG)
#endif
#ifndef MY_LOG_STREAM_INFO
#   define MY_LOG_STREAM_INFO  MY_LOG_STREAM(log4cpp::Priority::INFO)
#endif
#ifndef MY_LOG_STREAM_WARN
#   define MY_LOG_STREAM_WARN  MY_LOG_STREAM(log4cpp::Priority::WARN)
#endif
#ifndef MY_LOG_STREAM_ERROR
#   define MY_LOG_STREAM_ERROR MY_LOG_STREAM(log4cpp::Priority::ERROR)
#endif
#ifndef MY_LOG_STREAM_FATAL
#   define MY_LOG_STREAM_FATAL MY_LOG_STREAM(log4cpp::Priority::FATAL)
#endif

}

#endif // COMMON_LOG_H
