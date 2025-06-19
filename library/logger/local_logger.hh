/*
 * logger.hh
 * Based on KLogger.h, from KATRIN's Kasper
 *
 *  Created on: Jan 21, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_LOCAL_LOGGER_HH_
#define SCARAB_LOCAL_LOGGER_HH_

#include "logger.hh"

#include <atomic>
#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>


/**
 * @file
 * @brief Contains the logger class and macros
 * @date Reborn on: Oct 4, 2024
 * @author N.S. Oblath
 * 
 * Basic use instructions:
 * 
 * 1. Create a logger with LOGGER or LOCAL_LOGGER
 * 2. Use STOP_LOGGING or scarab::quill_guard to stop the use of Quill before main() returns in your application
 * 3. Use L[level] macros to log messages
 * 
 * Notes on the logging system design:
 * 
 * * The logger was originally based on the Kasper KLogger class, from the KATRIN experiment. This established many of the interface 
 *   choices, including the use of the LOGGER macro, and the macros for the various logging levels.
 * * The logger was originally built from scratch, sending its messages to stdout and stderr directly.
 * * In 2024 it was recognized that the existing logger was incompatible with the pybind11 handling of log messages 
 *   because the latter was not compatible with logging from multiple threads.  The choice was made to find a logging library
 *   that could funnel the logging through a single thread.
 * * The Quill library was selected because of the similarity of the logging macros and levels to the existing macros and levels, 
 *   meaning that adapting it to the existing macros would be relatively easy.  Quill provides asynchronous logging via a separate logging thread, 
 *   satisfying the pybind11 needs.
 * * The scarab::logger class now stands as a thin wrapper on the quill::Logger objects.  scarab::logger is still what client code will interact with.
 * * The Quill backend logging thread is started up using an initializer object that's built statically in the scarab::logger constructor.  
 *   This guarantees that it will be started when the first logger object is created.
 * * Logging levels had to be adapted slightly: only quill::TRACE_L1 is used of the trace levels, and it's equivalent to scarab's TRACE; Quill's NOTICE level 
 *   is used for scarab's PROG level, and Quill's CRITICAL level is used for scarab's FATAL level.
 * * Logging colors are matched to Python's logging colors.
 * * The scarab::logger class also contains the mechanism for processing the streamer notation, using operator<< to compose log messages. 
 *   The message is buffered in the logger object using stringstream, and then passed to the quill::Logger.
 * * In scarab and scarab-based code, sometimes log messages are printed from objects that exist after the main execution has stopped, as they're destructed at 
 *   the static cleanup phase.  This is a problem for Quill, because at some point, the Quill logging backend thread is automatically stopped and garbage collected.  
 *   Therefore we implemented a system where the logging via Quill can be explicitly stopped, at which point the logging switches to using "local" logging: directly via cout and cerr.  
 *   This is why we have a series of macros for printing each log level to Quill and locallly.  
 * * Once Quill logging is stopped it's presumed that the application is exiting, so it's not possible to turn it back on.
 * * The effect of not stopping the Quill logging mixed with printing after main execution can be seen by e.g. removing the quill_guard from test_static_initialization.  
 *   Depending on runtime conditions, it can work, segfault, or hang indefinitely.
 * * The implementation of the logging macros and the reason it is like it is might be a bit confusing:
 *   * The main interface is via the L[level] macros.  These provide a way to conveniently specify the logging level, logger object, and 
 *     message being composed with operator<<, all of which is known at compile time.
 *   * The _LOGGER_IF_QUILL macro checks whether Quill is running to decide whether or not to use the Quill or local logging.
 *   * Quill logging is dispatched to Quill with one of the _QUILL_[level] macros.
 *   * Local logging is called with one of the _LOCAL_[level] macros.
 *   * Local logging is performed by the _LOGGING_IF_PRINT macro, which also checks the message severity against the logger's threshold to know whether it should print or not.
 * * It's sometimes useful to get synchronous printing, e.g. for debugging.  Therefore when scarab is built in DEBUG mode, a preprocessor flag tells Quill to 
 *   log synchronously.
 * * Debug and trace messages are not compiled when scarab is built in Release mode.  This is done by a compile-time choice of using empty DEBUG and TRACE macros 
 *   when in Release mode.  Quill has compile-time filtering capabilities too, but we don't use that because scarab::logger has whole setup with 
 *   function pointers that allow the Quill logging to be stopped.
 */

