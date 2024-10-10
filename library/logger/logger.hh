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
    /// Starts the quill backend when the first logger is initialized; 
    /// Guarantees it's ready to receive log messages even during static initialization
    struct quill_initializer
    {
        quill_initializer();
        //~quill_initializer(); // uncomment if you'd like to print when the destructor is called
    };

    // Protects quill from being used after the execution phase (during static cleanup)
    // This was tested and didn't work for some reason.  Segfaults still occurred.
    // Instead the recommendation is to use the STOP_LOGGER macro at the end of an executable.
    //struct quill_guard : quill_initializer
    //{
    //    quill_guard() = default;
    //    ~quill_guard();
    //};


    /**
     \class logger
     \brief The console logger for scarab-based software

     \details
     The scarab logger provides a front end for logging to the console.

     The backend is provided by the Quill library (https://github.com/odygrd/quill)
     
     The primary interface for clients of the scarab logger is provided by a set of macros:

     First, a logger must be created:

     * LOGGER -- will create a static logger
     * LOCAL_LOGGER -- will create a non-static logger

     For logging the following macros can be used:

     * LTRACE(myLogger, "message");
     * LDEBUG(myLogger, "message");
     * LINFO(myLogger, "message");
     * LPROG(myLogger, "message");
     * LWARN(myLogger, "message");
     * LERROR(myLogger, "message");
     * FATAL(myLogger, "message");
     
     Messages may use the `<<` operator for including any types that can be converted to a string with a stringstream:
     ```
     int age = 5;
     LINFO(myLogger, "I am " << age << " years old");
     ```

     Due to interactions between the staticly-initialized scarab loggers and the statically-initialized Quill loggers, 
     the logging must be explicitly stopped at the end of execution, and loggers should not be used in post-execution 
     static cleanup.  To stop execution, the ``STOP_LOGGER`` macro can be used (or ``quill::Backend::stop()`` can be called).
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

            const std::string& name() const;

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
            void SetLevel(ELevel level);

            /**
             * Set a logger's minimum logging level to the global level
             */
            void UseGlobalLevel();

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

            typedef std::set< logger* > LoggerSet;
            static std::shared_ptr<LoggerSet>& AllLoggers();
            static unsigned& count();
            static ELevel& GlobalThreshold();
            static ELevel filterMinimumLevel(ELevel level);
        private:


            std::string fName;

            ELevel fThreshold;
            bool fThresholdIsGlobal;

            std::stringstream fStream;
            mutable std::string fBuffer;
            quill::Logger* qLogger;
    };


    inline const std::string& logger::name() const
    {
        return fName;
    }

} // end namespace scarab


// PRIVATE MACROS

/**
 \def __TO_QUILL
 \brief Private macro (not intended for client use) that converts between the scarab::logger interface and the Quill logger interface
 */
// this one would need to be adjusted before it works: logger --> a_logger; it uses a slightly different logic
//#define __TO_QUILL(logger, level, qlevel, ...) if(logger.IsLevelEnabled(scarab::logger::ELevel::e##level)) {logger << __VA_ARGS__; LOG_##qlevel(logger.quill(), logger.stream().str());} else {;}
// this one works
#define __TO_QUILL(a_logger, a_level, q_level, ...) if(a_logger.IsLevelEnabled(scarab::logger::ELevel::e##a_level)) {LOG_##q_level( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer());} else {;}
// this one works, but doesn't have logic for whether or not to print based on level
//#define __TO_QUILL(a_logger, a_level, q_level, ...) LOG_##q_level( a_logger.quill(), "{}", (a_logger.ref() << __VA_ARGS__).buffer());
// this one does nothing
//#define __TO_QUILL(a_logger, a_level, q_level, ...)

// PUBLIC MACROS

#define INITIALIZE_LOGGING    static scarab::quill_initializer quill_init;

#define STOP_LOGGING    quill::Backend::stop();

/**
 \brief Creates a static logger object.
 \note This logger should not be used during the static-initialization phase to avoid the static initialization order fiasco.
 \note Using this logger in a function called during the unloading phase (when static objects are destructed) can cause segfaults because objects used by the logger can be destructed before they're used.
 \param I static variable name for the logger
 \param K string name for the logger
 */
#define LOGGER(I,K)         static scarab::logger I(K);
/**
 \brief Creates a local (non-static) logger object; intended for use in situations where a static object could be problematic (e.g. during static initialization)
 \note Using this logger in a function called during the unloading phase (when static objects are destructed) can cause segfaults because objects used by the logger can be destructed before they're used.
 \param I static variable name for the logger
 \param K string name for the logger
 */
#define LOCAL_LOGGER(I,K)   scarab::logger I(K);

/**
 \def LTRACE
 \brief Print a trace-level log message
 \note Only available if compiled in DEBUG mode
 */
/**
 \def LDEBUG
 \brief Print a debug-level log message
 \note Only available if compiled in DEBUG mode
 */
/**
 \def LINFO
 \brief Print a info-level log message
 */
/**
 \def LPROG
 \brief Print a prog(ress)-level log message
 */
/**
 \def LWARN
 \brief Print a warning-level log message
 */
/**
 \def LERROR
 \brief Print a error-level log message
 */
/**
 \def LFATAL
 \brief Print a fatal-level log message
 */
#ifdef NDEBUG
#define LTRACE(a_logger, ...)
#define LDEBUG(a_logger, ...)
#else
#define LTRACE(a_logger, ...)       __TO_QUILL(a_logger, Trace, TRACE_L1, __VA_ARGS__ )
#define LDEBUG(a_logger, ...)       __TO_QUILL(a_logger, Debug, DEBUG, __VA_ARGS__ )
#endif
#define LINFO(a_logger, ...)        __TO_QUILL(a_logger, Info, INFO, __VA_ARGS__ )
#define LPROG(a_logger, ...)        __TO_QUILL(a_logger, Prog, NOTICE, __VA_ARGS__ )
#define LWARN(a_logger, ...)        __TO_QUILL(a_logger, Warn, WARNING, __VA_ARGS__ )
#define LERROR(a_logger, ...)       __TO_QUILL(a_logger, Error, ERROR, __VA_ARGS__ )
#define LFATAL(a_logger, ...)       __TO_QUILL(a_logger, Fatal, CRITICAL, __VA_ARGS__ )

#endif /* SCARAB_LOGGER_HH_ */
