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
  * Requires pybind11 v2.3.0 or greater