/**
 * The standard scarab namespace.
 */
namespace scarab
{
/*
    enum class ELevel : unsigned 
    {
        eTrace = unsigned(quill::LogLevel::TraceL1),
        eDebug = unsigned(quill::LogLevel::Debug),
        eInfo = unsigned(quill::LogLevel::Info),
        eProg = unsigned(quill::LogLevel::Notice),
        eWarn = unsigned(quill::LogLevel::Warning),
        eError = unsigned(quill::LogLevel::Error),
        eFatal = unsigned(quill::LogLevel::Critical)
    };
*/

    class local_logger
    {
        public:
            local_logger( const std::string& a_name );
            local_logger( const local_logger& ) = delete;
            local_logger( local_logger&& ) = default;
            ~local_logger();

            local_logger& operator=( const local_logger& ) = delete;
            local_logger& operator=( local_logger&& ) = default;

            //static std::set< logger* >& all_loggers();

        public:
            ELevel get_threshold() const;
            void set_threshold( ELevel a_level );

            //static ELevel get_global_threshold();
            //static void set_global_threshold( ELevel a_threshold );

        private:
            ELevel f_threshold;
            //static ELevel f_global_threshold;

        public:
            template< typename T >
            local_logger& operator<<( const T& data );

            local_logger& ref();
            const std::string& buffer();

        private:
            mutable std::stringstream f_buffer_stream;
            mutable std::string f_buffer;

    };

    inline local_logger& local_logger::ref()
    {
        return *this;
    }

    template< typename T >
    local_logger& local_logger::operator<<( const T& data )
    {
        f_buffer_stream << data;
        return *this;
    }

