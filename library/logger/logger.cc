/*
 * logger.cxx
 * based on KLogger.cxx from KATRIN's Kasper
 *
 *  Created on: 18.11.2011
 *      Author: Marco Haag <marco.haag@kit.edu>
 */

#define SCARAB_API_EXPORTS

#include "logger.hh"


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
            backend_options.log_level_descriptions[uint8_t(quill::LogLevel::Notice)] = "prog";
            backend_options.log_level_short_codes[uint8_t(quill::LogLevel::Critical)] = "F";
            backend_options.log_level_short_codes[uint8_t(quill::LogLevel::Notice)] = "P";
            // Modify the character filtering: make \t visible
            // The default filtering function is: [](char c){ return (c >= ' ' && c <= '~') || (c == '\n'); }
            backend_options.check_printable_char = [](char c){ return (c >= ' ' && c <= '~') || (c == '\n') || (c == '\t'); };
            quill::Backend::start( backend_options );

            // Set the logging functions to use Quill
            logger::s_log_trace_fcn = &scarab::logger::log_trace_to_quill;
            logger::s_log_debug_fcn = &scarab::logger::log_debug_to_quill;
            logger::s_log_info_fcn = &scarab::logger::log_info_to_quill;
            logger::s_log_prog_fcn = &scarab::logger::log_prog_to_quill;
            logger::s_log_warn_fcn = &scarab::logger::log_warn_to_quill;
            logger::s_log_error_fcn = &scarab::logger::log_error_to_quill;
            logger::s_log_fatal_fcn = &scarab::logger::log_fatal_to_quill;
        }
        else
        {
            // Set the logging functions to use stdout
            logger::s_log_trace_fcn = &scarab::logger::log_trace_to_stdout;
            logger::s_log_debug_fcn = &scarab::logger::log_debug_to_stdout;
            logger::s_log_info_fcn = &scarab::logger::log_info_to_stdout;
            logger::s_log_prog_fcn = &scarab::logger::log_prog_to_stdout;
            logger::s_log_warn_fcn = &scarab::logger::log_warn_to_stdout;
            logger::s_log_error_fcn = &scarab::logger::log_error_to_stdout;
            logger::s_log_fatal_fcn = &scarab::logger::log_fatal_to_stdout;
        }

        //logger::check_log_functions();
    }
    //quill_initializer::~quill_initializer()
    //{
    //    std::cerr << "Destructing quill initializer" << std::endl;
    //}

    quill_guard::~quill_guard()
    {
        //std::cerr << "calling backend stop via quill guard" << std::endl;
        logger::stop_quill();
    }

    ELevel logger::f_global_threshold = ELevel::eDebug;

    bool logger::s_quill_stopped = false;

    void (*logger::s_log_trace_fcn)(logger&, const std::string&) = &scarab::logger::log_trace_to_quill;
    void (*logger::s_log_debug_fcn)(logger&, const std::string&) = &scarab::logger::log_debug_to_quill;
    void (*logger::s_log_info_fcn)(logger&, const std::string&) = &scarab::logger::log_info_to_quill;
    void (*logger::s_log_prog_fcn)(logger&, const std::string&) = &scarab::logger::log_prog_to_quill;
    void (*logger::s_log_warn_fcn)(logger&, const std::string&) = &scarab::logger::log_warn_to_quill;
    void (*logger::s_log_error_fcn)(logger&, const std::string&) = &scarab::logger::log_error_to_quill;
    void (*logger::s_log_fatal_fcn)(logger&, const std::string&) = &scarab::logger::log_fatal_to_quill;

    logger::logger( const std::string& a_name ) :
            f_stream(),
            f_buffer(),
            f_quill(nullptr)
    {
        // Use static initialization to ensure that there's a Quill Backend ready to print messages whenever the first logger is created
        static scarab::quill_initializer q_init;

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

        //logger::check_log_functions();
    }

    logger::~logger()
    {}

    ELevel logger::get_global_threshold()
    {
        return f_global_threshold;
    }

    void logger::set_global_threshold( ELevel a_threshold )
    {
        f_global_threshold = a_threshold;
        auto t_all_loggers = quill::Frontend::get_all_loggers();
        for( auto t_logger : t_all_loggers )
        {
            t_logger->set_log_level( quill::LogLevel(unsigned(f_global_threshold)) );
        }
        return;
    }

    void logger::stop_quill()
    {
        std::cerr << "Stopping quill" << std::endl;

        s_quill_stopped = true;

        logger::s_log_trace_fcn = &scarab::logger::log_trace_to_stdout;
        logger::s_log_debug_fcn = &scarab::logger::log_debug_to_stdout;
        logger::s_log_info_fcn = &scarab::logger::log_info_to_stdout;
        logger::s_log_prog_fcn = &scarab::logger::log_prog_to_stdout;
        logger::s_log_warn_fcn = &scarab::logger::log_warn_to_stdout;
        logger::s_log_error_fcn = &scarab::logger::log_error_to_stdout;
        logger::s_log_fatal_fcn = &scarab::logger::log_fatal_to_stdout;

        quill::Backend::stop();

        //logger::check_log_functions();

        return;
    }

    bool logger::is_quill_stopped()
    {
        return logger::s_quill_stopped;
    }

    void logger::check_log_functions()
    {
        if( logger::is_quill_stopped() )
        {
            std::cerr << "Logger check functions: quill is stopped" << std::endl;
            //std::cerr << "Trace? " << (logger::s_log_trace_fcn == &logger::log_trace_to_stdout) << std::endl;
            //std::cerr << "Debug? " << (logger::s_log_debug_fcn == &logger::log_debug_to_stdout) << std::endl;
            //std::cerr << "Info? " << (logger::s_log_info_fcn == &logger::log_info_to_stdout) << std::endl;
            //std::cerr << "Prog? " << (logger::s_log_prog_fcn == &logger::log_prog_to_stdout) << std::endl;
            //std::cerr << "Warn? " << (logger::s_log_warn_fcn == &logger::log_warn_to_stdout) << std::endl;
            //std::cerr << "Error? " << (logger::s_log_error_fcn == &logger::log_error_to_stdout) << std::endl;
            //std::cerr << "Fatal? " << (logger::s_log_fatal_fcn == &logger::log_fatal_to_stdout) << std::endl;
        }
        else
        {
            std::cerr << "Logger check functions: quill is NOT stopped" << std::endl;
            //std::cerr << "Trace? " << (logger::s_log_trace_fcn == &logger::log_trace_to_quill) << std::endl;
            //std::cerr << "Debug? " << (logger::s_log_debug_fcn == &logger::log_debug_to_quill) << std::endl;
            //std::cerr << "Info? " << (logger::s_log_info_fcn == &logger::log_info_to_quill) << std::endl;
            //std::cerr << "Prog? " << (logger::s_log_prog_fcn == &logger::log_prog_to_quill) << std::endl;
            //std::cerr << "Warn? " << (logger::s_log_warn_fcn == &logger::log_warn_to_quill) << std::endl;
            //std::cerr << "Error? " << (logger::s_log_error_fcn == &logger::log_error_to_quill) << std::endl;
            //std::cerr << "Fatal? " << (logger::s_log_fatal_fcn == &logger::log_fatal_to_quill) << std::endl;
        }
        return;
    }

    void logger::log_trace_to_quill( logger& a_logger, const std::string& a_message )
    {
        LOG_TRACE_L1(a_logger.quill(), "{}", a_message);
    }

    void logger::log_trace_to_stdout( logger& /*a_logger*/, const std::string& a_message )
    {
        std::cout << "[TRACE] " << a_message << std::endl;
    }

    void logger::log_debug_to_quill( logger& a_logger, const std::string& a_message )
    {
        LOG_DEBUG(a_logger.quill(), "{}", a_message);
    }

    void logger::log_debug_to_stdout( logger& /*a_logger*/, const std::string& a_message )
    {
        std::cout << "[DEBUG] " << a_message << std::endl;
    }

    void logger::log_info_to_quill( logger& a_logger, const std::string& a_message )
    {
        LOG_INFO(a_logger.quill(), "{}", a_message);
    }

    void logger::log_info_to_stdout( logger& /*a_logger*/, const std::string& a_message )
    {
        std::cout << "[INFO] " << a_message << std::endl;
    }

    void logger::log_prog_to_quill( logger& a_logger, const std::string& a_message )
    {
        LOG_NOTICE(a_logger.quill(), "{}", a_message);
    }

    void logger::log_prog_to_stdout( logger& /*a_logger*/, const std::string& a_message )
    {
        std::cout << "[PROG] " << a_message << std::endl;
    }

    void logger::log_warn_to_quill( logger& a_logger, const std::string& a_message )
    {
        LOG_WARNING(a_logger.quill(), "{}", a_message);
    }

    void logger::log_warn_to_stdout( logger& /*a_logger*/, const std::string& a_message )
    {
        std::cout << "[WARNING] " << a_message << std::endl;
    }

    void logger::log_error_to_quill( logger& a_logger, const std::string& a_message )
    {
        LOG_ERROR(a_logger.quill(), "{}", a_message);
    }

    void logger::log_error_to_stdout( logger& /*a_logger*/, const std::string& a_message )
    {
        std::cout << "[ERROR] " << a_message << std::endl;
    }

    void logger::log_fatal_to_quill( logger& a_logger, const std::string& a_message )
    {
        LOG_CRITICAL(a_logger.quill(), "{}", a_message);
    }

    void logger::log_fatal_to_stdout( logger& /*a_logger*/, const std::string& a_message )
    {
        std::cout << "[FATAL] " << a_message << std::endl;
    }

} // namespace scarab
