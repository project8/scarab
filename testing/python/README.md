# Scarab's Python Testing

## Requirements

Testing of Scarab's python binding is implemented with pytest.  You'll need to install pytest before running these tests.

## System path

If you've installed the Python library in a system location, then you don't need to do anything further to let Python find the Scarab package.  If you've installed in the build directory or some other location, then you'll need to use the `add_lib_python_path.sh` script to set the correct system path:

    > source bin/add_lib_python_path.sh

## Usage

Run all tests with:

    > pytest testing/python
