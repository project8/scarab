Logger
======

Scarab includes a logger that should be used for printing to the terminal instead of simply using ``cout`` and ``cerr``, ``printf``, or other methods.  
It provides a uniform look to the terminal output, and adds useful information like the timestamp for each message, the thread ID, and the location 
in the source code.  Each log message is also assigned a "level" to indicate the priority level of the message, and to allow printing of messages 
to be controlled by verbosity settings either directly through the logger or in application settings.

The underlying infrastructure for the logger is provided by the `spdlog logging library <https://github.com/gabime/spdlog>`_.  
Most Scarab users and developers will not need to interact with spdlog, but information is provided :ref:`below <spdlog-logging-library>` 
on how and why it's used to provide insight into the design of the Scarab logger.

Logging Levels
--------------

.. _logging-levels:

+-------------+------------------------------------------------+------------+-----------------+-----------------------------------+
| Level       | Purpose                                        | Logging    | Terminal Color  | :cpp:enum:`scarab::logger::ELevel`| 
|             |                                                | Macro      |                 | enum                              |
+=============+================================================+============+=================+===================================+
| Fatal       | An error has occurred that should cause        | ``LFATAL`` | Red background  | ``eFatal``                        |
|             | an application to exit immediately             |            | Green text      |                                   |
+-------------+------------------------------------------------+------------+-----------------+-----------------------------------+
| Error       | Something has gone wrong that will cause       | ``LERROR`` | Red             | ``eError``                        |
|             | the application to act in an unexpected way    |            |                 |                                   |
+-------------+------------------------------------------------+------------+-----------------+-----------------------------------+
| Warning     | Something unexpected has happened, that the    | ``LWARN``  | Orange          | ``eWarn``                         |
|             | user should be aware of, but execution will    |            |                 |                                   |
|             | continue                                       |            |                 |                                   |
+-------------+------------------------------------------------+------------+-----------------+-----------------------------------+
| Progress    | Reporting important information for the        | ``LPROG``  | Dark Blue       | ``eProg``                         |
|             | user where the printing will happen only       |            |                 |                                   |
|             | once or infrequently                           |            |                 |                                   |
+-------------+------------------------------------------------+------------+-----------------+-----------------------------------+
| Information | Reporting useful information for the user;     | ``LINFO``  | Green           | ``eInfo``                         | 
|             | printing may repeat, but will be minimal       |            |                 |                                   |
+-------------+------------------------------------------------+------------+-----------------+-----------------------------------+
| Debug       | Reporting information useful for debugging;    | ``LDEBUG`` | Light Blue      | ``eDebug``                        |
|             | printing may be verbose and impact performance |            |                 |                                   |
+-------------+------------------------------------------------+------------+-----------------+-----------------------------------+
| Trace       | Reporting information useful for low-level     | ``LTRACE`` | White           | ``eTrace``                        |
|             | debugging; will be very verbose and impact     |            |                 |                                   |
|             | performance significantly                      |            |                 |                                   |
+-------------+------------------------------------------------+------------+-----------------+-----------------------------------+

Terminal Output
---------------

UPDATE

The default formatting for the terminal output at each level looks like this::

    2025-08-25 21:12:21.020 [TRACE] (7) test_logger.cc:64 -> test TRACE
    2025-08-25 21:12:21.020 [DEBUG] (7) test_logger.cc:67 -> test DEBUG
    2025-08-25 21:12:21.020 [INFO] (7) test_logger.cc:70 -> test INFO
    2025-08-25 21:12:21.020 [PROG] (7) test_logger.cc:73 -> test PROG
    2025-08-25 21:12:21.020 [WARNING] (7) test_logger.cc:78 -> test WARN
    2025-08-25 21:12:21.020 [ERROR] (7) test_logger.cc:81 -> test ERROR
    2025-08-25 21:12:21.020 [FATAL] (7) test_logger.cc:84 -> test FATAL

Use in Source Code
------------------

UPDATE: LOGGER_ST for single threaded
Anywhere the logger is being used, with few exceptions, the ``LOGGER()`` macro must first be used to create a static logger object.  
That macro takes two parameter:

1. The variable name to which the logger will be assigned.  This is a static variable, so it will have global scope.  It can be shared 
between all or multiple compilation units in a build, or it can be unique.
2. The string name for the backend logger object.  This may be shared between multiple logger objects or be unique.  Currently the 
logger name is not used in the Scarab logger API, but it may be used in the future.

Here's an example creation of a static logger::

    LOGGER( mylog, "library" );

In this case, ``mylog`` will be the variable name used in this compilation unit.  The string name ``library``, in this example, 
would be common in the library built in a certain package.  In a future release it'll allow properties of the logging to be controlled for 
the entire library.

UPDATE: LOCAL_LOGGER_ST for single threaded
Optionally, the ``LOCAL_LOGGER()`` macro may be used.  This creates a non-static logger object and takes the same arguments as ``LOGGER()``. 
It's primarily used in situations where a static object should not be used or would be complicated to use (e.g. in a header file).

To log a message, use one of the logging macros in the :ref:`table above <logging-levels>`, providing the logger variable name and the message.  The streaming operator 
``<<`` is used to combine parts of a message, and anything that can be passed to a :cpp:class:`std::stringstream` can be used.  For example:: 

    LINFO( mylog, "Hello, my count is " << counter );

