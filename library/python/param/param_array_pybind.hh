/*
 * KTPyNymph.cc
 *
 *  Created on: Feb 1, 2018
 *      Author: N. Oblath, L. Gladstone, B.H. LaRoque
 */

#include "param.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{

    void export_param_array( pybind11::module& mod )
    {
        // param_array
        pybind11::class_< scarab::param_array, scarab::param >( mod, "ParamArray" )
            .def( pybind11::init<>() )

            .def( "size", (unsigned (scarab::param_array::*)() const) &scarab::param_array::size,
                    "Returns the size of the array" )

            .def( "resize", (void (scarab::param_array::*)(unsigned)) &scarab::param_array::resize,
                    "Sets the size of the array; if smaller than the current size, the extra elements are deleted")

            .def( "assign", (void (scarab::param_array::*)(unsigned, const scarab::param&)) &scarab::param_array::assign,
                    "Add a param object to the specified index in a array" )

            .def( "at", (scarab::param& (scarab::param_array::*)(unsigned)) &scarab::param_array::operator[],
                    "Get the param object for a given index" )

            // Get value of the parameter, bringing along the default value
            .def( "get_value", (bool (scarab::param_array::*)(unsigned, bool) const) &scarab::param_array::get_value<bool>,
                    "Get parameter array value as a bool" )
            .def( "get_value", (unsigned (scarab::param_array::*)(unsigned, unsigned) const) &scarab::param_array::get_value<uint>,
                    "Get parameter array value as an unsigned integer" )
            .def( "get_value", (int (scarab::param_array::*)(unsigned, int) const) &scarab::param_array::get_value<int>,
                    "Get parameter array value as a signed integer" )
            .def( "get_value", (double (scarab::param_array::*)(unsigned, double) const) &scarab::param_array::get_value<double>,
                    "Get parameter array value as a float" )
            .def( "get_value", (std::string (scarab::param_array::*)(unsigned, const std::string& ) const) &scarab::param_array::get_value,
                    "Get parameter array value as a string" )
            ;
    }

} /* namespace Nymph */
