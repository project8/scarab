/*
 * logger.hh
 *
 *  Created on: Jul 10, 2025
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_LOGGER_HH_
#define SCARAB_LOGGER_HH_

#include <atomic>
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

/**
 * @file logger.hh
 * @brief Contains the logger class and macros
 * @date Reborn on: Jul 10, 2025
 * @author N.S. Oblath
 * 
 * Basic use instructions:
 * 
 * 1. Create a logger with LOGGER or LOCAL_LOGGER
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
 * * The scarab::logger class now stands as a thin wrapper on the spdlog::logger objects.  scarab::logger is still what client code will interact with.
 * * The Quill backend logging thread is started up using an initializer object that's built statically in the scarab::logger constructor.  
 *   This guarantees that it will be started when the first logger object is created.
 * * Logging levels had to be adapted slightly: only quill::TRACE_L1 is used of the trace levels, and it's equivalent to scarab's TRACE; Quill's NOTICE level 
 *   is used for scarab's PROG level, and Quill's CRITICAL level is used for scarab's FATAL level.
 * * Logging colors are matched to Python's logging colors.
 * * The scarab::logger class also contains the mechanism for processing the streamer notation, using operator<< to compose log messages. 
 *   The message is buffered in the logger object using stringstream, and then passed to the quill::Logger.
 * * In scarab and scarab-based code, sometimes log messages are printed from objects that exist after the main execution has stopped, as they're destructed at 
 *   the static cleanup phase.  This is a problem for Quill, because at some point, the Quill logging backend thread is automatically stopped and garbage collected.  
 *   Therefore we implemented a system where the logging via Quill can be explicitly stopped, at which point the logging switches to using stdout.  
 *   This is why we have a series of function pointers and separate functions for printing each log level to Quill and to stdout.  Once Quill logging is stopped, 
 *   it's presumed that the application is exiting, so it's not possible to turn it back on.
 * * The effect of not stopping the Quill logging mixed with printing after main execution can be seen by e.g. removing the quill_guard from test_static_initialization.  
 *   Depending on runtime conditions, it can work, segfault, or hang indefinitely.
 * * The implementation of the logging macros and the reason it is like it is might be a bit confusing:
 *   * The main interface is via the L[level] macros.  These provide a way to conveniently specify the logging level, logger object, and 
 *     message being composed with operator<<, all of which is known at compile time.
 *   * The L[level] macros "call" one of the scarab::logger::s_log_[level]_fcn function pointers. We want to use a function pointer so that we can switch between 
 *     logging via Quill and logging via stdout at runtime.
 *   * Each function pointer points to either scarab::logger::log_level_to_quill() or scarab::logger::log_level_to_stdout().
 *   * The logging functions are provided with a logging message in the form of a std::string, which is composed using the LOGGER_GET_STRING macro; 
 *     this macro takes the variable macro arguments and creates a single string using the operator<< mechanism.
 *   * The scarab::logger::log_[level]_to_quill() functions use Quill's logging macros to pass the messages to Quill.
 *   * The scarab::logger::log_[level]_to_stdout() functions use std::cout to print the messages.
 * * It's sometimes useful to get synchronous printing, e.g. for debugging.  Therefore when scarab is built in DEBUG mode, a preprocessor flag tells Quill to 
 *   log synchronously.
 * * Debug and trace messages are not compiled when scarab is built in Release mode.  This is done by a compile-time choice of using empty DEBUG and TRACE macros 
 *   when in Release mode.  Quill has compile-time filtering capabilities too, but we don't use that because scarab::logger has whole setup with 
 *   function pointers that allow the Quill logging to be stopped.
 * * TODO: Individual logger verbosity can be controlled by requesting that logger via the string name that's provided in the LOGGER macro and an ELevel value.
 */

namespace scarab
{
    struct SCARAB_API spd_initializer
    {
        spd_initializer();
        std::shared_ptr< spdlog::logger > make_or_get_logger( const std::string& a_name );
        virtual std::shared_ptr< spdlog::logger > make_logger( const std::string& a_name ) = 0;
        std::shared_ptr< spdlog::sinks::sink > f_sink;
    };

