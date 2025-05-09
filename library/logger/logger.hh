/*
 * logger.hh
 * Based on KLogger.h, from KATRIN's Kasper
 *
 *  Created on: Jan 21, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_LOGGER_HH_
#define SCARAB_LOGGER_HH_

#include "macros.hh"
#include "scarab_api.hh"

#include <quill/LogMacros.h>
#include <quill/Logger.h>

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>


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
 * Notes on the logging system design
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
 *   Therefore we implemented a system where the logging via Quill can be explicitly stopped, at which point the logging switches to using stdout.  
 *   This is why we have a series of function pointers and separate functions for printing each log level to Quill and to stdout.  Once Quill logging is stopped, 
 *   it's presumed that the application is exiting, so it's not possible to turn it back on.
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
 *
 */

/**
 * The standard scarab namespace.
 */
namespace scarab
{
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


    /// Starts the quill backend when the first logger is initialized; 
    /// Guarantees it's ready to receive log messages even during static initialization
    struct quill_initializer
    {
        quill_initializer();
        //~quill_initializer(); // uncomment if you'd like to print when the destructor is called
    };

    class logger
    {
        public:
            logger( const std::string& a_name );
            ~logger();

            quill::Logger* quill()
            {
                return f_quill;
            }

            logger& ref()
            {
                return *this;
            }

            template< typename T >
            logger& operator<<( const T& data )
            {
                f_stream << data;
                return *this;
            }

            const std::string& buffer()
            {
                f_buffer = f_stream.str();
                //std::cerr << "Buffer: " << fBuffer << std::endl;
                f_stream.str( std::string() );
                return f_buffer;
            }

        private:
            mutable std::stringstream f_stream;
            mutable std::string f_buffer;
            quill::Logger* f_quill;

        public:
            static ELevel get_global_threshold();
            static void set_global_threshold( ELevel a_threshold );

        private:
            static ELevel f_global_threshold;

        public:
            /// Stops logging via Quill and switches logging functions to stdout
            /// Sets flag s_quill_stopped so that if other loggers are created, they won't use Quill
            static void stop_quill();

            static bool is_quill_stopped();

        private:
            static bool s_quill_stopped;

        public:
            static void (*s_log_trace_fcn)(logger&, const std::string&);
            static void (*s_log_debug_fcn)(logger&, const std::string&);
            static void (*s_log_info_fcn)(logger&, const std::string&);
            static void (*s_log_prog_fcn)(logger&, const std::string&);
            static void (*s_log_warn_fcn)(logger&, const std::string&);
            static void (*s_log_error_fcn)(logger&, const std::string&);
            static void (*s_log_fatal_fcn)(logger&, const std::string&);

            static void log_trace_to_quill( logger& a_logger, const std::string& a_message );
            static void log_trace_to_stdout( logger& /*a_logger*/, const std::string& a_message );
        
            static void log_debug_to_quill( logger& a_logger, const std::string& a_message );
            static void log_debug_to_stdout( logger& /*a_logger*/, const std::string& a_message );
        
            static void log_info_to_quill( logger& a_logger, const std::string& a_message );
            static void log_info_to_stdout( logger& /*a_logger*/, const std::string& a_message );
        
            static void log_prog_to_quill( logger& a_logger, const std::string& a_message );
            static void log_prog_to_stdout( logger& /*a_logger*/, const std::string& a_message );
        
            static void log_warn_to_quill( logger& a_logger, const std::string& a_message );
            static void log_warn_to_stdout( logger& /*a_logger*/, const std::string& a_message );
        
            static void log_error_to_quill( logger& a_logger, const std::string& a_message );
            static void log_error_to_stdout( logger& /*a_logger*/, const std::string& a_message );
        
            static void log_fatal_to_quill( logger& a_logger, const std::string& a_message );
            static void log_fatal_to_stdout( logger& /*a_logger*/, const std::string& a_message );

            static void check_log_functions();
        
    };

} // end namespace scarab

#define LOGGER(a_logger, a_name)         static scarab::logger a_logger(a_name);

#define LOCAL_LOGGER(a_logger, a_name)   scarab::logger a_logger(a_name);

//#define LOGGER_GET_STRING(a_logger, one_string) one_string
#define LOGGER_GET_STRING(a_logger, ...) (a_logger.ref() << __VA_ARGS__).buffer()


#ifdef NDEBUG
#define LTRACE(a_logger, ...)
#define LDEBUG(a_logger, ...)
#else
#define LTRACE(a_logger, ...)       scarab::logger::s_log_trace_fcn( a_logger, LOGGER_GET_STRING(a_logger, __VA_ARGS__) )
#define LDEBUG(a_logger, ...)       scarab::logger::s_log_debug_fcn( a_logger, LOGGER_GET_STRING(a_logger, __VA_ARGS__) )
#endif
#define LINFO(a_logger, ...)        scarab::logger::s_log_info_fcn( a_logger, LOGGER_GET_STRING(a_logger, __VA_ARGS__) )
#define LPROG(a_logger, ...)        scarab::logger::s_log_prog_fcn( a_logger, LOGGER_GET_STRING(a_logger, __VA_ARGS__) )
#define LWARN(a_logger, ...)        scarab::logger::s_log_warn_fcn( a_logger, LOGGER_GET_STRING(a_logger, __VA_ARGS__) )
#define LERROR(a_logger, ...)       scarab::logger::s_log_error_fcn( a_logger, LOGGER_GET_STRING(a_logger, __VA_ARGS__) )
#define LFATAL(a_logger, ...)       scarab::logger::s_log_fatal_fcn( a_logger, LOGGER_GET_STRING(a_logger, __VA_ARGS__) )


    // Protects quill from being used after the execution phase (during static cleanup)
    // This was tested and didn't work for some reason.  Segfaults still occurred.
    // Instead the recommendation is to use the STOP_LOGGER macro at the end of an executable.
    //struct quill_guard : quill_initializer
    //{
    //    quill_guard() = default;
    //    ~quill_guard();
    //};

#endif // SCARAB_LOGGER_HH_
