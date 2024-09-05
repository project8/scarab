# Test script for the authentication binding
# Author: N.S. Oblath
# Date created: Aug 12, 2024

import scarab

def test_auth_file():

    auth = scarab.Authentication('test_auth_file.py')

    print(auth.spec)
    assert 'file' in auth.spec
    assert auth.spec['file']().as_string() == 'test_auth_file.py'
