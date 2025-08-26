

#ifndef YALA_YALA
#define YALA_YALA

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

#define PASTE2( x, y ) x##y
#define PASTE( x, y ) PASTE2( x, y )

namespace yala
{
    struct spd_initializer
    {
        spd_initializer();
        virtual std::shared_ptr< spdlog::logger > make_logger( const std::string& a_name ) = 0;
        std::shared_ptr< spdlog::sinks::sink > f_sink;
    };

    struct spd_initializer_async_stdout_color_mt : spd_initializer
    {
        spd_initializer_async_stdout_color_mt();
        std::shared_ptr< spdlog::logger > make_logger( const std::string& a_name );
    };

    struct spd_initializer_stdout_color : spd_initializer
    {
        spd_initializer_stdout_color();
        std::shared_ptr< spdlog::logger > make_logger( const std::string& a_name );
    };

    class logger
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

        f_spdlogger = s_init.make_logger( a_name );
        spdlog::register_logger( f_spdlogger );
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
//    if( ::yala::logger::using_quill() ) { \
//        ::std::stringstream t_strstr; \
//        t_strstr << __VA_ARGS__; \
//        LOG_INFO(a_logger.quill(), "{}", t_strstr.str()); \
//    } else { \
//        ::std::cout << "INFO: " << __VA_ARGS__ << ::std::endl; \
//    } 
//#define LINFO(a_logger, ...) \
//    if( ::yala::logger::using_quill() ) { \
//        a_logger << __VA_ARGS__; \
//        LOG_INFO(a_logger.quill(), "{}", a_logger.buffer()); \
//    } else { \
//        ::std::cout << "INFO: " << __VA_ARGS__ << ::std::endl; \
//    } 

/// Creates a static scarab::logger object with variable name a_logger; Quill Logger name will be a_name.
#define LOGGER(a_logger, a_name)         static ::yala::logger_type< spd_initializer_async_stdout_color_mt > a_logger(a_name);

/// Private macro for creating a single message string with a given logger object
#define _LOGGER_GET_STRING( a_logger, ... ) (a_logger << __VA_ARGS__).buffer()

#define _LOGGER_IF_PRINT( a_loglevel, a_stream, a_prefix, a_file, a_line_no, a_logger, a_message ) \
    if( a_loglevel >= a_logger.get_threshold() ) { \
        a_stream << a_prefix << a_file << ":" << a_line_no << " -> " << a_message << std::endl; \
    }

// These are the local-logging (i.e. w/o Quill) statements
#define _LOCAL_TRACE( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::yala::logger::ELevel::eTrace,  std::cout, "[TRACE] ",   a_file, a_line_no, a_logger, a_message );
#define _LOCAL_DEBUG( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::yala::logger::ELevel::eDebug,  std::cout, "[DEBUG] ",   a_file, a_line_no, a_logger, a_message );
#define _LOCAL_INFO(  a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::yala::logger::ELevel::eInfo,   std::cout, "[INFO] ",    a_file, a_line_no, a_logger, a_message );
#define _LOCAL_PROG(  a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::yala::logger::ELevel::eProg,   std::cout, "[PROG] ",    a_file, a_line_no, a_logger, a_message );
#define _LOCAL_WARN(  a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::yala::logger::ELevel::eWarn,   std::cerr, "[WARNING] ", a_file, a_line_no, a_logger, a_message );
#define _LOCAL_ERROR( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::yala::logger::ELevel::eError,  std::cerr, "[ERROR] ",   a_file, a_line_no, a_logger, a_message );
#define _LOCAL_FATAL( a_file, a_line_no, a_logger, a_message )  _LOGGER_IF_PRINT( ::yala::logger::ELevel::eFatal,  std::cerr, "[FATAL] ",   a_file, a_line_no, a_logger, a_message );

// These are the Quill logging statements that dispatch the log messages to Quill via Quill's logging macros
#define _SPD_TRACE( a_logger, a_message )  SPDLOG_LOGGER_TRACE(    a_logger.spdlogger(), "{}", a_message );
#define _SPD_DEBUG( a_logger, a_message )  SPDLOG_LOGGER_DEBUG(    a_logger.spdlogger(), "{}", a_message );
#define _SPD_INFO(  a_logger, a_message )  SPDLOG_LOGGER_INFO(     a_logger.spdlogger(), "{}", a_message );
#define _SPD_PROG(  a_logger, a_message )  SPDLOG_LOGGER_NOTICE(   a_logger.spdlogger(), "{}", a_message );
#define _SPD_WARN(  a_logger, a_message )  SPDLOG_LOGGER_WARN(     a_logger.spdlogger(), "{}", a_message );
#define _SPD_ERROR( a_logger, a_message )  SPDLOG_LOGGER_ERROR(    a_logger.spdlogger(), "{}", a_message );
#define _SPD_FATAL( a_logger, a_message )  SPDLOG_LOGGER_CRITICAL( a_logger.spdlogger(), "{}", a_message );

#define _LOGGER_IF_SPD(a_severity, a_file, a_line_no, a_logger, ...) \
    if( ::yala::logger::using_spd() ) { \
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




#endif
