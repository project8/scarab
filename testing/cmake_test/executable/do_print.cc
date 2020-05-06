// print.cc
// Author: N.S. Oblath
// Date: May 4, 2020

#include "print.hh"

int main()
{
    pbtest::print( scarab::param_value("Hello, World") );

    return 0;
}