// nested_do_print.cc
// Author: N.S. Oblath
// Date: Jun 1, 2020

#include "nested_print.hh"
#include "print.hh"

int main()
{
    nested_pbtest::print( scarab::param_value("Hello, Nested World") );
    pbtest::print( scarab::param_value("Hello, World") );

    return 0;
}