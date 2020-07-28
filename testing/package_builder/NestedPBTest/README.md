# PackageBuilder Test

This directory contains the infrastructure to test the nested-submodules aspects of the PackageBuilder framework.

Scarab's PackageBuilder provides the framework for using multiply-nested submodules.  To test these features, we need to be able to make Scarab a submodule of the test project.  Therefore we've included the script `create_nesting.sh`, which is used by CMake to copy all of the contents of the scarab directory to a subdirectory called `scarab` (excluding anything listed in the `.gitignore` file), and the `testing/PBTest` directory to a subdirectory called `PBTest`.  `PBTest` also includes its own copy of `scarab`.  Then you can use the `CMakeLists.txt` file in this directory to test the submodule framework in PackageBuilder.

## Usage

```
> cd /top/level/of/Scarab/testing/package_builder/NestedPBTest
> mkdir build
> cd build
> cmake ..
> make install
```

For development, if working on Scarab and building in a subdirectory of `testing/package_builder/NestedPBTest`, be sure to clean scarab and PBTEst before re-testing the CMake process:

```
rm -rf ../scarab ../PBTest
cmake ..
```

## Cleanup

```
> cd /top/level/of/Scarab/testing/package_builder/NestedPBTest
> rm -rf scarab PBTest
```
