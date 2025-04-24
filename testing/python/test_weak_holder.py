# Test script for the weak_holder binding
# Author: N.S. Oblath
# Date created: Apr 23, 2025

import scarab, scarab_testing

def test_base_tester():
    whtester = scarab_testing.WHTester()

    # First test with a weak_ptr
    wpt_parent = scarab_testing.WPTesterParent()
    assert wpt_parent.value() == -999

    # Put a WHTester in
    wpt_parent.load(whtester)
    assert wpt_parent.value() == 100

    # Second test with a weak_holder
    wht_parent = scarab_testing.WHTesterParent()
    assert wht_parent.value() == -999

    # Put a WHTester in
    wht_parent.load(whtester)
    assert wht_parent.value() == 100

