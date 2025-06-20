#include "yala.hh"

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/sinks/ConsoleSink.h"


namespace yala
{
    logger::quill_initializer::quill_initializer()
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

        std::atexit(
            [](){
                std::cerr << "Quill running? " << quill::Backend::is_running() << std::endl;
                logger::stop_using_quill();
            }
        );
    }

    logger::logger( const std::string& a_name ) :
        f_threshold( logger::ELevel::eInfo ),
        f_strstr(),
        f_quill( nullptr )
    {
        // Start the backend, but only once
        static quill_initializer s_quill_init;

        // Frontend
        auto t_console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>( a_name+"_console_sink" );
        f_quill = quill::Frontend::create_or_get_logger( a_name, std::move(t_console_sink) );
    
        // Change the LogLevel to print everything
        f_quill->set_log_level(quill::LogLevel::TraceL3);
    }

    logger::~logger()
    {
        logger::all_loggers().erase(this);
    }

    std::set< logger* >& logger::all_loggers()
    {
        static std::set< logger* > s_all_loggers;
        return s_all_loggers;
    }

    logger::ELevel logger::get_threshold() const
    {
        return f_threshold;
    }

    void logger::set_threshold( logger::ELevel a_level )
    {
        f_threshold = a_level;
        return;
    }

    logger::ELevel logger::get_global_threshold()
    {
        return logger::global_threshold();
    }

    void logger::set_global_threshold( ELevel a_level )
    {
        logger::global_threshold() = a_level;
        return;
    }

    logger::ELevel& logger::global_threshold()
    {
        static logger::ELevel s_global_threshold( logger::ELevel::eInfo );
        return s_global_threshold;
    }

    std::string logger::buffer()
    {
        std::string t_buffer( f_strstr.str() );
        f_strstr.str( std::string() );
        return std::string( std::move(t_buffer) );
    }

    quill::Logger* logger::quill()
    {
        return f_quill;
    }

    void logger::stop_using_quill()
    {
        std::cerr << "Stopping use of quill" << std::endl;
        logger::using_quill().store(false);
        return;
    }

    std::atomic_bool& logger::using_quill()
    {
        static std::atomic_bool s_using_quill(true);
        return s_using_quill;
    }

}