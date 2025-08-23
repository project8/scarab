Scarab includes a variety of utilities primarily aimed at the C++ packages developed by the Project 8 collaboration.

It contains three main components:
* cmake: common CMake modules and scripts
  * PackageBuilder framework
  * Find-module scripts for Eigen3, FFTW, ROOT, and Sphinx
* Documentation
  * C++ system
  * Python system
* library: C++ library of common utilities
  * Param system
  * Logger
  * Authentication class
  * A variety of utilities
* python: Python bindings for a subset of the C++ library

Requirements:
* CMake v3.12 or higher
* Boost 1.46 or higher; filesystem and system components
* For optional YAML/param codec
  * yaml-cpp v1.0 or higher
* For optional JSON/param codec
  * RapidJSON
* For optional Python bindings:
  * Python 3
  * Pybind11 v3.0.0 or higher
