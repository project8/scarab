# Test script for the weak_holder binding
# Author: N.S. Oblath
# Date created: Apr 23, 2025

import scarab, scarab_testing

class WHTesterDerived(scarab_testing.WHTester):
    def value():
        return 200

def test_base_tester():
    whtester = scarab_testing.WHTester()

    # First test with a weak_ptr
    print('Empty weak_ptr')
    wpt_parent = scarab_testing.WPTesterParent()
    assert wpt_parent.expired()
    assert wpt_parent.value() == -999

    # Put a WHTester in
    print('Base class stored with weak_ptr')
    wpt_parent.load(whtester)
    assert not wpt_parent.expired()
    assert wpt_parent.value() == 100

    # Second test with a weak_holder
    print('Empty weak_holder')
    wht_parent = scarab_testing.WHTesterParent()
    assert wht_parent.expired()
    assert wht_parent.value() == -999

    # Put a WHTester in
    print('Base class stored with weak_holder')
    wht_parent.load(whtester)
    assert not wht_parent.expired()
    assert wht_parent.value() == 100
#
#def test_derived_tester():
#    whtester = WHTesterDerived()
#
#    # First test with a weak_ptr
#    wpt_parent = scarab_testing.WPTesterParent()
#    assert wpt_parent.expired()
#    assert wpt_parent.value() == -999
#
#    # Put a WHTester in
#    # This is where weak_ptr fails from pybind11 because it doesn't maintain the reference to the derived (Python) class
#    wpt_parent.load(whtester)
#    assert wpt_parent.expired() # would be not expired if weak_ptr worked
#    assert wpt_parent.value() == -999 # Would be 200
#
#    # Second test with a weak_holder
#    wht_parent = scarab_testing.WHTesterParent()
#    assert wht_parent.expired()
#    assert wht_parent.value() == -999
#
#    # Put a WHTester in
#    wht_parent.load(whtester)
#    assert not wht_parent.expired()
#    assert wht_parent.value() == 200
#
