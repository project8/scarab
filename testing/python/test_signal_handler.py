import scarab

class Tester(scarab.Cancelable):
    __test__ = False
    def __init__(self):
        scarab.Cancelable.__init__(self)

def test_cancelation():
    test = Tester()
    assert not test.is_canceled()

    sig_handler = scarab.SignalHandler(True)
    sig_handler.add_cancelable(test)

    sig_handler.cancel_all(0)
    assert test.is_canceled()

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
