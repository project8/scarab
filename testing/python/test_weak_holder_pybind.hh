/*
 * test_weak_holder_pybind.hh
 *
 *  Created on: Apr 23, 2025
 *      Author: N. Oblath
 */

#ifndef TEST_WEAK_HOLDER_PYBIND_HH_
#define TEST_WEAK_HOLDER_PYBIND_HH_
 
#include "weak_holder.hh"
  
#include "pybind11/pybind11.h"
#include "pybind11/pytypes.h"
 
#include <list>
 
namespace scarab_testing_pybind
{
    struct wh_tester
    {
        virtual int value() {return 100;}
        virtual ~wh_tester() = default;
    };

    struct wp_tester_parent
    {
        std::weak_ptr< wh_tester > f_ptr;
        void load( const std::shared_ptr< wh_tester > a_ptr )
        {
            f_ptr = a_ptr;
            return;
        }
        int value() const
        {
            auto wht_ptr = f_ptr.lock();
            return wht_ptr ? wht_ptr->value() : -999;
        }
    };

    struct wh_tester_parent
    {
        scarab::weak_holder< wh_tester > f_holder;
        void load( const std::shared_ptr< wh_tester > a_ptr )
        {
            f_holder.load( a_ptr );
            return;
        }
         int value() const
        {
            auto wht_ptr = f_holder.lock();
            return wht_ptr ? wht_ptr->value() : -999;
        }
    };

 
    std::list< std::string > export_test_weak_holder( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        // wh_tester
        all_members.push_back( "wh_tester" );
        pybind11::classh< wh_tester >( mod, "WHTester", "WHTester base class" )
            .def( pybind11::init< >() )
            .def( "value", &wh_tester::value, "Return a value" )
            ;
 
        // wp_tester_parent
        all_members.push_back( "wp_tester_parent" );
        pybind11::classh< wp_tester_parent >( mod, "WPTesterParent", "WPTesterParent base class" )
            .def( pybind11::init< >() )
            .def( "load", &wp_tester_parent::load, "Load a new pointer")
            .def( "value", &wp_tester_parent::value, "Return a value" )
            ;
 
        // wh_tester_parent
        all_members.push_back( "wh_tester_parent" );
        pybind11::classh< wh_tester_parent >( mod, "WHTesterParent", "WHTesterParent base class" )
            .def( pybind11::init< >() )
            .def( "load", &wh_tester_parent::load, "Load a new pointer")
            .def( "value", &wh_tester_parent::value, "Return a value" )
            ;
 
//        // weak_holder< wh_tester >
//        all_members.push_back( "weak_holder_test" );
//        pybind11::classh< scarab::weak_holder<wh_tester> >( mod, "WHTesterWeakHolder", "Weak-reference holder for a wh_tester object" )
//            .def( pybind11::init< >() )
//            .def( pybind11::init<std::shared_ptr<wh_tester> >() )
// 
//            .def( "load", &scarab::weak_holder<wh_tester>::load_from_py, "Store a weak reference to a wh_tester object" )
//            .def( "lock", &scarab::weak_holder<wh_tester>::lock, "Return a reference to a wh_tester object if the object still exists")
//            ;
        return all_members;
    }
 
} /* namespace scarab_testing_pybind */
#endif /* TEST_WEAK_HOLDER_PYBIND_HH_ */
 