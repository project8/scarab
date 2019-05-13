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

/**
 * @file
 * @brief Contains the logger class and macros, based on Kasper's KLogger class.
 * @date Created on: 18.11.2011
 * @author Marco Haag <marco.haag@kit.edu>
 *
 */

// UTILITY MACROS

#ifndef LOGGER_UTILITY_MACROS_
#define LOGGER_UTILITY_MACROS_

#define msvc_bug_fix(m, args) m args
#define expand(x) x

//#define va_num_args(...) va_num_args_(__VA_ARGS__, 5,4,3,2,1)
//#define va_num_args_(...) msvc_bug_fix(va_num_args_impl, (__VA_ARGS__))
#define va_num_args(...) expand( va_num_args_impl(__VA_ARGS__, 5,4,3,2,1))
#define va_num_args_impl(_1,_2,_3,_4,_5,N,...) N


#define macro_dispatcher(func, ...) macro_dispatcher_(func, va_num_args(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func ## nargs

#endif  /* LOGGER_UTILITY_MACROS_ */

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

// INCLUDES

#include <cstring>
#include <iostream>
#include <sstream>

// CLASS DEFINITIONS

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
     * ASSERT(myLogger, assertion, "message");
     *
     * LOG_ONCE(myLogger, level, "message");
     * TRACE_ONCE(myLogger, "message");
     * DEBUG_ONCE(myLogger, "message");
     * INFO_ONCE(myLogger, "message");
     * PROG_ONCE(myLogger, "message");
     * WARN_ONCE(myLogger, "message");
     * ERROR_ONCE(myLogger, "message");
     * FATAL_ONCE(myLogger, "message");
     * </pre>
     *
     */
    class SCARAB_API logger
    {
        public:
            enum ELevel {
                eTrace = 0,
                eDebug = 1,
                eInfo = 2,
                eProg = 3,
                eWarn = 4,
                eError = 5,
                eFatal = 6
            };

        public:
            /**
             * A simple struct used by the Logger macros to pass information about the filename and line number.
             * Not to be used directly by the user!
             */
            struct Location {
                Location(const char* const fileName = "", const char* const functionName = "", int lineNumber = -1) :
                    fLineNumber(lineNumber), fFileName(fileName), fFunctionName(functionName)
                { }
                int fLineNumber;
                std::string fFileName;
                std::string fFunctionName;
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
            logger(const std::string& name);

            virtual ~logger();

            /**
             * Check whether a certain log-level is enabled.
             * @param level The log level
             * @return
             */
            bool IsLevelEnabled(ELevel level) const;

            /**
             * Set a logger's minimum logging level
             * @param level The log level
             */
            void SetLevel(ELevel level) const;

            /**
             * Set all loggers' minimum logging level
             * @param level The log level
             */
            void SetGlobalLevel(ELevel level) const;

            /**
             * Set whether colored text will be used
             * @param flag Bool determining whether colored text will be used
             */
            static void SetColored(bool flag);

            /**
             * Set the ostream pointer used for standard output messages
             * @param stream Stream object for standard output
             */
            static void SetOutStream(std::ostream* stream);

            /**
             * Set the ostream pointer used for standard error messages
             * @param stream Stream object for standard errors
             */
            static void SetErrStream(std::ostream* stream);

            /**
             * Log a message with the specified level.
             * Use the macro LOG(logger, level, message).
             * @param level The log level.
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void Log(ELevel level, const std::string& message, const Location& loc = Location());

            /**
             * Log a message at TRACE level.
             * Use the macro LTRACE(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogTrace(const std::string& message, const Location& loc = Location())
            {
                Log(eTrace, message, loc);
            }
            /**
             * Log a message at DEBUG level.
             * Use the macro LDEBUG(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogDebug(const std::string& message, const Location& loc = Location())
            {
                Log(eDebug, message, loc);
            }
            /**
             * Log a message at INFO level.
             * Use the macro LINFO(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogInfo(const std::string& message, const Location& loc = Location())
            {
                Log(eInfo, message, loc);
            }
            /**
             * Log a message at PROG level.
             * Use the macro PROG(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogProg(const std::string& message, const Location& loc = Location())
            {
                Log(eProg, message, loc);
            }
            /**
             * Log a message at WARN level.
             * Use the macro LWARN(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogWarn(const std::string& message, const Location& loc = Location())
            {
                Log(eWarn, message, loc);
            }
            /**
             * Log a message at ERROR level.
             * Use the macro LERROR(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogError(const std::string& message, const Location& loc = Location())
            {
                Log(eError, message, loc);
            }
            /**
             * Log a message at FATAL level.
             * Use the macro FATAL(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogFatal(const std::string& message, const Location& loc = Location())
            {
                Log(eFatal, message, loc);
            }

        private:
            struct Private;
            Private* fPrivate;
    };

}

// PRIVATE MACROS

#define __DEFAULT_LOGGER        scarab::logger::GetRootLogger()

#define __LOG_LOCATION         scarab::logger::Location(__FILE__, __FUNC__, __LINE__)
//#ifndef _WIN32
#define __LOG_LOG_4(I,L,M,O) \
        { \
    if (I.IsLevelEnabled(scarab::logger::e##L)) { \
        static bool _sLoggerMarker = false; \
        if (!O || !_sLoggerMarker) { \
            _sLoggerMarker = true; \
            std::ostringstream stream; stream << M; \
            I.Log(scarab::logger::e##L, stream.str(), __LOG_LOCATION); \
        } \
    } \
        }
//#else
//#define __LOG_LOG_4(I,L,M,O) \
//	    { }
//#endif


#define __LOG_LOG_3(I,L,M)     __LOG_LOG_4(I,L,M,false)
#define __LOG_LOG_2(L,M)       __LOG_LOG_4(__DEFAULT_LOGGER,L,M,false)
#define __LOG_LOG_1(M)         __LOG_LOG_4(__DEFAULT_LOGGER,Debug,M,false)

#define __LOG_TRACE_2(I,M)     __LOG_LOG_4(I,Trace,M,false)
#define __LOG_TRACE_1(M)       __LOG_LOG_4(__DEFAULT_LOGGER,Trace,M,false)

#define __LOG_DEBUG_2(I,M)     __LOG_LOG_4(I,Debug,M,false)
#define __LOG_DEBUG_1(M)       __LOG_LOG_4(__DEFAULT_LOGGER,Debug,M,false)

#define __LOG_INFO_2(I,M)      __LOG_LOG_4(I,Info,M,false)
#define __LOG_INFO_1(M)        __LOG_LOG_4(__DEFAULT_LOGGER,Info,M,false)

#define __LOG_PROG_2(I,M)      __LOG_LOG_4(I,Prog,M,false)
#define __LOG_PROG_1(M)        __LOG_LOG_4(__DEFAULT_LOGGER,Prog,M,false)

#define __LOG_WARN_2(I,M)      __LOG_LOG_4(I,Warn,M,false)
#define __LOG_WARN_1(M)        __LOG_LOG_4(__DEFAULT_LOGGER,Warn,M,false)

#define __LOG_ERROR_2(I,M)     __LOG_LOG_4(I,Error,M,false)
#define __LOG_ERROR_1(M)       __LOG_LOG_4(__DEFAULT_LOGGER,Error,M,false)

#define __LOG_FATAL_2(I,M)     __LOG_LOG_4(I,Fatal,M,false)
#define __LOG_FATAL_1(M)       __LOG_LOG_4(__DEFAULT_LOGGER,Fatal,M,false)

#define __LOG_ASSERT_3(I,C,M)  if (!(C)) { __LOG_ERROR_2(I,M) }
#define __LOG_ASSERT_2(C,M)    __LOG_ASSERT_3(__DEFAULT_LOGGER,C,M)


#define __LOG_LOG_ONCE_3(I,L,M)     __LOG_LOG_4(I,L,M,true)
#define __LOG_LOG_ONCE_2(L,M)       __LOG_LOG_4(__DEFAULT_LOGGER,L,M,true)
#define __LOG_LOG_ONCE_1(M)         __LOG_LOG_4(__DEFAULT_LOGGER,Debug,M,true)

#define __LOG_TRACE_ONCE_2(I,M)     __LOG_LOG_4(I,Trace,M,true)
#define __LOG_TRACE_ONCE_1(M)       __LOG_LOG_4(__DEFAULT_LOGGER,Trace,M,true)

#define __LOG_DEBUG_ONCE_2(I,M)     __LOG_LOG_4(I,Debug,M,true)
#define __LOG_DEBUG_ONCE_1(M)       __LOG_LOG_4(__DEFAULT_LOGGER,Debug,M,true)

#define __LOG_INFO_ONCE_2(I,M)      __LOG_LOG_4(I,Info,M,true)
#define __LOG_INFO_ONCE_1(M)        __LOG_LOG_4(__DEFAULT_LOGGER,Info,M,true)

#define __LOG_WARN_ONCE_2(I,M)      __LOG_LOG_4(I,Warn,M,true)
#define __LOG_WARN_ONCE_1(M)        __LOG_LOG_4(__DEFAULT_LOGGER,Warn,M,true)

#define __LOG_ERROR_ONCE_2(I,M)     __LOG_LOG_4(I,Error,M,true)
#define __LOG_ERROR_ONCE_1(M)       __LOG_LOG_4(__DEFAULT_LOGGER,Error,M,true)

#define __LOG_FATAL_ONCE_2(I,M)     __LOG_LOG_4(I,Fatal,M,true)
#define __LOG_FATAL_ONCE_1(M)       __LOG_LOG_4(__DEFAULT_LOGGER,Fatal,M,true)


// PUBLIC MACROS

#define LOGGER(I,K)      static scarab::logger I(K);

#ifndef _WIN32

#define LOG(...)          macro_dispatcher(__LOG_LOG_, __VA_ARGS__)(__VA_ARGS__)
#ifdef NDEBUG
#define LTRACE(...)
#define LDEBUG(...)
#else
#define LTRACE(...)       macro_dispatcher(__LOG_TRACE_, __VA_ARGS__)(__VA_ARGS__)
#define LDEBUG(...)       macro_dispatcher(__LOG_DEBUG_, __VA_ARGS__)(__VA_ARGS__)
#endif
#define LINFO(...)        macro_dispatcher(__LOG_INFO_, __VA_ARGS__)(__VA_ARGS__)
#define LPROG(...)        macro_dispatcher(__LOG_PROG_, __VA_ARGS__)(__VA_ARGS__)
#define LWARN(...)        macro_dispatcher(__LOG_WARN_, __VA_ARGS__)(__VA_ARGS__)
#define LERROR(...)       macro_dispatcher(__LOG_ERROR_, __VA_ARGS__)(__VA_ARGS__)
#define LFATAL(...)       macro_dispatcher(__LOG_FATAL_, __VA_ARGS__)(__VA_ARGS__)
#define LASSERT(...)      macro_dispatcher(__LOG_ASSERT_, __VA_ARGS__)(__VA_ARGS__)

#define LOG_ONCE(...)     macro_dispatcher(__LOG_LOG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#ifdef NDEBUG
#define LTRACE_ONCE(...)
#define LDEBUG_ONCE(...)
#else
#define LTRACE_ONCE(...)  macro_dispatcher(__LOG_TRACE_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define LDEBUG_ONCE(...)  macro_dispatcher(__LOG_DEBUG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#endif
#define LINFO_ONCE(...)   macro_dispatcher(__LOG_INFO_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define LPROG_ONCE(...)   macro_dispatcher(__LOG_PROG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define LWARN_ONCE(...)   macro_dispatcher(__LOG_WARN_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define LERROR_ONCE(...)  macro_dispatcher(__LOG_ERROR_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define LFATAL_ONCE(...)  macro_dispatcher(__LOG_FATAL_ONCE_, __VA_ARGS__)(__VA_ARGS__)

#else /*_WIN32*/

#define LOG(I, ...)          __LOG_LOG_2(I, __VA_ARGS__)
#ifdef NDEBUG
#define LTRACE(I, ...)
#define LDEBUG(I, ...)
#else
#define LTRACE(I, ...)       __LOG_TRACE_2(I, __VA_ARGS__)
#define LDEBUG(I, ...)       __LOG_DEBUG_2(I, __VA_ARGS__)
#endif
#define LINFO(I, ...)        __LOG_INFO_2(I, __VA_ARGS__)
#define LPROG(I, ...)        __LOG_PROG_2(I, __VA_ARGS__)
#define LWARN(I, ...)        __LOG_WARN_2(I, __VA_ARGS__)
#define LERROR(I, ...)       __LOG_ERROR_2(I, __VA_ARGS__)
#define LFATAL(I, ...)       __LOG_FATAL_2(I, __VA_ARGS__)
#define LASSERT(I, ...)      __LOG_ASSERT_2(I, __VA_ARGS__)

#define LOG_ONCE(I, ...)     __LOG_LOG_ONCE_2(I, __VA_ARGS__)
#ifdef NDEBUG
#define LTRACE_ONCE(I, ...)
#define LDEBUG_ONCE(I, ...)
#else
#define LTRACE_ONCE(I, ...)  __LOG_TRACE_ONCE_2(I, __VA_ARGS__)
#define LDEBUG_ONCE(I, ...)  __LOG_DEBUG_ONCE_2(I, __VA_ARGS__)
#endif
#define LINFO_ONCE(I, ...)   __LOG_INFO_ONCE_2(I, __VA_ARGS__)
//#define LPROG_ONCE(I, ...)   __LOG_PROG_ONCE_2(I, __VA_ARGS__)
#define LWARN_ONCE(I, ...)   __LOG_WARN_ONCE_2(I, __VA_ARGS__)
#define LERROR_ONCE(I, ...)  __LOG_ERROR_ONCE_2(I, __VA_ARGS__)
#define LFATAL_ONCE(I, ...)  __LOG_FATAL_ONCE_2(I, __VA_ARGS__)

#endif /*_WIN32*/

#if 0

#include <iostream>
#define LOGGER(I,K)

#define LOG(I,K)         std::cout << "LOG: " << K << std::endl;
#ifndef NDEBUG
#define LTRACE(I,K)       std::cout << "TRACE: " << K << std::endl;
#define LDEBUG(I,K)       std::cout << "DEBUG: " << K << std::endl;
#else
#define LTRACE(I,K)
#define LDEBUG(I,K)
#endif
#define LINFO(I,K)        std::cout << "INFO: " << K << std::endl;
#define LPROG(I,K)        std::cout << "PROG: " << K << std::endl;
#define LWARN(I,K)        std::cout << "WARN: " << K << std::endl;
#define LERROR(I,K)       std::cout << "ERROR: " << K << std::endl;
#define LFATAL(I,K)       std::cout << "FATAL: " << K << std::endl;
#define LASSERT(I,K)      std::cout << "ASSERT: " << K << std::endl;

#define LOG_ONCE(I,K)    std::cout << "LOG: " << K << std::endl;
#ifndef NDEBUG
#define LTRACE_ONCE(I,K)  std::cout << "TRACE: " << K << std::endl;
#define LDEBUG_ONCE(I,K)  std::cout << "DEBUG: " << K << std::endl;
#else
#define LTRACE_ONCE(I,K)
#define LDEBUG_ONCE(I,K)
#endif
#define LINFO_ONCE(I,K)   std::cout << "INFO: " << K << std::endl;
#define LPROG_ONCE(I,K)   std::cout << "PROG: " << K << std::endl;
#define LWARN_ONCE(I,K)   std::cout << "WARN: " << K << std::endl;
#define LERROR_ONCE(I,K)  std::cout << "ERROR: " << K << std::endl;
#define LFATAL_ONCE(I,K)  std::cout << "FATAL: " << K << std::endl;

#endif

#endif /* SCARAB_LOGGER_HH_ */