    struct SCARAB_API spd_initializer_async_stdout_color_mt : spd_initializer
    {
        spd_initializer_async_stdout_color_mt();
        std::shared_ptr< spdlog::logger > make_logger( const std::string& a_name );
    };

    struct SCARAB_API spd_initializer_stdout_color : spd_initializer
    {
        spd_initializer_stdout_color();
        std::shared_ptr< spdlog::logger > make_logger( const std::string& a_name );
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

            static void stop_using_spd();
            static void reset_using_spd();
            static std::atomic_bool& using_spd();

        protected:
            mutable std::stringstream f_strstr;

            std::shared_ptr< spdlog::logger > f_spdlogger;

    };

    template< typename initializer_x >
    class logger_type : public logger
    {
        public:
            logger_type( const std::string& a_name );
            logger_type( const logger_type& ) = delete;
            logger_type( logger_type&& ) = default;
            virtual ~logger_type() = default;

            logger_type& operator=( const logger_type& ) = delete;
            logger_type& operator=( logger_type&& ) = default;

    };

    template< typename initializer_x >
    logger_type< initializer_x >::logger_type( const std::string& a_name ) :
            logger( a_name )
    {
        // Start the backend, but only once
        static initializer_x s_init;

        f_spdlogger = s_init.make_or_get_logger( a_name );
    }


    template< typename T >
    logger& logger::operator<<( const T& data )
    {
        f_strstr << data;
        return *this;
    }

}

    //quill::Logger* use_logger( const std::string& a_name );

//#define LINFO(logger, ...)  LOG_INFO(logger, "{}", [](){std::stringstream t_strstr; t_strstr << __VA_ARGS__; return t_strstr.str();});
//#define LINFO(a_logger, ...) \
//    if( ::scarab::logger::using_quill() ) { \
//        ::std::stringstream t_strstr; \
//        t_strstr << __VA_ARGS__; \
//        LOG_INFO(a_logger.quill(), "{}", t_strstr.str()); \
//    } else { \
//        ::std::cout << "INFO: " << __VA_ARGS__ << ::std::endl; \
//    } 
//#define LINFO(a_logger, ...) \
//    if( ::scarab::logger::using_quill() ) { \
//        a_logger << __VA_ARGS__; \
//        LOG_INFO(a_logger.quill(), "{}", a_logger.buffer()); \
//    } else { \
//        ::std::cout << "INFO: " << __VA_ARGS__ << ::std::endl; \
//    } 

/// Creates a static scarab::logger object with variable name a_logger
/// Uses spdlog's asynchronous logger; spdlog logger name will be a_name.
#define LOGGER( a_logger, a_name )         static ::scarab::logger_type< ::scarab::spd_initializer_async_stdout_color_mt > a_logger( a_name );
#define LOCAL_LOGGER( a_logger, a_name )          ::scarab::logger_type< ::scarab::spd_initializer_stdout_color > a_logger( a_name );

/// Private macro for creating a single message string with a given logger object
#define _LOGGER_GET_STRING( a_logger, ... ) (a_logger << __VA_ARGS__).buffer()

#define _LOGGER_IF_PRINT( a_loglevel, a_stream, a_prefix, a_file, a_line_no, a_logger, a_message ) \
    if( a_loglevel >= a_logger.get_threshold() ) { \
        a_stream << a_prefix << a_file << ":" << a_line_no << " -> " << a_message << std::endl; \
    }

