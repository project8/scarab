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
 * \note This logger was originally based on Kasper's KLogger class.
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

    class logger_helper
    {
        public:
            logger_helper( quill::Logger*& a_logger, const std::string& a_name );
            ~logger_helper();

            logger_helper& ref()
            {
                return *this;
            }

            template< typename T >
            logger_helper& operator<<( const T& data )
            {
                fStream << data;
                return *this;
            }

            const std::string& buffer()
            {
                fBuffer = fStream.str();
                //std::cerr << "Buffer: " << fBuffer << std::endl;
                fStream.str( std::string() );
                return fBuffer;
            }

        private:
            mutable std::stringstream fStream;
            mutable std::string fBuffer;

        public:
            static ELevel get_global_threshold();
            static void set_global_threshold( ELevel a_threshold );

        private:
            static ELevel f_global_threshold;


    };

} // end namespace scarab

#define LOGGER(a_logger, a_name)         static quill::Logger* a_logger;  static scarab::logger_helper PASTE(logger_helper_, a_logger)(a_logger, a_name);

#define LOCAL_LOGGER(a_logger, a_name)   quill::Logger* a_logger;  scarab::logger_helper PASTE(logger_helper_, a_logger)(a_logger, a_name);

//#define LOGGER_GET_STRING(a_logger, one_string) one_string
#define LOGGER_GET_STRING(a_logger, ...) (PASTE(logger_helper_, a_logger).ref() << __VA_ARGS__).buffer()


//#ifdef NDEBUG
//#define LTRACE(a_logger, ...)
//#define LDEBUG(a_logger, ...)
//#else
//#define LTRACE(a_logger, ...)       LOG_TRACE_L1(a_logger, "{}", "hello trace")
//#define LDEBUG(a_logger, ...)       LOG_DEBUG(a_logger, "{}", "hello debug")
//#endif
//#define LINFO(a_logger, ...)        LOG_INFO(a_logger, "{}", "hello info")
//#define LPROG(a_logger, ...)        LOG_NOTICE(a_logger, "{}", "hello notice")
//#define LWARN(a_logger, ...)        LOG_WARNING(a_logger, "{}", "hello warning")
//#define LERROR(a_logger, ...)       LOG_ERROR(a_logger, "{}", "hello error")
//#define LFATAL(a_logger, ...)       LOG_CRITICAL(a_logger, "{}", "hello critical")

#ifdef NDEBUG
#define LTRACE(a_logger, ...)
#define LDEBUG(a_logger, ...)
#else
#define LTRACE(a_logger, ...)       LOG_TRACE_L1(a_logger, "{}", LOGGER_GET_STRING(a_logger, __VA_ARGS__))
#define LDEBUG(a_logger, ...)       LOG_DEBUG(a_logger, "{}", LOGGER_GET_STRING(a_logger, __VA_ARGS__))
#endif
#define LINFO(a_logger, ...)        LOG_INFO(a_logger, "{}", LOGGER_GET_STRING(a_logger, __VA_ARGS__))
#define LPROG(a_logger, ...)        LOG_NOTICE(a_logger, "{}", LOGGER_GET_STRING(a_logger, __VA_ARGS__))
#define LWARN(a_logger, ...)        LOG_WARNING(a_logger, "{}", LOGGER_GET_STRING(a_logger, __VA_ARGS__))
#define LERROR(a_logger, ...)       LOG_ERROR(a_logger, "{}", LOGGER_GET_STRING(a_logger, __VA_ARGS__))
#define LFATAL(a_logger, ...)       LOG_CRITICAL(a_logger, "{}", LOGGER_GET_STRING(a_logger, __VA_ARGS__))

    // Protects quill from being used after the execution phase (during static cleanup)
    // This was tested and didn't work for some reason.  Segfaults still occurred.
    // Instead the recommendation is to use the STOP_LOGGER macro at the end of an executable.
    //struct quill_guard : quill_initializer
    //{
    //    quill_guard() = default;
    //    ~quill_guard();
    //};

#endif // SCARAB_LOGGER_HH_
