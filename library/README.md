# Description

Common C++ utilities.

# Requirements

* C++03 (C++11 optional)
* Boost 1.46, including filesystem, system, thread, and date_time libraries (if using Authentication)

# OS Compatibility

* Linux
* Mac
* Windows

# Components

* Authentication
* Logger
* Param (hierarchical data structure)
* Utility
  * Concurrent queue
  * Digitization functions
  * Error (exception class)
  * Member variable macros
  * Mutex (wraps either pthread or C++11 std::mutex)
  * API macros for Windows
  * Singleton base class
  * Version (base class scarab::version_semver and implementation scarab::version)
  * Time functions