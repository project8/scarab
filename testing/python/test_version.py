# Test script for the version binding
# Author: N.S. Oblath
# Date created: Aug 7, 2024

import scarab

def test_comparisons():

    test_100 = scarab.VersionSemantic(1, 0, 0)
    test_001 = scarab.VersionSemantic(0, 0, 0)
    test_100_v2 = scarab.VersionSemantic(1, 0, 0)

    assert test_100 > test_001
    assert test_001 < test_100
    assert test_100 == test_100_v2

