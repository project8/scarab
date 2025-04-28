/*
 * logger.cxx
 * based on KLogger.cxx from KATRIN's Kasper
 *
 *  Created on: 18.11.2011
 *      Author: Marco Haag <marco.haag@kit.edu>
 */

#define SCARAB_API_EXPORTS

#include "logger.hh"


//#include <quill/core/LoggerManager.h>

using namespace std;

namespace scarab
{
    quill_initializer::quill_initializer( std::function< void(quill::BackendOptions&) > a_runtime_opts )
    {
        //std::cerr << "Creating Quill Initializer" << std::endl;
        quill::BackendOptions backend_options;
        // Scarab uses slightly different level names at the fatal/critical and prod/notice levels
        backend_options.log_level_descriptions[uint8_t(quill::LogLevel::Critical)] = "FATAL";
        backend_options.log_level_descriptions[uint8_t(quill::LogLevel::Notice)] = "PROD";
        backend_options.log_level_short_codes[uint8_t(quill::LogLevel::Critical)] = "F";
        backend_options.log_level_short_codes[uint8_t(quill::LogLevel::Notice)] = "P";
        // Modify the character filtering: make \t visible
        // The default filtering function is: [](char c){ return (c >= ' ' && c <= '~') || (c == '\n'); }
        backend_options.check_printable_char = [](char c){ return (c >= ' ' && c <= '~') || (c == '\n') || (c == '\t'); };
        //
        backend_options.sleep_duration = std::chrono::microseconds(50);
        // Use the callback for any runtime options
        std::cerr << "initializing quill and using runtime options setting" << std::endl;
        a_runtime_opts( backend_options );
        quill::Backend::start( backend_options );
    }
    //quill_initializer::~quill_initializer()
    //{
    //    std::cerr << "Destructing quill initializer" << std::endl;
    //}

    //quill_guard::~quill_guard()
    //{
    //    std::cerr << "calling backend stop via quill guard" << std::endl;
    //    quill::Backend::stop();
    //}


    logger::logger( const char* name, std::function< void(quill::BackendOptions&) > a_runtime_opts )
    {
        // Use static initialization to ensure that there's a Quill Backend ready to print messages whenever the first logger is created
        static scarab::quill_initializer qInit( a_runtime_opts );

        auto console_colors = quill::ConsoleSink::Colours();
        console_colors.apply_default_colours();
        console_colors.assign_colour_to_log_level( quill::LogLevel::Notice, quill::ConsoleSink::Colours::blue);
        auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>( "console_sink", console_colors );
        qLogger = quill::Frontend::create_or_get_logger( name, std::move(console_sink),
                                                         quill::PatternFormatterOptions{ "%(time) [%(log_level)] (%(thread_id)) %(short_source_location) -> %(message)", "%Y-%m-%d %T.%Qms", quill::Timezone::GmtTime}
                                                       );
        //qLogger->set_log_level( quill::LogLevel::TraceL3 );

        logger::count()++;

        if (name == nullptr)
        {
            fName = "root";
        }
        else
        {
            const char* logName = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
            fName = logName;
        }

        UseGlobalLevel();
        logger::AllLoggers()->insert(this);

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
        quill::detail::LoggerManager& lm = quill::detail::LoggerManager::instance();
        //std::cerr << "Destructing logger <" << fName << ">; quill logger: " << lm.get_logger( fName ) << std::endl;

        logger::count()--;
        
        //std::cerr << "destructing logger (" << fPrivate->count() << ") " << fPrivate->fLogger << std::endl;
        logger::AllLoggers()->erase(this);
        //std::cerr << "logger removed from set: " << fPrivate->fLogger << std::endl;
    }

    bool logger::IsLevelEnabled(ELevel level) const
    {
        return level >= fThreshold;
    }

    logger::ELevel logger::GetLevel() const
    {
        return fThreshold;
    }

    void logger::SetLevel(ELevel level)
    {
        fThreshold = logger::filterMinimumLevel(level);
        fThresholdIsGlobal = false;
        qLogger->set_log_level( quill::v8::LogLevel(fThreshold) );
    }

    void logger::UseGlobalLevel()
    {
        fThreshold = logger::filterMinimumLevel(logger::GlobalThreshold());
        fThresholdIsGlobal = true;
        qLogger->set_log_level( quill::v8::LogLevel(fThreshold) );
    } 

    //////
    // Static functions
    //////

    std::shared_ptr<logger::LoggerSet>& logger::AllLoggers()
    {
        // construct-on-first-use strategy to avoid static initialization fiasco
        static std::shared_ptr<LoggerSet> sAllLoggers = std::make_shared< LoggerSet >();
        return sAllLoggers;
    }

    unsigned& logger::count()
    {
        // construct on first use to avoid static initialization fiasco
        static unsigned sCount = 0;
        return sCount;
    }

    logger::ELevel& logger::GlobalThreshold()
    {
        // construct-on-first-use strategy to avoid static initialization fiasco
        static ELevel sGlobalThreshold = logger::filterMinimumLevel(ELevel::eDebug);
        return sGlobalThreshold;
    }

    logger::ELevel logger::filterMinimumLevel(logger::ELevel level)
    {
#if defined(NDEBUG) && defined(STANDARD)
        return level >= logger::ELevel::eInfo ? level : logger::ELevel::eInfo;
#elif defined(NDEBUG)
        return level >= logger::ELevel::eProg ? level : logger::ELevel::eProg;
#else
        return level;
#endif
    }

    logger::ELevel logger::GetGlobalLevel()
    {
        return logger::GlobalThreshold();
    }

    void logger::SetGlobalLevel(ELevel level)
    {
        logger::GlobalThreshold() = logger::filterMinimumLevel(level);
        for( logger::LoggerSet::iterator logIt = logger::AllLoggers()->begin(); logIt != logger::AllLoggers()->end(); ++logIt)
        {
            if( (*logIt)->fThresholdIsGlobal )
            {
                (*logIt)->fThreshold = logger::GlobalThreshold();
            }
        }
        return;
    }

    void SCARAB_API stop_logging()
    {
        quill::Backend::stop();
    }

}