    inline const std::string& local_logger::buffer()
    {
        f_buffer = f_buffer_stream.str();
        //std::cerr << "Buffer: " << fBuffer << std::endl;
        f_buffer_stream.str( std::string() );
        return f_buffer;
    }

} // end namespace scarab
/*
/// Creates a static scarab::logger object with variable name a_logger; Quill Logger name will be a_name.
#define LOGGER(a_logger, a_name)         static ::scarab::logger a_logger(a_name);
*/
/// Creates a (non-static) scarab::logger object with variable name a_logger; Quill Logger name will be a_name.
#define LOCAL_LOGGER(a_logger, a_name)   ::scarab::local_logger a_logger(a_name);
/*
/// Stop Quill logging -- all loggers will switch to using stdout; this macro should be used just before returning from main().
#define STOP_LOGGING  ::scarab::logger::stop_quill();

/// Private macro for creating a single message string with a given logger object
#define _LOGGER_GET_STRING( a_logger, ... ) (a_logger.ref() << __VA_ARGS__).buffer()

/// Private macro used when local-logging (i.e. w/o Quill) to check the severity against the threshold and print the log message if it's above threshold
#define _LOGGER_IF_PRINT( a_loglevel, a_stream, a_prefix, a_file, a_line_no, a_logger, a_message ) \
    if( a_loglevel >= a_logger.get_threshold() ) { \
        a_stream << a_prefix << a_file << ":" << a_line_no << " -> " << a_message << std::endl; \
    }

// These are the local-logging (i.e. w/o Quill) statements
#define _LOCAL_TRACE( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::ELevel::eTrace,  std::cout, "[TRACE] ",   a_file, a_line_no, a_logger, a_message );
#define _LOCAL_DEBUG( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::ELevel::eDebug,  std::cout, "[DEBUG] ",   a_file, a_line_no, a_logger, a_message );
#define _LOCAL_INFO(  a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::ELevel::eInfo,   std::cout, "[INFO] ",    a_file, a_line_no, a_logger, a_message );
#define _LOCAL_PROG(  a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::ELevel::eProg,   std::cout, "[PROG] ",    a_file, a_line_no, a_logger, a_message );
#define _LOCAL_WARN(  a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::ELevel::eWarn,   std::cerr, "[WARNING] ", a_file, a_line_no, a_logger, a_message );
#define _LOCAL_ERROR( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::ELevel::eError,  std::cerr, "[ERROR] ",   a_file, a_line_no, a_logger, a_message );
#define _LOCAL_FATAL( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::scarab::ELevel::eFatal,  std::cerr, "[FATAL] ",   a_file, a_line_no, a_logger, a_message );

// These are the Quill logging statements that dispatch the log messages to Quill via Quill's logging macros
#define _QUILL_TRACE( a_logger, a_message )  LOG_TRACE_L1( a_logger.quill(), "{}", a_message );
#define _QUILL_DEBUG( a_logger, a_message )  LOG_DEBUG(    a_logger.quill(), "{}", a_message );
#define _QUILL_INFO( a_logger,  a_message )  LOG_INFO(     a_logger.quill(), "{}", a_message );
#define _QUILL_PROG( a_logger,  a_message )  LOG_NOTICE(   a_logger.quill(), "{}", a_message );
#define _QUILL_WARN( a_logger,  a_message )  LOG_WARNING(  a_logger.quill(), "{}", a_message );
#define _QUILL_ERROR( a_logger, a_message )  LOG_ERROR(    a_logger.quill(), "{}", a_message );
#define _QUILL_FATAL( a_logger, a_message )  LOG_CRITICAL( a_logger.quill(), "{}", a_message );

/// Private macro that checks whether Quill is running to decide whether to use Quill or local logging
#define _LOGGER_IF_QUILL(a_severity, a_file, a_line_no, a_logger, ...) \
    if( ::scarab::logger::is_quill_stopped() ) { \
        std::cerr << "Local logging" << std::endl; \
        PASTE(_LOCAL_, a_severity)( a_file, a_line_no, a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) ); \
    } else { \
        std::cerr << "Quill logging" << std::endl; \
        PASTE(_QUILL_, a_severity)( a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) ); \
    }
*/
// Public logging functions
#ifdef NDEBUG
/// Log a message at the trace level (disabled when not in a debug build)
#define LOCAL_LTRACE(a_logger, ...)
/// Log a message at the debug level (disabled when not in a debug build)
#define LOCAL_LDEBUG(a_logger, ...)
#else
/// Log a message at the trace level
#define LOCAL_LTRACE(a_logger, ...)       _LOCAL_TRACE( __FILE_NAME__, __LINE__, a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) );
/// Log a message at the debug level
#define LOCAL_LDEBUG(a_logger, ...)       _LOCAL_DEBUG( __FILE_NAME__, __LINE__, a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) );
#endif
/// Log a message at the info level
#define LOCAL_LINFO(a_logger, ...)        _LOCAL_INFO(  __FILE_NAME__, __LINE__, a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) );
/// Log a message at the prog(ress) level
#define LOCAL_LPROG(a_logger, ...)        _LOCAL_PROG(  __FILE_NAME__, __LINE__, a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) );
/// Log a message at the warn(ing) level
#define LOCAL_LWARN(a_logger, ...)        _LOCAL_WARN(  __FILE_NAME__, __LINE__, a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) );
/// Log a message at the error level
#define LOCAL_LERROR(a_logger, ...)       _LOCAL_ERROR( __FILE_NAME__, __LINE__, a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) );
/// Log a message at the fatal level
#define LOCAL_LFATAL(a_logger, ...)       _LOCAL_FATAL( __FILE_NAME__, __LINE__, a_logger, _LOGGER_GET_STRING( a_logger, __VA_ARGS__ ) );

#endif // SCARAB_LOGGER_HH_
