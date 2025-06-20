

#ifndef YALA_YALA
#define YALA_YALA

#include <atomic>
#include <iostream>
#include <set>
#include <string>
#include <sstream>

#include "quill/Logger.h"
#include "quill/LogMacros.h"


namespace yala
{
    class logger
    {
        public:
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

            struct quill_initializer
            {
                quill_initializer();
            };

        public:
            logger( const std::string& a_name );
            logger( const logger& ) = delete;
            logger( logger&& ) = default;
            ~logger();

            logger& operator=( const logger& ) = delete;
            logger& operator=( logger&& ) = default;

            static std::set< logger* >& all_loggers();

        public:
            ELevel get_threshold() const;
            void set_threshold( ELevel a_level );

            static ELevel get_global_threshold();
            static void set_global_threshold( ELevel a_threshold );

        private:
            ELevel f_threshold;
            static ELevel& global_threshold();

        public:
            template< typename T >
            logger& operator<<( const T& data );

            std::string buffer();

            quill::Logger* quill();

            static void stop_using_quill();
            static std::atomic_bool& using_quill();

        private:
            mutable std::stringstream f_strstr;

            quill::Logger* f_quill;

    };

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
#define LINFO(a_logger, ...) \
    if( ::yala::logger::using_quill() ) { \
        a_logger << __VA_ARGS__; \
        LOG_INFO(a_logger.quill(), "{}", a_logger.buffer()); \
    } else { \
        ::std::cout << "INFO: " << __VA_ARGS__ << ::std::endl; \
    } 


#endif
