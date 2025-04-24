/*
 * weak_holder_pybind.hh
 *
 *  Created on: Feb 1, 2018
 *      Author: N. Oblath, L. Gladstone, B.H. LaRoque
 */

#ifndef WEAK_HOLDER_PYBIND_HH_
#define WEAK_HOLDER_PYBIND_HH_

#include "weak_holder.hh"
#include "cancelable.hh"

#include "pybind11/pybind11.h"
#include "pybind11/pytypes.h"


namespace scarab_pybind
{
    /*
    struct weak_holder_cancelable_trampoline : scarab::weak_holder<cancelable>, pybind11::trampoline_self_life_support
    {
        using scarab::weak_holder<cancelable>;

        void load( const std::shared_ptr< cancelable > a_ptr ) override 
        {
            PYBIND11_OVERRIDE( void, scarab::weak_holder<cancelable>, load, const std::shared_ptr<cancelable> );
        }

        std::shared_ptr< cancelable > lock() const override 
        {
            PYBIND11_OVERRIDE( std::shared_ptr<cancelable>, scarab::weak_holder<cancelable>, lock, );
        }
    };
    */    

    std::list< std::string > export_weak_holder( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        // weak_holder< cancelable >
        all_members.push_back( "weak_holder_cancelable" );
        pybind11::classh< scarab::weak_holder<scarab::cancelable> >( mod, "WeakHolderCancelable", "Weak-reference holder for a cancelable object" )
            .def( pybind11::init< >() )
            .def( pybind11::init<std::shared_ptr<scarab::cancelable> >() )

            .def( "load", &scarab::weak_holder<scarab::cancelable>::load_from_py, "Store a weak reference to a scarab::cancelable object" )
            .def( "lock", &scarab::weak_holder<scarab::cancelable>::lock, "Return a reference to a scarab::cancelable object if the object still exists")

            ;
        return all_members;
    }

} /* namespace scarab_pybind */
#endif /* WEAK_HOLDER_PYBIND_HH_ */
