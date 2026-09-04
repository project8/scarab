/*
 * test_static_destruction.cc
 *
 *  Created on: Sep 4, 2026
 *      Author: N.S. Oblath
 *
 *  Use:
 *    > bin/test_static_destruction
 *
 *  Regression test for a heap-use-after-free during static destruction.
 *
 *  A registrar un-registers itself in its destructor, calling indexed_factory::remove_class().
 *  For a static registrar that happens during static destruction, so remove_class() must not
 *  log: a logger is itself a static object and may already have been destroyed.
 *
 *  Two registrars are required to reproduce the failure.  Statics are destroyed in reverse
 *  order of construction, so a logger created inside remove_class() during the destruction
 *  phase is destroyed almost immediately afterward -- the first registrar creates it, and
 *  the second then writes into freed memory.  With only one registrar nothing follows it,
 *  and the bug does not appear.
 *
 *  This program exits successfully either way unless it is built with the sanitizers
 *  (Scarab_ENABLE_SANITIZERS=TRUE), which is what turns the invalid write into a failure.
 *  It must also be built without NDEBUG, since LTRACE compiles to nothing when NDEBUG is set.
 */

#include "factory.hh"

#include <string>

namespace scarab
{
    struct test_base
    {
        virtual ~test_base() {}
    };

    struct test_derived_1 : test_base {};
    struct test_derived_2 : test_base {};
}

// Both registrars must be static and use the same factory instantiation so that the
// second one's destructor runs after the first one has already created the logger.
static scarab::registrar< scarab::test_base, scarab::test_derived_1 > s_reg_1( "test_derived_1" );
static scarab::registrar< scarab::test_base, scarab::test_derived_2 > s_reg_2( "test_derived_2" );

int main(int , char ** )
{
    return( EXIT_SUCCESS );
}
