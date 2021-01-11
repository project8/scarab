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
        static unsigned& count()
        {
            static unsigned sCount = 0;
            return sCount;
        }

        static std::mutex sMutex;

        typedef std::set< logger* > LoggerSet;
        static std::shared_ptr<LoggerSet> AllLoggers()
        {
            // construct-on-first-use strategy to avoid static initialization fiasco
            static std::shared_ptr<LoggerSet> sAllLoggers = std::make_shared< LoggerSet >();
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

        static std::ostream* fOut;
        static std::ostream* fErr;

        ELevel fThreshold;
        bool fThresholdIsGlobal;

        static ELevel& GlobalThreshold()
        {
            // construct-on-first-use strategy to avoid static initialization fiasco
            static ELevel sGlobalThreshold = logger::Private::filterMinimumLevel(ELevel::eDebug);
            return sGlobalThreshold;
        }

        static const char* level2Str(ELevel level)
        {
            switch(level)
            {
                case ELevel::eTrace : return "TRACE"; break;
                case ELevel::eDebug : return "DEBUG"; break;
                case ELevel::eInfo  : return "INFO"; break;
                case ELevel::eProg  : return "PROG"; break;
                case ELevel::eWarn  : return "WARN"; break;
                case ELevel::eError : return "ERROR"; break;
                case ELevel::eFatal : return "FATAL"; break;
                default     : return "XXX";
            }
        }

        static string level2Color(ELevel level)
        {
            switch(level)
            {
                case ELevel::eTrace : return TraceColor(); break;
                case ELevel::eDebug : return DebugColor(); break;
                case ELevel::eInfo  : return InfoColor(); break;
                case ELevel::eProg  : return ProgColor(); break;
                case ELevel::eWarn  : return WarnColor(); break;
                case ELevel::eError : return ErrorColor(); break;
                case ELevel::eFatal : return FatalColor(); break;
                default     : return OtherColor();
            }
        }

        static ELevel filterMinimumLevel(ELevel level)
        {
#if defined(NDEBUG) && defined(STANDARD)
            return level >= ELevel::eInfo ? level : ELevel::eInfo;
#elif defined(NDEBUG)
            return level >= ELevel::eProg ? level : ELevel::eProg;
#else
            return level;
#endif
        }


        void logCout(ELevel level, const string& message, const Location& loc)
        {
            logger::Private::sMutex.lock();
            logger::Private::getTimeAbsoluteStr();
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
            logger::Private::sMutex.unlock();
        }

        void logCerr(ELevel level, const string& message, const Location& loc)
        {
            logger::Private::sMutex.lock();
            logger::Private::getTimeAbsoluteStr();
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
        fPrivate->count()++;

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
        UseGlobalLevel();
        logger::Private::AllLoggers()->insert(this);

        std::cerr << "created logger (" << fPrivate->count() << ") " << fPrivate->fLogger << std::endl;
    }

    logger::logger(const std::string& name) : fPrivate(new Private())
    {
        fPrivate->count()++;
        
        fPrivate->fLogger = name.c_str();
#ifndef _WIN32
        fPrivate->fColored = true;
#else
        fPrivate->fColored = false;
#endif
        sprintf(logger::Private::sDateTimeFormat, "%%Y-%%m-%%d %%T");
		UseGlobalLevel();
		logger::Private::AllLoggers()->insert(this);

        std::cerr << "created logger (" << fPrivate->count() << ") " << fPrivate->fLogger << std::endl;
    }

    logger::~logger()
    {
        fPrivate->count()--;
        
        std::cerr << "destructing logger (" << fPrivate->count() << ") " << fPrivate->fLogger << std::endl;
        logger::Private::AllLoggers()->erase(this);
        std::cerr << "logger removed from set: " << fPrivate->fLogger << std::endl;
    }

    bool logger::IsLevelEnabled(ELevel level) const
    {
        return level >= fPrivate->fThreshold;
    }

    logger::ELevel logger::GetLevel() const
    {
        return fPrivate->fThreshold;
    }

    void logger::SetLevel(ELevel level) const
    {
        fPrivate->fThreshold = logger::Private::filterMinimumLevel(level);
        fPrivate->fThresholdIsGlobal = false;
    }

    void logger::UseGlobalLevel() const
    {
        fPrivate->fThreshold = logger::Private::filterMinimumLevel(logger::Private::GlobalThreshold());
        fPrivate->fThresholdIsGlobal = true;
    }

    // static function
    logger::ELevel logger::GetGlobalLevel()
    {
        return logger::Private::GlobalThreshold();
    }

    // static function
    void logger::SetGlobalLevel(ELevel level)
    {
        logger::Private::GlobalThreshold() = logger::Private::filterMinimumLevel(level);
        for( logger::Private::LoggerSet::iterator logIt = logger::Private::AllLoggers()->begin(); logIt != logger::Private::AllLoggers()->end(); ++logIt)
        {
            if( (*logIt)->fPrivate->fThresholdIsGlobal )
            {
                (*logIt)->fPrivate->fThreshold = logger::Private::GlobalThreshold();
            }
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
        if (level >= ELevel::eWarn)
        {
            fPrivate->logCerr(level, message, loc);
        }
        else
        {
            fPrivate->logCout(level, message, loc);
        }
    }
}
