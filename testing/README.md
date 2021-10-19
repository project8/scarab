# README for scarab/testing

## C++

### Catch2

Scarab uses the Catch2 testing framework for unit tests.  See these links for more information:

* [Main GitHub page](https://github.com/catchorg/Catch2)
* [Tutorial](https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md)

### Building tests

In CMake, enable the option `Scarab_ENABLE_TESTING`, and build.

The testing executable, `run_tests`, will be installed in `testing/`.

### Running tests

To see the available options for running tests, you can do:

```
> testing/run_tests -h
```

To simply run all of the tests, you can do:

```
> testing/run_tests
```

Or if you want to run a specific test, you can do:

```
> testing/run_tests [name]
```

For further documentation on using Catch2, see the [tutorial](https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md)

## Applications

Four example applications are included in the `applications` directory, demonstrating different ways in which the CLI portion of the library can be used:

* test_app_with_callback
* test_app_with_options
* test_app_with_subcommands
* test_basic_application

To build these examples, ensure that the CMake flags `Scarab_ENABLE_TESTING` AND `Scarab_BUILD_CLI` are set to `TRUE`.

## Package Builder

Tests of the package builder framework are included in the `package_builder` directory, in these subdirectories:

* NestedPBTest
* PBTest

## Doxygen Documentation

A Doxygen configuration file has been included in the `testing` directory to allow testing of Doxygen documentation generation.

To use:

1. Enter into the `testing` source directory
1. Run `doxygen Doxyfile`
1. Open `testing/user_doxygen_out/html/index.html` in a web browser
1. When finished, remember to delete `testing/user_doxygen_out` so that it doesn't get committed to the repo
