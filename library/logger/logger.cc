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
    quill_initializer::quill_initializer()
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

    logger_helper::logger_helper( quill::Logger*& a_logger, const std::string& a_name ) :
            fStream(),
            fBuffer()
    {
        // Use static initialization to ensure that there's a Quill Backend ready to print messages whenever the first logger is created
        static scarab::quill_initializer qInit;

        auto console_colors = quill::ConsoleSinkConfig::Colours();
        console_colors.apply_default_colours();
        console_colors.assign_colour_to_log_level( quill::LogLevel::Notice, quill::ConsoleSinkConfig::Colours::blue);
        quill::ConsoleSinkConfig config;
        config.set_colours(console_colors);
        auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>( a_name+"_console_sink", config );
        a_logger = quill::Frontend::create_or_get_logger( a_name, std::move(console_sink),
                                                         quill::PatternFormatterOptions{ "%(time) [%(log_level)] (%(thread_id)) %(short_source_location) -> %(message)", "%Y-%m-%d %T.%Qms", quill::Timezone::GmtTime}
                                                       );
        //qLogger->set_log_level( quill::LogLevel::TraceL3 );
    }

    logger_helper::~logger_helper()
    {}

    ELevel logger_helper::get_global_threshold()
    {
        return f_global_threshold;
    }

    void logger_helper::set_global_threshold( ELevel a_threshold )
    {
        f_global_threshold = a_threshold;
        auto t_all_loggers = quill::Frontend::get_all_loggers();
        for( auto t_logger : t_all_loggers )
        {
            t_logger->set_log_level( quill::LogLevel(unsigned(f_global_threshold)) );
        }
        return;
    }


    ELevel logger_helper::f_global_threshold = ELevel::eDebug;

} // namespace scarab