// These are the local-logging (i.e. w/o Quill) statements
#define _LOCAL_TRACE( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::logger::ELevel::eTrace,  std::cout, "[TRACE] ",   a_file, a_line_no, a_logger, a_message );
#define _LOCAL_DEBUG( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::logger::ELevel::eDebug,  std::cout, "[DEBUG] ",   a_file, a_line_no, a_logger, a_message );
#define _LOCAL_INFO(  a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::logger::ELevel::eInfo,   std::cout, "[INFO] ",    a_file, a_line_no, a_logger, a_message );
#define _LOCAL_PROG(  a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::logger::ELevel::eProg,   std::cout, "[PROG] ",    a_file, a_line_no, a_logger, a_message );
#define _LOCAL_WARN(  a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::logger::ELevel::eWarn,   std::cerr, "[WARNING] ", a_file, a_line_no, a_logger, a_message );
#define _LOCAL_ERROR( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::logger::ELevel::eError,  std::cerr, "[ERROR] ",   a_file, a_line_no, a_logger, a_message );
#define _LOCAL_FATAL( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::logger::ELevel::eFatal,  std::cerr, "[FATAL] ",   a_file, a_line_no, a_logger, a_message );

// These are the Quill logging statements that dispatch the log messages to Quill via Quill's logging macros
#define _SPD_TRACE( a_logger, a_message )  SPDLOG_LOGGER_TRACE(    a_logger.spdlogger(), "{}", a_message );
#define _SPD_DEBUG( a_logger, a_message )  SPDLOG_LOGGER_DEBUG(    a_logger.spdlogger(), "{}", a_message );
#define _SPD_INFO(  a_logger, a_message )  SPDLOG_LOGGER_INFO(     a_logger.spdlogger(), "{}", a_message );
#define _SPD_PROG(  a_logger, a_message )  SPDLOG_LOGGER_NOTICE(   a_logger.spdlogger(), "{}", a_message );
#define _SPD_WARN(  a_logger, a_message )  SPDLOG_LOGGER_WARN(     a_logger.spdlogger(), "{}", a_message );
#define _SPD_ERROR( a_logger, a_message )  SPDLOG_LOGGER_ERROR(    a_logger.spdlogger(), "{}", a_message );
#define _SPD_FATAL( a_logger, a_message )  SPDLOG_LOGGER_CRITICAL( a_logger.spdlogger(), "{}", a_message );

#define _LOGGER_IF_SPD(a_severity, a_file, a_line_no, a_logger, ...) \
    if( ::scarab::logger::using_spd() ) { \
        std::cerr << "spd logging" << std::endl; \
        PASTE(_SPD_, a_severity)( a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) ); \
    } else { \
        std::cerr << "Local logging" << std::endl; \
        PASTE(_LOCAL_, a_severity)( a_file, a_line_no, a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) ); \
    }

// Public logging functions
#ifdef NDEBUG
/// Log a message at the trace level (disabled when not in a debug build)
#define LTRACE(a_logger, ...)
/// Log a message at the debug level (disabled when not in a debug build)
#define LDEBUG(a_logger, ...)
#else
/// Log a message at the trace level
#define LTRACE(a_logger, ...)       _LOGGER_IF_SPD( TRACE, __FILE_NAME__, __LINE__, a_logger, __VA_ARGS__ );
/// Log a message at the debug level
#define LDEBUG(a_logger, ...)       _LOGGER_IF_SPD( DEBUG, __FILE_NAME__, __LINE__, a_logger, __VA_ARGS__ );
#endif
/// Log a message at the info level
#define LINFO(a_logger, ...)        _LOGGER_IF_SPD( INFO,  __FILE_NAME__, __LINE__, a_logger, __VA_ARGS__ );
/// Log a message at the prog(ress) level
#define LPROG(a_logger, ...)        _LOGGER_IF_SPD( PROG,  __FILE_NAME__, __LINE__, a_logger, __VA_ARGS__ );
/// Log a message at the warn(ing) level
#define LWARN(a_logger, ...)        _LOGGER_IF_SPD( WARN,  __FILE_NAME__, __LINE__, a_logger, __VA_ARGS__ );
/// Log a message at the error level
#define LERROR(a_logger, ...)       _LOGGER_IF_SPD( ERROR, __FILE_NAME__, __LINE__, a_logger, __VA_ARGS__ );
/// Log a message at the fatal level
#define LFATAL(a_logger, ...)       _LOGGER_IF_SPD( FATAL, __FILE_NAME__, __LINE__, a_logger, __VA_ARGS__ );




#endif // SCARAB_LOGGER_HH_
