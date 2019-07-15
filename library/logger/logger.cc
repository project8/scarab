/*
 * logger.cxx
 * based on KLogger.cxx from KATRIN's Kasper
 *
 *  Created on: 18.11.2011
 *      Author: Marco Haag <marco.haag@kit.edu>
 */

#define SCARAB_API_EXPORTS


#include <algorithm>
#include <cstring> // for strrchr
#include <iomanip>
#include <iterator>
#include <mutex>
#include <set>
#include <time.h>

#ifndef NDEBUG
#include <thread>
#endif

#ifdef PYTHON_LOGGER
#include "pybind11/pybind11.h"
#endif

#include "logger.hh"

using namespace std;

namespace scarab
{
    const string& EndColor()   {static string* color = new string(COLOR_PREFIX COLOR_NORMAL COLOR_SUFFIX); return *color;}
    const string& FatalColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_RED    COLOR_SUFFIX); return *color;}
    const string& ErrorColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_RED    COLOR_SUFFIX); return *color;}
    const string& WarnColor()  {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_YELLOW COLOR_SUFFIX); return *color;}
    const string& InfoColor()  {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_GREEN  COLOR_SUFFIX); return *color;}
    const string& ProgColor()  {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_BLUE   COLOR_SUFFIX); return *color;}
    const string& DebugColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_CYAN   COLOR_SUFFIX); return *color;}
    const string& TraceColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_WHITE  COLOR_SUFFIX); return *color;}
    const string& OtherColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_WHITE  COLOR_SUFFIX); return *color;}

    struct logger::Private
    {
            static std::mutex sMutex;

            typedef std::set< logger* > LoggerSet;
            static LoggerSet* AllLoggers()
            {
                static LoggerSet* sAllLoggers = new LoggerSet();
                return sAllLoggers;
            }

            static char sDateTimeFormat[16];
			static time_t sRawTime;
            static tm* sProcessedTime;
            static char sTimeBuff[512];
            static size_t getTimeAbsoluteStr()
            {
                time(&logger::Private::sRawTime);

				sProcessedTime = localtime(&logger::Private::sRawTime);
				return strftime(logger::Private::sTimeBuff, 512,
					logger::Private::sDateTimeFormat,
					logger::Private::sProcessedTime);
            }


            const char* fLogger;
            static bool fColored;
            ELevel fThreshold;

            static std::ostream* fOut;
            static std::ostream* fErr;

            static const char* level2Str(ELevel level)
            {
                switch(level)
                {
                    case eTrace : return "TRACE"; break;
                    case eDebug : return "DEBUG"; break;
                    case eInfo  : return "INFO"; break;
                    case eProg  : return "PROG"; break;
                    case eWarn  : return "WARN"; break;
                    case eError : return "ERROR"; break;
                    case eFatal : return "FATAL"; break;
                    default     : return "XXX";
                }
            }

            static string level2Color(ELevel level)
            {
                switch(level)
                {
                    case eTrace : return TraceColor(); break;
                    case eDebug : return DebugColor(); break;
                    case eInfo  : return InfoColor(); break;
                    case eProg  : return ProgColor(); break;
                    case eWarn  : return WarnColor(); break;
                    case eError : return ErrorColor(); break;
                    case eFatal : return FatalColor(); break;
                    default     : return OtherColor();
                }
            }


            void logCout(ELevel level, const string& message, const Location& loc)
            {
                logger::Private::sMutex.lock();
                logger::Private::getTimeAbsoluteStr();
#ifdef PYTHON_LOGGER
                std::stringstream sStream;
                fOut = &sStream;
#endif
                if (fColored)
                {
                    //cout << color << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << skKTEndColor << endl;
                    (*fOut) << Private::level2Color(level) << logger::Private::sTimeBuff << " [" << setw(5) << Private::level2Str(level) << "] ";
#ifndef NDEBUG
                    (*fOut) << "(tid " << std::this_thread::get_id() << ") ";
#endif
                    copy(loc.fFileName.end() - std::min< int >(loc.fFileName.size(), 16), loc.fFileName.end(), ostream_iterator<char>(*fOut));
                    (*fOut) << "(" << loc.fLineNumber  << "): ";
                    (*fOut) << message << EndColor() << endl;
                }
                else
                {
                    //cout << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << endl;
                    (*fOut) << logger::Private::sTimeBuff << " [" << setw(5) << Private::level2Str(level) << "] ";
#ifndef NDEBUG
                    (*fOut) << "(tid " << std::this_thread::get_id() << ") ";
#endif
                    copy(loc.fFileName.end() - std::min< int >(loc.fFileName.size(), 16), loc.fFileName.end(), ostream_iterator<char>(*fOut));
                    (*fOut) << "(" << loc.fLineNumber  << "): ";
                    (*fOut) << message << endl;
                }
#ifdef PYTHON_LOGGER
                pybind11::print(sStream.str());
#endif
                logger::Private::sMutex.unlock();
            }

            void logCerr(ELevel level, const string& message, const Location& loc)
            {
                logger::Private::sMutex.lock();
                logger::Private::getTimeAbsoluteStr();
#ifdef PYTHON_LOGGER
                std::stringstream sStream;
                fErr = &sStream;
#endif
                if (fColored)
                {
                    //cout << color << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << skKTEndColor << endl;
                    (*fErr) << Private::level2Color(level) << logger::Private::sTimeBuff << " [" << setw(5) << Private::level2Str(level) << "] ";
#ifndef NDEBUG
                    (*fErr) << "(tid " << std::this_thread::get_id() << ") ";
#endif
                    copy(loc.fFileName.end() - std::min< int >(loc.fFileName.size(), 16), loc.fFileName.end(), ostream_iterator<char>(*fErr));
                    (*fErr) << "(" << loc.fLineNumber  << "): ";
                    (*fErr) << message << EndColor() << endl;
                }
                else
                {
                    //cout << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << endl;
                    (*fErr) << logger::Private::sTimeBuff << " [" << setw(5) << Private::level2Str(level) << "] ";
#ifndef NDEBUG
                    (*fErr) << "(tid " << std::this_thread::get_id() << ") ";
#endif
                    copy(loc.fFileName.end() - std::min< int >(loc.fFileName.size(), 16), loc.fFileName.end(), ostream_iterator<char>(*fErr));
                    (*fErr) << "(" << loc.fLineNumber  << "): ";
                    (*fErr) << message << endl;
                }
#ifdef PYTHON_LOGGER
                pybind11::print(sStream.str());
#endif
                logger::Private::sMutex.unlock();
            }
    };

    mutex logger::Private::sMutex;

    char logger::Private::sDateTimeFormat[16];
	time_t logger::Private::sRawTime;
	tm* logger::Private::sProcessedTime;
	char logger::Private::sTimeBuff[512];

    bool logger::Private::fColored = true;

    std::ostream* logger::Private::fOut = &cout;
    std::ostream* logger::Private::fErr = &cerr;


    logger::logger(const char* name) : fPrivate(new Private())
    {
        if (name == 0)
        {
            fPrivate->fLogger = "root";
        }
        else
        {
            const char* logName = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
            fPrivate->fLogger = logName;
        }
#ifndef _WIN32
        fPrivate->fColored = true;
#else
        fPrivate->fColored = false;
#endif
        sprintf(logger::Private::sDateTimeFormat,  "%%Y-%%m-%%d %%T");
        SetLevel(eDebug);
        logger::Private::AllLoggers()->insert(this);
    }

    logger::logger(const std::string& name) : fPrivate(new Private())
    {
        fPrivate->fLogger = name.c_str();
#ifndef _WIN32
        fPrivate->fColored = true;
#else
        fPrivate->fColored = false;
#endif
        sprintf(logger::Private::sDateTimeFormat, "%%Y-%%m-%%d %%T");
		SetLevel(eDebug);
		logger::Private::AllLoggers()->insert(this);
    }

    logger::~logger()
    {
        delete fPrivate;
    }

    bool logger::IsLevelEnabled(ELevel level) const
    {
        return level >= fPrivate->fThreshold;
    }

    void logger::SetLevel(ELevel level) const
    {
#if defined(NDEBUG) && defined(STANDARD)
                fPrivate->fThreshold = level >= eInfo ? level : eInfo;
#elif defined(NDEBUG)
                fPrivate->fThreshold = level >= eProg ? level : eProg;
#else
                fPrivate->fThreshold = level;
#endif
    }

    void logger::SetGlobalLevel(ELevel level) const
    {
        for( std::set< logger* >::iterator logIt = logger::Private::AllLoggers()->begin(); logIt != logger::Private::AllLoggers()->end(); ++logIt)
        {
            (*logIt)->SetLevel(level);
        }
        return;
    }

    void logger::SetColored(bool flag)
    {
#ifndef _WIN32
        logger::Private::fColored = flag;
#else
        std::cerr << "Colored logging is not enabled in Windows" << std::endl;
#endif
        return;
    }

    void logger::SetOutStream(std::ostream* stream)
    {
        logger::Private::fOut = stream;
        return;
    }

    void logger::SetErrStream(std::ostream* stream)
    {
        logger::Private::fErr = stream;
        return;
    }

    void logger::Log(ELevel level, const string& message, const Location& loc)
    {
        if (level >= eWarn)
        {
            fPrivate->logCerr(level, message, loc);
        }
        else
        {
            fPrivate->logCout(level, message, loc);
        }
    }
}
