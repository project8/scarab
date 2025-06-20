#include "test_class.hh"
#include "yala.hh"

#include "quill/LogMacros.h"

//#include <iostream>
#include <string>
#include <utility>

static yala::test_class s_tc;

int main()
{
    s_tc.print_something();
  
    return 0;
}
