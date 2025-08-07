/*
 * logger.cc
 *
 *  Created on: Jul 10, 2025
 *      Author: N.S. Oblath
 */

 #include "logger.hh"

#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
//#include "spdlog/spdlog.h"

namespace scarab
{
    spd_initializer::spd_initializer( const std::string& a_pattern ) :
            f_pattern( a_pattern )
    {
        if( f_pattern.empty() ) f_pattern = "%^%Y-%m-%d %H:%M:%S.%e [%l] (%t) %s:%# -> %v%$";
    }

    std::shared_ptr< spdlog::logger > spd_initializer::make_or_get_logger( const std::string& a_name )
    {
        std::shared_ptr< spdlog::logger > t_logger = spdlog::get( a_name );
        if( ! t_logger )
        {
            t_logger = this->make_logger( a_name );
            spdlog::register_logger( t_logger );
        }
        return t_logger;
    }


    spd_initializer_async_stdout_color_mt::spd_initializer_async_stdout_color_mt( const std::string& a_pattern ) :
            spd_initializer( a_pattern ),
            f_sink()
    {
        spdlog::init_thread_pool(8192, 1);
        f_sink = std::make_shared< spdlog::sinks::stdout_color_sink_mt >();
        f_sink->set_pattern( f_pattern ); 
        auto at_exit_fcn = [this](){ logger::stop_using_spd_asyc(); };
        std::atexit( at_exit_fcn );
    }

    std::shared_ptr< spdlog::logger > spd_initializer_async_stdout_color_mt::make_logger( const std::string& a_name )
    {
        return std::make_shared<spdlog::async_logger>( a_name, f_sink, spdlog::thread_pool(), spdlog::async_overflow_policy::block );
    }


    spd_initializer_stdout_color::spd_initializer_stdout_color( const std::string& a_pattern ) :
            spd_initializer( a_pattern ),
            f_sink()
    {
        f_sink = std::make_shared< spdlog::sinks::stdout_color_sink_st >();
        f_sink->set_pattern( f_pattern );
    }

    std::shared_ptr< spdlog::logger > spd_initializer_stdout_color::make_logger( const std::string& a_name )
    {
        return std::make_shared<spdlog::logger>( a_name, f_sink );
    }


    logger::logger( const std::string& a_name ) :
        f_threshold( logger::ELevel::eInfo ),
        f_strstr(),
        f_spdlogger()
    {
        scarab::logger::all_loggers().insert(this);
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
        //if( logger::using_spd() )
        //{
            f_spdlogger->set_level( spdlog::level::level_enum(unsigned(f_threshold)) );
        //}
        return;
    }

    logger::ELevel logger::get_global_threshold()
    {
        return logger::global_threshold();
    }

    void logger::set_global_threshold( ELevel a_level )
    {
        logger::global_threshold() = a_level;
        spdlog::set_level( spdlog::level::level_enum(unsigned(a_level)) );
        std::set< logger* >& t_all_loggers = logger::all_loggers();
        for( auto t_logger_ptr : t_all_loggers )
        {
            t_logger_ptr->set_threshold( a_level );
        }
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

    std::shared_ptr<spdlog::logger> logger::spdlogger()
    {
        return f_spdlogger;
    }

    void logger::stop_using_spd_async()
    {
        std::cerr << "Stopping use of spd async" << std::endl;
        logger::using_spd_async().store(false);
        return;
    }

    void logger::reset_using_spd_async()
    {
        std::cerr << "Resetting use of spd async" << std::endl;
        logger::using_spd_async().store(true);
        return;
    }

    std::atomic_bool& logger::using_spd_async()
    {
        static std::atomic_bool s_using_spd_async(true);
        return s_using_spd_async;
    }

}
