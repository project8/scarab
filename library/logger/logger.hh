/*
 * logger.hh
 *
 *  Created on: Jul 10, 2025
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_LOGGER_HH_
#define SCARAB_LOGGER_HH_

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <sstream>

#include "spdlog/fwd.h"
#include "spdlog/common.h"
// this is here only for the logging macros; would be nice if the macros were in their own file
#include "spdlog/spdlog.h"

#include "macros.hh"
#include "scarab_api.hh"
#include "typename.hh"

#ifndef SCARAB_LOGGER_DEFAULT_THRESHOLD
#define SCARAB_LOGGER_DEFAULT_THRESHOLD
#endif

/**
 * @file logger.hh
 * @brief Contains the logger class and macros
 * @date Reborn on: Jul 10, 2025
 * @author N.S. Oblath
 * 
 * Basic use instructions:
 * 
 * 1. Create a logger with LOGGER or LOCAL_LOGGER (for asynchronous logging) or LOGGER_ST or LOCAL_LOGGER_ST (for synchronous logging)
 * 2. Use L[level] macros to log messages
 * 
 * Notes on the logging system design:
 * 
 * * The logger was originally based on the Kasper KLogger class, from the KATRIN experiment. This established many of the interface 
 *   choices, including the use of the LOGGER macro, and the macros for the various logging levels.
 * * The logger was originally built from scratch, sending its messages to stdout and stderr directly.
 * * In 2024 it was recognized that the existing logger was incompatible with the pybind11 handling of log messages 
 *   because the latter was not compatible with logging from multiple threads.  The choice was made to find a logging library
 *   that could funnel the logging through a single thread.
 * * The spdlog library was selected because of the similarity of the logging macros and levels to the existing macros and levels, 
 *   meaning that adapting it to the existing macros would be relatively easy.  spdlog provides asynchronous logging via a separate logging thread, 
 *   satisfying the pybind11 needs.
 * * The spdlog library was forked (project8/spdlog) and modified to add the NOTICE level of logging that's equivalent to scarab's PROG level.
 * * The scarab::logger class now stands as a thin wrapper on the spdlog::logger objects.  scarab::logger is still what client code will interact with.
 * * The spdlog async backend logging thread is started up using an initializer object that's built statically in the scarab::logger constructor.  
 *   This guarantees that it will be started when the first logger object is created.
 * * Logging levels had to be adapted slightly: forked spdlog's NOTICE level 
 *   is used for scarab's PROG level, and spdlog's CRITICAL level is used for scarab's FATAL level.
 * * Logging colors are matched to Python's logging colors.
 * * Verbosity can be set globally for all loggers and for individual loggers with the `set_global_threshold()` and `set_threshold() functions, respectively.  
 *   The function `set_global_threshold()` will set the threshold for all loggers, regardless of whether their level has been previously set individually, 
 *   so you should establish the global level first before customizing for individual loggers.
 * * The scarab::logger class also contains the mechanism for processing the streamer notation, using operator<< to compose log messages. 
 *   The message is buffered in the logger object using stringstream, and then passed to the spdlog::logger.
 * * In scarab and scarab-based code, sometimes log messages are printed from objects that exist after the main execution has stopped, as they're destructed at 
 *   the static cleanup phase.  This could cause a problem for spdlog if the asynchronous logging thread is automatically stopped and garbage collected.  
 *   Therefore we implemented a system where the async logging via spdlog can be explicitly stopped, at which point the logging switches to using single-threaded spdlog loggers.  
 * * The effect of not stopping the async spdlog logging mixed with logging after the async thread stops is undefined.
 * * Debug and trace messages are not compiled when scarab is built in Release mode.  This is done by a compile-time choice of using empty DEBUG and TRACE macros 
 *   when in Release mode.  spdlog has compile-time filtering capabilities too, but we don't use that currently.
 * * The initializer struct includes a function `make_or_get_logger()` that takes a callback that will be responsible fore creating a logger if it's needed.  
 *   This setup is used to avoid a virtual function call using spd_initializer::make_logger() to the derived initializer structs because 
 *   this is typically done at static initialization, and we can't use virtual function calls then.
 * * The macro SCARAB_LOGGER_DEBUG can be used to enable printing about the creation of spd loggers.
 */