.. _asynchronous-logging:

Asynchronous Logging
--------------------

The scarab logger uses a logging library that provides asynchronous logging via a separate logging thread.  Every log message is passed from 
the thread in which the message was created to the logging thread, and that thread prints the message in the terminal.  
See section :ref:`spdlog-logging-library` for more information on why this setup was used.  The logging thread prints messages in the order they are 
received.  If logging via scarab's logger is used at the same time as printing via ``stdout``, or if the C++ code is being used as part of a Python 
application, the asynchronous nature of the C++ logging will result in messages out of order with respect to the ``stdout`` or Python-based messages.

How Do I...
-----------

Control Verbosity
^^^^^^^^^^^^^^^^^

While running an application
""""""""""""""""""""""""""""

If an application is built with :cpp:class:`scarab::application` and the standard CL options, the ``-v`` flag will increase verbosity, and ``-q`` will decrease 
verbosity.  Each flag can be used multiple times to shift the verbosity by multiple levels.

Interfacing with ``logger`` in source code
""""""""""""""""""""""""""""""""""""""""""

You can set the static parameter :cpp:var:`logger::f_global_verbosity` with values from the :cpp:enum:`scarab::logger::ELevel` enum (see above).

.. _hdi-setup-logging:

Setup Logging in an Application
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* If you application is built with :cpp:class:`scarab::application`, verbosity controls are already provided.
* The logger backend will be configured and automatically started by the first use of the ``LOGGER[_ST]()`` or ``LOCAL_LOGGER[_ST]()`` macro.

Log a Message
^^^^^^^^^^^^^

Use one of the logging macros in the :ref:`logging-levels table <logging-levels>`.  For users familiar with using ``std::cout``, for instance, 
you would replace:: 

    std::cout << "Here's the value of my_counter: " << my_counter << std::endl;

with this::

    LINFO( mylog, "Here's the value of my_counter: " << my_counter );

Take Performance into Account
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Every logged message will have a small but finite performance cost.  For the normal-operations logging levels, please consider the following: 

``LTRACE``
    This level is intended to allow developers to follow the execution of code at a low level.  Use this level for output that most developers 
    wouldn't need to see while doing development work.  Developers would turn this on explicitly to trace execution, knowing that it might 
    produce large volumes of terminal output, especially in looping code.  For ``Release`` builds, these messages are excluded at compile 
    time and have zero overhead.

``LDEBUG``
    This level is intended as the "standard" debugging-level output.  You would not want this output when doing production running, but 
    it's useful to understand how the code is operating.  You don't want it to flood the terminal, but it can still be quite verbose 
    during execution.  For ``Release`` builds, these messages are excluded at compile time and have zero overhead.

``LINFO``
    This is the default level of printing.  If seeking high performance you might want to turn these messages off, but they will not be 
    excluded at compile time -- the messages will be evaluated at runtime, but they will not be printed if verbosity is reduced far enough.

``LPROG``
    This level is intended to be the minimum logging to demonstrate that progress is being made by the application.  It should be used 
    sparingly so that when verbosity is decreased to this level, only minimal information is printed.  These messages are not 
    excluded at compile time -- the messages are evaluated at runtime, but they will not be printed if verbosity is reduced far enough. 

Interface with Python
^^^^^^^^^^^^^^^^^^^^^

If developing a Python application that uses wrapped Scarab code, minimal attention is needed.  The C++ verbosity can be set from the Python 
side using the function ``scarab.s2py_verbosity()``.

.. _spd-logging-library:

Logging Infrastructure: spdlog
------------------------------

In 2024 the Scarab logger was revamped to be based on the `spdlog logging library <https://github.com/gabime/spdlog>`_.  
spdlog was chosen because it

* Provides a rich logging environment with options that suited the existing needs of Scarab users;
* Used macros that were almost compatible with the existing Scarab logging macros, so that no changes were necessary for Scarab logger users;
* Funneled all logging through a single thread, so that Pybind11-wrapped code would be happy.

spdlog divides the logging implementation into the frontend "logger" components and the backend "sink" components.  Client code that wants to print a log 
message interacts with the frontend, primarily via the logging macros.  The Scarab logger is a lightweight wrapper around an spdlog logger 
frontend.  

The backend is responsible for printing messages to the terminal (other backend implementations are possible so one can e.g. log to a file).  
The backend configuration includes the color choices and the text that prepends log messages, and this is setup once when the first :cpp:class:`scarab::logger` 
object is created.

spdlog's asynchronous logger uses a separate thread for logging messages, to minimize the performance impact of logging on the client code.  

Stopping Asynchronous Logging
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Any thread that starts has to stop at some point before execution completely stops, and the logging thread created by spdlog is no exception.  
For most users of spdlog, presumably the spdlog's own handling of its logging thread is sufficient.  However, for Scarab, where objects that use 
the logger are created at static initialization time, and therefore destructed after the main execution completes, it was found that 
we could not reliably extend the operation of the spdlog logging thread to cover the lifetime of all logger objects.  This would result in inconsistent 
segfaults as an application exited.  Therefore the choice was made to stop usig asynchronous loggers at the end of the :cpp:func:`main()` function 
(see :ref:`hdi-setup-logging`) and switch to using a synchronous logger to log messages to the terminal after that (typically only messages regarding the 
destruction of statically-created objects).  This is taken care of automatically by a function called as the program is exiting.
