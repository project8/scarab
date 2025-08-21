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

    std::shared_ptr< spdlog::logger > spd_initializer::make_or_get_logger( const std::string& a_name, std::function< std::shared_ptr< spdlog::logger > (const std::string&) > a_make_logger_fcn )
    {
        std::shared_ptr< spdlog::logger > t_logger = spdlog::get( a_name );
        if( ! t_logger )
        {
            // see the comment in logger.hh about why the a_make_logger_fcn callback is used instead of the virtual function call
            t_logger = a_make_logger_fcn( a_name ); //this->make_logger( a_name );
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
        auto at_exit_fcn = [](){ logger::stop_using_spd_async(); };
        std::atexit( at_exit_fcn );
    }

    std::shared_ptr< spdlog::logger > spd_initializer_async_stdout_color_mt::make_logger( const std::string& a_name )
    {
        return make_logger_async( a_name );
    }

    std::shared_ptr< spdlog::logger > spd_initializer_async_stdout_color_mt::make_logger_async( const std::string& a_name )
    {
        return std::make_shared<spdlog::async_logger>( a_name, f_sink, spdlog::thread_pool(), spdlog::async_overflow_policy::block );
    }

    std::shared_ptr< spdlog::logger > spd_initializer_async_stdout_color_mt::make_logger_sync( const std::string& a_name )
    {
        return std::make_shared<spdlog::logger>( a_name, f_sink );
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
        f_name( a_name ),
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

    const std::string& logger::name() const
    {
        return f_name;
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
#ifdef SCARAB_LOGGER_DEBUG
        std::cout << "Stopping use of spd async" << std::endl;
#endif
        logger::using_spd_async().store(false);
        // loop through all loggers
            // if dynamic_cast to async version works, then switch backend to sync
        std::set< logger* >& t_all_loggers = logger::all_loggers();
        std::shared_ptr< spdlog::sinks::sink > t_new_sink;
        std::set< std::string > t_replaced_loggers;
        for( auto t_logger_ptr : t_all_loggers )
        {
            logger_type< spd_initializer_async_stdout_color_mt >* t_ltype_ptr = dynamic_cast< logger_type< spd_initializer_async_stdout_color_mt >* >(t_logger_ptr);
            if( t_ltype_ptr != nullptr )
            {
                if( ! t_new_sink ) // then this is the first time we've gotten an async logger; we'll do this only once
                {
                    // We'll use the logger's pointer to the initializer to update the (static) initializer with the new sink
                    // create the new sink and store to t_new_sink so this doesn't repeat
                    t_new_sink = std::make_shared< spdlog::sinks::stdout_color_sink_st >();
                    // we need to do the same steps that are in spd_initializer_stdout_color's constructor
                    t_ltype_ptr->initializer().f_sink = t_new_sink;
                    t_new_sink->set_pattern( t_ltype_ptr->initializer().f_pattern );
                }
                if( t_replaced_loggers.find(t_ltype_ptr->name()) == t_replaced_loggers.end() )
                {
                    // new spd logger doesn't exist, so create the new logger and save it to the scarab::logger
                    t_replaced_loggers.insert( t_ltype_ptr->name() );
                    std::shared_ptr< spdlog::logger > t_new_logger = t_ltype_ptr->initializer().make_logger_sync( t_ltype_ptr->name() );
                    spdlog::register_or_replace( t_new_logger );
                    t_ltype_ptr->f_spdlogger = t_new_logger;
                }
                else
                {
                    // new spd logger already exists, so get it and save it to the scarab::logger
                    std::shared_ptr< spdlog::logger > t_new_logger = spdlog::get( t_ltype_ptr->name() );
                    t_ltype_ptr->f_spdlogger = t_new_logger;
                }
            }
        }

#ifdef SCARAB_LOGGER_DEBUG
        std::cout << "SPD logger changed to sync for " << t_replaced_loggers.size() << " loggers" << std::endl;
#endif

        return;
    }

    void logger::reset_using_spd_async()
    {
#ifdef SCARAB_LOGGER_DEBUG
        std::cerr << "Resetting use of spd async" << std::endl;
#endif
        logger::using_spd_async().store(true);
        std::set< logger* >& t_all_loggers = logger::all_loggers();
        std::shared_ptr< spdlog::sinks::sink > t_new_sink;
        std::set< std::string > t_replaced_loggers;
        // loop through all loggers
            // if dynamic_cast to async version works, then switch backend to async
        for( auto t_logger_ptr : t_all_loggers )
        {
            logger_type< spd_initializer_async_stdout_color_mt >* t_ltype_ptr = dynamic_cast< logger_type< spd_initializer_async_stdout_color_mt >* >(t_logger_ptr);
            if( t_ltype_ptr != nullptr )
            {
                if( ! t_new_sink ) // then this is the first time we've gotten an async logger; we'll do this only once
                {
                    // We'll use the logger's pointer to the initializer to update the (static) initializer with the new sink
                    // create the new sink and store to t_new_sink so this doesn't repeat
                    t_new_sink = std::make_shared< spdlog::sinks::stdout_color_sink_mt >();
                    // we need to do the same steps that are in spd_initializer_async_stdout_color_mt's constructor
                    if( ! spdlog::thread_pool() )
                    {
                        spdlog::init_thread_pool(8192, 1);
                    }
                    t_ltype_ptr->initializer().f_sink = t_new_sink;
                    t_new_sink->set_pattern( t_ltype_ptr->initializer().f_pattern );
                }
                if( t_replaced_loggers.find(t_ltype_ptr->name()) == t_replaced_loggers.end() )
                {
                    // new spd logger doesn't exist, so create the new logger and save it to the scarab::logger
                    t_replaced_loggers.insert( t_ltype_ptr->name() );
                    std::shared_ptr< spdlog::logger > t_new_logger = t_ltype_ptr->initializer().make_logger_async( t_ltype_ptr->name() );
                    spdlog::register_or_replace( t_new_logger );
                    t_ltype_ptr->f_spdlogger = t_new_logger;
                }
                else
                {
                    // new spd logger already exists, so get it and save it to the scarab::logger
                    std::shared_ptr< spdlog::logger > t_new_logger = spdlog::get( t_ltype_ptr->name() );
                    t_ltype_ptr->f_spdlogger = t_new_logger;
                }
            }
        }

#ifdef SCARAB_LOGGER_DEBUG
        std::cout << "SPD logger changed to async for " << t_replaced_loggers.size() << " loggers" << std::endl;
#endif

        return;
    }

    std::atomic_bool& logger::using_spd_async()
    {
        static std::atomic_bool s_using_spd_async(true);
        return s_using_spd_async;
    }

}
