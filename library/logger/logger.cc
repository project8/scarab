/*
 * logger.cxx
 * based on KLogger.cxx from KATRIN's Kasper
 *
 *  Created on: 18.11.2011
 *      Author: Marco Haag <marco.haag@kit.edu>
 */

#define SCARAB_API_EXPORTS

#include "logger.hh"

#include <chrono>
#include <thread>

using namespace std;

namespace scarab
{

    quill_initializer::quill_initializer()
    {
        if( ! logger::is_quill_stopped() )
        {
            //std::cerr << "Creating Quill Initializer" << std::endl;
            quill::BackendOptions backend_options;
            // Scarab uses slightly different level names at the fatal/critical and prog/notice levels
            backend_options.log_level_descriptions[uint8_t(quill::LogLevel::Critical)] = "FATAL";
            backend_options.log_level_descriptions[uint8_t(quill::LogLevel::Notice)] = "PROG";
            backend_options.log_level_descriptions[uint8_t(quill::LogLevel::TraceL1)] = "TRACE";
            backend_options.log_level_short_codes[uint8_t(quill::LogLevel::Critical)] = "F";
            backend_options.log_level_short_codes[uint8_t(quill::LogLevel::Notice)] = "P";
            // Modify the character filtering: make \t visible
            // The default filtering function is: [](char c){ return (c >= ' ' && c <= '~') || (c == '\n'); }
            backend_options.check_printable_char = [](char c){ return (c >= ' ' && c <= '~') || (c == '\n') || (c == '\t'); };
            quill::Backend::start( backend_options );
        }
    }

    quill_guard::~quill_guard()
    {
        logger::stop_quill();
    }

    ELevel logger::f_global_threshold = ELevel::eDebug;

    // This is to indicate whether Quill has been stopped via the logger class after having run,
    // so it needs to return false before it runs.
    std::atomic_bool logger::s_quill_stopped(false);

    //std::set< logger* > logger::s_all_loggers;

    logger::logger( const std::string& a_name ) :
            f_buffer_stream(),
            f_buffer(),
            f_quill(nullptr),
            f_threshold(ELevel::eInfo)
    {
        // Use static initialization to ensure that there's a Quill Backend ready to print messages whenever the first logger is created
        static scarab::quill_initializer q_init;

        logger::all_loggers().insert(this);

        if( ! logger::is_quill_stopped() )
        {
            auto console_colors = quill::ConsoleSinkConfig::Colours();
            console_colors.apply_default_colours();
            console_colors.assign_colour_to_log_level( quill::LogLevel::Notice, quill::ConsoleSinkConfig::Colours::blue);
            quill::ConsoleSinkConfig config;
            config.set_colours(console_colors);
            auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>( a_name+"_console_sink", config );
            f_quill = quill::Frontend::create_or_get_logger( a_name, std::move(console_sink),
                                                            quill::PatternFormatterOptions{ "%(time) [%(log_level)] (%(thread_id)) %(short_source_location) -> %(message)", "%Y-%m-%d %T.%Qms", quill::Timezone::GmtTime}
                                                            );
            f_quill->set_log_level( quill::LogLevel(unsigned(f_global_threshold)) );
        }
    }

    logger::~logger()
    {
        all_loggers().erase(this);
    }

    ELevel logger::get_threshold() const
    {
        return f_threshold;
    }

    void logger::set_threshold( ELevel a_threshold )
    {
        f_threshold = a_threshold;
        if( ! logger::is_quill_stopped() )
        {
            f_quill->set_log_level( quill::LogLevel(unsigned(f_threshold)) );
        }
        return;
    }

    ELevel logger::get_global_threshold()
    {
        return f_global_threshold;
    }

    void logger::set_global_threshold( ELevel a_threshold )
    {
        logger::f_global_threshold = a_threshold;
        std::set< logger* >& t_all_loggers = logger::all_loggers();
        for( auto t_logger_ptr : t_all_loggers )
        {
            t_logger_ptr->set_threshold( a_threshold );
        }
        return;
    }

    std::set< logger* >& logger::all_loggers()
    {
        static std::set< logger* > s_all_loggers;
        return s_all_loggers;
    }

    void logger::stop_quill()
    {
        using namespace std::chrono_literals;

        // Do this first so that logging statements stop using Quill before we tell Quill to stop
        s_quill_stopped.store(true);

        quill::Backend::stop();

        const auto t_start = std::chrono::high_resolution_clock::now();
        while( quill::Backend::is_running() )
        {
            std::this_thread::sleep_for(1ms);
        }
        const auto t_end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::milli> t_elapsed = t_end - t_start;
        std::cout << "Quill took " << t_elapsed.count() << " ms to shutdown" << std::endl;

        return;
    }

    bool logger::is_quill_stopped()
    {
        return logger::s_quill_stopped.load();
    }

} // namespace scarab