namespace scarab
{
    struct SCARAB_API spd_initializer
    {
        spd_initializer( const std::string& a_pattern = "" );
        // see the comment above about why the callback a_make_logger_fcn is used
        std::shared_ptr< spdlog::logger > make_or_get_logger( const std::string& a_name, std::function< std::shared_ptr< spdlog::logger > (const std::string&) > a_make_logger_fcn );
        //virtual std::shared_ptr< spdlog::logger > make_logger( const std::string& a_name ) = 0;
        std::string f_pattern;
    };

    struct SCARAB_API spd_initializer_async_stdout_color_mt : spd_initializer
    {
        spd_initializer_async_stdout_color_mt( const std::string& a_pattern = "" );
        std::shared_ptr< spdlog::logger > make_logger( const std::string& a_name );
        std::shared_ptr< spdlog::logger > make_logger_async( const std::string& a_name );
        std::shared_ptr< spdlog::logger > make_logger_sync( const std::string& a_name );
        std::shared_ptr< spdlog::sinks::sink > f_sink;
    };

    struct SCARAB_API spd_initializer_stdout_color : spd_initializer
    {
        spd_initializer_stdout_color( const std::string& a_pattern = "" );
        std::shared_ptr< spdlog::logger > make_logger( const std::string& a_name );
        std::shared_ptr< spdlog::sinks::sink > f_sink;
    };

    class SCARAB_API logger
    {
        public:
            enum class ELevel : unsigned 
            {
                eTrace = unsigned(SPDLOG_LEVEL_TRACE),
                eDebug = unsigned(SPDLOG_LEVEL_DEBUG),
                eInfo = unsigned(SPDLOG_LEVEL_INFO),
                eProg = unsigned(SPDLOG_LEVEL_NOTICE),
                eWarn = unsigned(SPDLOG_LEVEL_WARN),
                eError = unsigned(SPDLOG_LEVEL_ERROR),
                eFatal = unsigned(SPDLOG_LEVEL_CRITICAL)
            };

        public:
            logger( const std::string& a_name );
            logger( const logger& ) = delete;
            logger( logger&& ) = default;
            virtual ~logger();

            logger& operator=( const logger& ) = delete;
            logger& operator=( logger&& ) = default;

            static std::set< logger* >& all_loggers();

            const std::string& name() const;

        protected:
            std::string f_name;

        public:
            ELevel get_threshold() const;
            void set_threshold( ELevel a_level );

            static ELevel get_global_threshold();
            static void set_global_threshold( ELevel a_threshold );

        protected:
            ELevel f_threshold;
            static ELevel& global_threshold();

        public:
            template< typename T >
            logger& operator<<( const T& data );

            std::string buffer();

            std::shared_ptr<spdlog::logger> spdlogger();

            static void stop_using_spd_async();
            static void reset_using_spd_async();
            static std::atomic_bool& using_spd_async();

        protected:
            mutable std::stringstream f_strstr;

            std::shared_ptr< spdlog::logger > f_spdlogger;

    };

    template< typename initializer_x >
    class logger_type : public logger
    {
        public:
            logger_type( const std::string& a_name, const std::string& a_file="[unknown]", int a_line=-1 );
            logger_type( const logger_type& ) = delete;
            logger_type( logger_type&& ) = default;
            virtual ~logger_type() = default;

            logger_type& operator=( const logger_type& ) = delete;
            logger_type& operator=( logger_type&& ) = default;

            initializer_x& initializer() { return *f_initializer_ptr; }
            const initializer_x& initializer() const { return *f_initializer_ptr; }

        protected:
            initializer_x* f_initializer_ptr;

    };

