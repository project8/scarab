/*
 * logger.cxx
 * based on KLogger.cxx from KATRIN's Kasper
 *
 *  Created on: 18.11.2011
 *      Author: Marco Haag <marco.haag@kit.edu>
 */

#define SCARAB_API_EXPORTS

#include "logger.hh"

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>


#include <memory>
#include <set>


using namespace std;

namespace scarab
{
    struct quill_initializer
    {
        quill_initializer()
        {
            //std::cerr << "Quill Initializer" << std::endl;
            quill::BackendOptions backend_options;
            backend_options.log_level_descriptions[uint8_t(quill::LogLevel::Critical)] = "FATAL";
            backend_options.log_level_descriptions[uint8_t(quill::LogLevel::Notice)] = "PROD";
            backend_options.log_level_short_codes[uint8_t(quill::LogLevel::Critical)] = "F";
            backend_options.log_level_short_codes[uint8_t(quill::LogLevel::Notice)] = "P";
            quill::Backend::start( backend_options );
        }
    };

    struct logger::Private
    {
        static unsigned& count()
        {
            static unsigned sCount = 0;
            return sCount;
        }

        typedef std::set< logger* > LoggerSet;
        static std::shared_ptr<LoggerSet>& AllLoggers()
        {
            // construct-on-first-use strategy to avoid static initialization fiasco
            static std::shared_ptr<LoggerSet> sAllLoggers = std::make_shared< LoggerSet >();
            return sAllLoggers;
        }

        const char* fLogger;

        ELevel fThreshold;
        bool fThresholdIsGlobal;

        static ELevel& GlobalThreshold()
        {
            // construct-on-first-use strategy to avoid static initialization fiasco
            static ELevel sGlobalThreshold = logger::Private::filterMinimumLevel(ELevel::eDebug);
            return sGlobalThreshold;
        }
/*
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
*/
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

    }; // end of logger::Private struct


    logger::logger(const char* name) : fPrivate(new Private())
    {
        // Use static initialization to ensure that there's a Quill Backend ready to print messages whenever the first logger is created
        static scarab::quill_initializer qInit;

        auto console_colors = quill::ConsoleColours();
        console_colors.set_default_colours();
        console_colors.set_colour( quill::LogLevel::Notice, quill::ConsoleColours::blue);
        auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>( "console_sink", console_colors );
        qLogger = quill::Frontend::create_or_get_logger( name, std::move(console_sink),
                                                         quill::PatternFormatterOptions{ "%(time) [%(log_level)] (%(thread_id)) %(short_source_location) -> %(message)", "%Y-%m-%d %T.%Qms", quill::Timezone::GmtTime}
                                                       );
        //qLogger->set_log_level( quill::LogLevel::TraceL3 );

        logger::Private::count()++;

        if (name == nullptr)
        {
            fPrivate->fLogger = "root";
        }
        else
        {
            const char* logName = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
            fPrivate->fLogger = logName;
        }

        UseGlobalLevel();
        logger::Private::AllLoggers()->insert(this);

        //std::cerr << "created logger (" << fPrivate->count() << ") " << fPrivate->fLogger << std::endl;
    }
/*
    logger::logger(const std::string& name) : fPrivate(new Private())
    {
        if( logger::Private::count() == 0 )
        {
            snprintf( logger::Private::dateTimeFormat(), 16,  "%%Y-%%m-%%d %%T" );
        }

        logger::Private::count()++;
        
        fPrivate->fLogger = name.c_str();

		UseGlobalLevel();

		logger::Private::AllLoggers()->insert(this);

        //std::cerr << "created logger (" << fPrivate->count() << ") " << fPrivate->fLogger << std::endl;
    }
*/
    logger::~logger()
    {
        fPrivate->count()--;
        
        //std::cerr << "destructing logger (" << fPrivate->count() << ") " << fPrivate->fLogger << std::endl;
        logger::Private::AllLoggers()->erase(this);
        //std::cerr << "logger removed from set: " << fPrivate->fLogger << std::endl;

        if( fPrivate->count() == 0 )
        {
            quill::Backend::stop();
        }
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
        qLogger->set_log_level( quill::v7::LogLevel(fPrivate->fThreshold) );
    }

    void logger::UseGlobalLevel() const
    {
        fPrivate->fThreshold = logger::Private::filterMinimumLevel(logger::Private::GlobalThreshold());
        fPrivate->fThresholdIsGlobal = true;
        qLogger->set_log_level( quill::v7::LogLevel(fPrivate->fThreshold) );
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

}
