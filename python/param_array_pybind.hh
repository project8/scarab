/*
 * KTPyNymph.cc
 *
 *  Created on: Feb 1, 2018
 *      Author: N. Oblath, L. Gladstone, B.H. LaRoque
 */

#ifndef PARAM_ARRAY_PYBIND_HH_
#define PARAM_ARRAY_PYBIND_HH_

#include "param.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{

    std::list< std::string > export_param_array( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        // param_array
        all_members.push_back( "ParamArray" );
        pybind11::class_< scarab::param_array, scarab::param >( mod, "ParamArray", "param data structure object for holding an ordered sequence of objects" )
            .def( pybind11::init< >() )
            .def( "__str__", &scarab::param_array::to_string )
            .def( "__len__", &scarab::param_array::size,
                    "Returns the size of the array" )
            .def( "__getitem__", (scarab::param& (scarab::param_array::*)(unsigned)) &scarab::param_array::operator[],
                    pybind11::return_value_policy::reference_internal)
            .def( "__setitem__", [](scarab::param_array& an_obj, unsigned a_index, scarab::param& a_value){ an_obj.assign( a_index, a_value ); } )
            .def( "__iter__", [](const scarab::param_array& an_obj){ return pybind11::make_iterator(an_obj.begin(), an_obj.end()); },
                    pybind11::keep_alive<0, 1>() /* keep object alive while the iterator exists */)

            //TODO: has_subset()

            .def( "empty", &scarab::param_array::empty,
                    "True if the length is zero" )

            .def( "resize", &scarab::param_array::resize, pybind11::arg( "size" ),
                    "Sets the size of the array; if smaller than the current size, the extra elements are deleted" )

            .def( "assign",
                    (void (scarab::param_array::*)(unsigned, const scarab::param&)) &scarab::param_array::assign,
                    pybind11::arg( "index" ),
                    pybind11::arg( "value" ),
                    "Add a param object [value] to the specified [index] in a array" )

            // Get value of the parameter, bringing along the default value
            .def( "get_value",
                    (bool (scarab::param_array::*)(unsigned, bool) const) &scarab::param_array::get_value<bool>,
                    pybind11::arg( "index" ),
                    pybind11::arg( "default" ),
                    "Get boolean parameter array value at index, or return provided value if out of index range" )
            .def( "get_value",
                    (unsigned (scarab::param_array::*)(unsigned, unsigned) const) &scarab::param_array::get_value<uint>,
                    pybind11::arg( "index" ),
                    pybind11::arg( "default" ),
                    "Get integer parameter array value at index, or return provided value if out of index range" )
            .def( "get_value",
                    (int (scarab::param_array::*)(unsigned, int) const) &scarab::param_array::get_value<int>,
                    pybind11::arg( "index" ),
                    pybind11::arg( "default" ),
                    "Get signed integer parameter array value at index, or return provided value if out of index range" )
            .def( "get_value",
                    (double (scarab::param_array::*)(unsigned, double) const) &scarab::param_array::get_value<double>,
                    pybind11::arg( "index" ),
                    pybind11::arg( "default" ),
                    "Get float parameter array value at index, or return provided value if out of index range" )
            .def( "get_value",
                    (std::string (scarab::param_array::*)(unsigned, const std::string& ) const) &scarab::param_array::get_value,
                    pybind11::arg( "index" ),
                    pybind11::arg( "default" ),
                    "Get string parameter array value at index, or return provided value if out of index range" )

            .def( "push_back",
                    (void (scarab::param_array::*)(const scarab::param&)) &scarab::param_array::push_back,
                    pybind11::arg( "value" ),
                    "add a param object to the end of the array")
            .def( "push_front",
                    (void (scarab::param_array::*)(const scarab::param&)) &scarab::param_array::push_front,
                    pybind11::arg( "value" ),
                    "add a param object to the end of the array")

            .def( "append",
                    &scarab::param_array::append,
                    pybind11::arg( "array" ),
                    "adds all elements of a param_array to the end of this" )
            .def( "merge",
                    &scarab::param_array::merge,
                    pybind11::arg( "array" ),
                    "merges provided param_array, effectively replacing the current array")
            .def( "erase",
                    &scarab::param_array::erase,
                    pybind11::arg( "index" ),
                    "eliminates the value at a position in the array" )
            .def( "remove",
                    &scarab::param_array::remove,
                    pybind11::arg( "index" ),
                    "returns the param_value at the provided index and removes it from the array" )
            .def( "clear", &scarab::param_array::clear, "erase all contents and resize to 0" )

            ;
        return all_members;
    }

} /* namespace scarab_pybind */
#endif /* PARAM_ARRAY_PYBIND_HH_ */
