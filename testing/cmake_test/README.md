# PackageBuilder Test

This directory contains the infrastructure yto test the PackageBuilder framework, including the use of submodules.

## Submodules

Scarab's PackageBuilder provides the framework for using multiply-nested submodules.  To test these features, we need to be able to make Scarab a submodule of the test project.  Therefore we've included the script `copy_scarab.sh`, which copies all of the contents of the scarab directory to a subdirectory called `scarab` (excluding anything listed in the `.gitignore` file).  Then you can use the `CMakeLists.txt` file in this directory to test the submodule framework in PackageBuilder.

Example usage:
```
> cd /top/level/of/Scarab/testing/cmake_test
> ./copy_scarab.sh
> mkdir build
> cd build
> cmake ..
```
