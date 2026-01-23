#ifndef TESTING_PYBIND_HH_
#define TESTING_PYBIND_HH_

#include "cancelable.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    class derived_cancelable : public scarab::cancelable
    {
        public:
            derived_cancelable() :
                scarab::cancelable()
            {}
            virtual ~derived_cancelable() {}
    };

    class second_derived_cancelable : public derived_cancelable
    {
        public:
            second_derived_cancelable() :
                derived_cancelable()
            {}
            virtual ~second_derived_cancelable() {}
    };

    class unbound_cancelable : public scarab::cancelable
    {
        public:
            unbound_cancelable() :
                scarab::cancelable()
            {}
            virtual ~unbound_cancelable() {}
    };

    class derived_unbound : public unbound_cancelable
    {
        public:
            derived_unbound() :
                unbound_cancelable()
            {}
            virtual ~derived_unbound() {}
    };

    class derived_shared : public scarab::cancelable
    {
        public:
            derived_shared() :
                scarab::cancelable()
            {}
            virtual ~derived_shared() {}
    };

    std::list< std::string > export_testing( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        all_items.push_back( "DerivedCancelable" );
        pybind11::classh< derived_cancelable, scarab::cancelable >( mod, "DerivedCancelable", "Class that derives from scarab::cancelable in the C++" )
            .def( pybind11::init<>() )

        ;

        all_items.push_back( "SecondDerivedCancelable" );
        pybind11::classh< second_derived_cancelable, derived_cancelable, scarab::cancelable >( mod, "SecondDerivedCancelable", "Class that derives from scarab::cancelable in the C++" )
            .def( pybind11::init<>() )

        ;

        all_items.push_back( "DerivedUnbound" );
        pybind11::classh< derived_unbound, scarab::cancelable >( mod, "DerivedUnbound", "Class that derives from scarab::cancelable in the C++" )
            .def( pybind11::init<>() )

        ;

        all_items.push_back( "DerivedShared" );
        pybind11::classh< derived_shared, scarab::cancelable >( mod, "DerivedShared", "Class that derives from scarab::cancelable in the C++" )
            .def( pybind11::init<>() )

        ;

        return all_items;
    }

} /* namespace scarab_pybind */

#endif /* TESTING_PYBIND_HH_ */
