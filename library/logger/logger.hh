/*
 * logger.hh
 * Based on KLogger.h, from KATRIN's Kasper
 *
 *  Created on: Jan 21, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_LOGGER_HH_
#define SCARAB_LOGGER_HH_

//#include "macros.hh"
#include "scarab_api.hh"

#include <quill/LogMacros.h>
#include <quill/Logger.h>

#include <cstring>
#include <iostream>
#include <sstream>


/**
 * @file
 * @brief Contains the logger class and macros, based on Kasper's KLogger class.
 * @date Created on: 18.11.2011
 * @author Marco Haag <marco.haag@kit.edu>
 *
 */

/*
// COLOR DEFINITIONS
#define COLOR_NORMAL "0"
#define COLOR_BRIGHT "1"
#define COLOR_FOREGROUND_RED "31"
#define COLOR_FOREGROUND_GREEN "32"
#define COLOR_FOREGROUND_YELLOW "33"
#define COLOR_FOREGROUND_BLUE "34"
#define COLOR_FOREGROUND_CYAN "36"
#define COLOR_FOREGROUND_WHITE "37"
#define COLOR_PREFIX "\033["
#define COLOR_SUFFIX "m"
#define COLOR_SEPARATOR ";"
*/

/**
 * The standard scarab namespace.
 */
namespace scarab
{

    /**
     * The scarab logger.
     *
     * The usage and syntax is inspired by log4j. logger itself uses the log4cxx library if it
     * was available on the system during compiling, otherwise it falls back to std::stream.
     *
     * The logger output can be configured in a file specified with the environment variable
     * @a LOGGER_CONFIGURATION (by default log4cxx.properties in the config directory).
     *
     * In most cases the following macro can be used
     * to instantiate a Logger in your code:
     * <pre>LOGGER(myLogger, "loggerName");</pre>
     *
     * This is equivalent to:
     * <pre>static mantis::logger myLogger("loggerName");</pre>
     *
     * For logging the following macros can be used. The source code location will then automatically
     * included in the output:
     *
     * <pre>
     * LOG(myLogger, level, "message");
     * LTRACE(myLogger, "message");
     * LDEBUG(myLogger, "message");
     * LINFO(myLogger, "message");
     * LPROG(myLogger, "message");
     * LWARN(myLogger, "message");
     * LERROR(myLogger, "message");
     * FATAL(myLogger, "message");
     *
     * </pre>
     *
     */
    class SCARAB_API logger
    {
        public:
            /**
             * \enum logger::ELevel
             * Defines the verbosity levels and their integer equivalents
             * Note that these values were selected to match with those used in the dripline_python project
             */
            enum class ELevel : unsigned 
            {
                eTrace = int(quill::LogLevel::TraceL1),
                eDebug = int(quill::LogLevel::Debug),
                eInfo = int(quill::LogLevel::Info),
                eProg = int(quill::LogLevel::Notice),
                eWarn = int(quill::LogLevel::Warning),
                eError = int(quill::LogLevel::Error),
                eFatal = int(quill::LogLevel::Critical)
            };

        public:
            static logger& GetRootLogger() {
                static logger rootLogger;
                return rootLogger;
            }

        public:
            /**
             * Standard constructor assigning a name to the logger instance.
             * @param name The logger name.
             */
            logger(const char* name = 0);
            /// @overload
            //logger(const std::string& name);

            virtual ~logger();

            /**
             * Check whether a certain log-level is enabled.
             * @param level The log level
             * @return
             */
            bool IsLevelEnabled(ELevel level) const;

            /**
             * Get the logger's minimum logging level
             * @return The log level
             */
            ELevel GetLevel() const;

            /**
             * Set a logger's minimum logging level
             * @param level The log level
             */
            void SetLevel(ELevel level) const;

            /**
             * Set a logger's minimum logging level to the global level
             */
            void UseGlobalLevel() const;

            /**
             * Get the global minimum logging level
             */
            static logger::ELevel GetGlobalLevel();

            /**
             * Set all loggers' minimum logging level
             * @param level The log level
             */
            static void SetGlobalLevel(ELevel level);

            /**
             * Set whether colored text will be used
             * @param flag Bool determining whether colored text will be used
             */
            static void SetColored(bool flag);

            logger& ref()
            {
                return *this;
            }

            template< typename T >
            logger& operator<<( const T& data )
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

            quill::Logger* quill()
            {
                return qLogger;
            }

        private:
            struct Private;
            std::shared_ptr<Private> fPrivate;

            std::stringstream fStream;
            mutable std::string fBuffer;
            quill::Logger* qLogger;
    };

}

// PRIVATE MACROS

////#define TO_QUILL(logger, level, qlevel, ...) if(logger.IsLevelEnabled(scarab::logger::ELevel::e##level)) {logger << __VA_ARGS__; LOG_##qlevel(logger.quill(), logger.stream().str());} else {;}
#define TO_QUILL(a_logger, a_level, q_level, ...) if(a_logger.IsLevelEnabled(scarab::logger::ELevel::e##a_level)) {LOG_##q_level( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer());} else {;}
//#define TO_QUILL(a_logger, a_level, q_level, ...) LOG_##q_level( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer());


// PUBLIC MACROS

#define LOGGER(I,K)      static scarab::logger I(K);

/**/
#ifdef NDEBUG
#define LTRACE(a_logger, ...)
#define LDEBUG(a_logger, ...)
#else
#define LTRACE(a_logger, ...)       TO_QUILL(a_logger, Trace, TRACE_L1, __VA_ARGS__ )
#define LDEBUG(a_logger, ...)       TO_QUILL(a_logger, Debug, DEBUG, __VA_ARGS__ )
#endif
#define LINFO(a_logger, ...)        TO_QUILL(a_logger, Info, INFO, __VA_ARGS__ )
#define LPROG(a_logger, ...)        TO_QUILL(a_logger, Prog, NOTICE, __VA_ARGS__ )
#define LWARN(a_logger, ...)        TO_QUILL(a_logger, Warn, WARNING, __VA_ARGS__ )
#define LERROR(a_logger, ...)       TO_QUILL(a_logger, Error, ERROR, __VA_ARGS__ )
#define LFATAL(a_logger, ...)       TO_QUILL(a_logger, Fatal, CRITICAL, __VA_ARGS__ )
/**//*
#define LTRACE(a_logger, ...)       LOG_TRACE_L1( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer() );
#define LDEBUG(a_logger, ...)       LOG_DEBUG( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer() );
#define LINFO(a_logger, ...)        LOG_INFO( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer() );
#define LPROG(a_logger, ...)        LOG_NOTICE( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer() );
#define LWARN(a_logger, ...)        LOG_WARNING( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer() );
#define LERROR(a_logger, ...)       LOG_ERROR( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer() );
#define LFATAL(a_logger, ...)       LOG_CRITICAL( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer() );
*/

#endif /* SCARAB_LOGGER_HH_ */