    template< typename initializer_x >
    logger_type< initializer_x >::logger_type( const std::string& a_name, const std::string& a_file, int a_line ) :
            logger( a_name ),
            f_initializer_ptr( nullptr )
    {
#ifdef SCARAB_LOGGER_DEBUG
        std::cerr << "[logger_type constructor] Logger <" << a_name << "> was initialized from <" << a_file << ":" << a_line << ">; type: " << ::scarab::type( *this ) << std::endl;
#endif
        // Start the backend, but only once
        static initializer_x s_init;
        f_initializer_ptr = &s_init;

        f_spdlogger = s_init.make_or_get_logger( a_name, [](const std::string& a_name) -> std::shared_ptr< spdlog::logger > { return s_init.make_logger( a_name ); } );
    }


    template< typename T >
    logger& logger::operator<<( const T& data )
    {
        f_strstr << data;
        return *this;
    }

}

// Logging Macros

// Logger creation

/// Creates a static scarab::logger object with variable name a_logger
/// Uses spdlog's asynchronous logger; spdlog logger name will be a_name.
#define LOGGER( a_logger, a_name ) \
    static ::scarab::logger_type< ::scarab::spd_initializer_async_stdout_color_mt > a_logger( a_name, __FILE_NAME__, __LINE__ );

/// Creates a static single-threaded (non-asynchronous) scarab::logger object with variable name a_logger
/// Uses spdlog's asynchronous logger; spdlog logger name will be a_name.
#define LOGGER_ST( a_logger, a_name ) \
    static ::scarab::logger_type< ::scarab::spd_initializer_stdout_color > a_logger( a_name, __FILE_NAME__, __LINE__ );

/// Creates a local scarab::logger object with variable name a_logger
/// Uses spdlog's asynchronous logger; spdlog logger name will be a_name.
//#define LOCAL_LOGGER( a_logger, a_name ) \
//    ::scarab::logger_type< ::scarab::spd_initializer_async_stdout_color_mt > a_logger( a_name, __FILE_NAME__, __LINE__ );

/// Creates a local single-threaded (non-asynchronous) scarab::logger object with variable name a_logger
/// Uses spdlog's basic logger; spdlog logger name will be a_name.
//#define LOCAL_LOGGER_ST( a_logger, a_name ) \
//    ::scarab::logger_type< ::scarab::spd_initializer_stdout_color > a_logger( a_name, __FILE_NAME__, __LINE__ );

// Logging functions
#ifdef NDEBUG
/// Log a message at the trace level (disabled when not in a debug build)
#define LTRACE(a_logger, ...)
/// Log a message at the debug level (disabled when not in a debug build)
#define LDEBUG(a_logger, ...)
#else
/// Log a message at the trace level
#define LTRACE(a_logger, ...)       SPDLOG_LOGGER_TRACE(    a_logger.spdlogger(), "{}", (a_logger << __VA_ARGS__).buffer() );
/// Log a message at the debug level
#define LDEBUG(a_logger, ...)       SPDLOG_LOGGER_DEBUG(    a_logger.spdlogger(), "{}", (a_logger << __VA_ARGS__).buffer() );
#endif
/// Log a message at the info level
#define LINFO(a_logger, ...)        SPDLOG_LOGGER_INFO(     a_logger.spdlogger(), "{}", (a_logger << __VA_ARGS__).buffer() );
/// Log a message at the prog(ress) level
#define LPROG(a_logger, ...)        SPDLOG_LOGGER_NOTICE(   a_logger.spdlogger(), "{}", (a_logger << __VA_ARGS__).buffer() );
/// Log a message at the warn(ing) level
#define LWARN(a_logger, ...)        SPDLOG_LOGGER_WARN(     a_logger.spdlogger(), "{}", (a_logger << __VA_ARGS__).buffer() );
/// Log a message at the error level
#define LERROR(a_logger, ...)       SPDLOG_LOGGER_ERROR(    a_logger.spdlogger(), "{}", (a_logger << __VA_ARGS__).buffer() );
/// Log a message at the fatal level
#define LFATAL(a_logger, ...)       SPDLOG_LOGGER_CRITICAL( a_logger.spdlogger(), "{}", (a_logger << __VA_ARGS__).buffer() );


#endif // SCARAB_LOGGER_HH_
