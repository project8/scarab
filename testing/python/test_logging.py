# Test script for the logging binding
# Author: N.S. Oblath
# Date created: Jan 7, 2025

import scarab
import logging

def test_verbosity():

    assert scarab.s2py_verbosity(0) == logging.DEBUG
    assert scarab.s2py_verbosity(1) == logging.DEBUG
    assert scarab.s2py_verbosity(2) == logging.INFO
    assert scarab.s2py_verbosity(3) == logging.INFO
    assert scarab.s2py_verbosity(4) == logging.WARNING
    assert scarab.s2py_verbosity(5) == logging.ERROR
    assert scarab.s2py_verbosity(6) == logging.CRITICAL
