import scarab

class Tester(scarab.Cancelable):
    __test__ = False
    def __init__(self):
        scarab.Cancelable.__init__(self)

def test_cancel():
    test = Tester()
    assert not test.is_canceled()
    test.cancel(0)
    assert test.is_canceled()
    test.reset_cancel()
    assert not test.is_canceled()
