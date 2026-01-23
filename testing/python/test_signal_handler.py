import scarab

class Tester(scarab.Cancelable):
    __test__ = False
    def __init__(self):
        scarab.Cancelable.__init__(self)

class TesterDerived(Tester):
    __test__ = False
    def __init__(self):
        Tester.__init__(self)

def test_cancelation():
    print("Test iteration C")

    cancel = scarab.Cancelable()
    assert not cancel.is_canceled()

    cancel_derived = scarab.DerivedCancelable()
    assert not cancel_derived.is_canceled()

    second_cancel_derived = scarab.SecondDerivedCancelable()
    assert not second_cancel_derived.is_canceled()

    derived_unbound = scarab.DerivedUnbound()
    assert not derived_unbound.is_canceled()

    shared_derived = scarab.DerivedShared()
    assert not shared_derived.is_canceled()

    test = Tester()
    assert not test.is_canceled()

    test_derived = TesterDerived()
    assert not test_derived.is_canceled()

    sig_handler = scarab.SignalHandler(True)
    sig_handler.add_cancelable(cancel)
    sig_handler.add_cancelable(cancel_derived)
    sig_handler.add_cancelable(second_cancel_derived)
    sig_handler.add_cancelable(derived_unbound)
    sig_handler.add_cancelable(shared_derived)
    sig_handler.add_cancelable(test)
    sig_handler.add_cancelable(test_derived)

    sig_handler.cancel_all(0)

    assert cancel.is_canceled()
    assert cancel_derived.is_canceled()
    assert second_cancel_derived.is_canceled()
    assert derived_unbound.is_canceled()
    assert shared_derived.is_canceled()
    assert test.is_canceled()
    assert test_derived.is_canceled()

    # automated join of waiting thread upon SignalHandler destruction

def test_remove_cancelable():
    test = Tester()
    assert not test.is_canceled()

    sig_handler = scarab.SignalHandler(True)
    sig_handler.add_cancelable(test)
    sig_handler.remove_cancelable(test)

    sig_handler.cancel_all(0)
    assert not test.is_canceled()

    # manually call join
    sig_handler.join_waiting_thread()
