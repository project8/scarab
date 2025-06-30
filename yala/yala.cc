#include "yala.hh"

#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
//#include "spdlog/spdlog.h"

namespace yala
{
    spd_initializer::spd_initializer()
    {
        std::atexit(
            [](){
                logger::stop_using_spd();
            }
        );
    }


    spd_initializer_async_stdout_color_mt::spd_initializer_async_stdout_color_mt() :
            spd_initializer()
    {
        spdlog::init_thread_pool(8192, 1);
        f_sink = std::make_shared< spdlog::sinks::stdout_color_sink_mt >();
        // TODO: setup sink here
    }

    std::shared_ptr< spdlog::logger > spd_initializer_async_stdout_color_mt::make_logger( const std::string& a_name )
    {
        return std::make_shared<spdlog::async_logger>( a_name, f_sink, spdlog::thread_pool(), spdlog::async_overflow_policy::block );
    }


    spd_initializer_stdout_color::spd_initializer_stdout_color() :
            spd_initializer()
    {
        f_sink = std::make_shared< spdlog::sinks::stdout_color_sink_st >();
        // TODO: setup sink here
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
        yala::logger::all_loggers().insert(this);
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
        if( logger::using_spd() )
        {
            f_spdlogger->set_level( spdlog::level::level_enum(unsigned(f_threshold)) );
        }
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

    void logger::stop_using_spd()
    {
        std::cerr << "Stopping use of spd" << std::endl;
        logger::using_spd().store(false);
        return;
    }

    std::atomic_bool& logger::using_spd()
    {
        static std::atomic_bool s_using_spd(true);
        return s_using_spd;
    }

}