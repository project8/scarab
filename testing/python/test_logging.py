# Test script for the logging binding
# Author: N.S. Oblath
# Date created: Jan 7, 2025

import scarab
import logging

def test_verbosity():

    assert scarab.s2py_verbosity(2) == logging.DEBUG
    assert scarab.s2py_verbosity(3) == logging.DEBUG
    assert scarab.s2py_verbosity(4) == logging.INFO
    assert scarab.s2py_verbosity(5) == logging.INFO
    assert scarab.s2py_verbosity(6) == logging.WARNING
    assert scarab.s2py_verbosity(7) == logging.ERROR
    assert scarab.s2py_verbosity(8) == logging.CRITICAL
