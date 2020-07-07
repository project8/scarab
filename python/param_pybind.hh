/*
 * param_pybind.hh
 *
 *  Created on: Feb 1, 2018
 *      Author: N. Oblath, L. Gladstone, B.H. LaRoque
 */

#ifndef PARAM_PYBIND_HH_
#define PARAM_PYBIND_HH_

#include "param.hh"
#include "param_binding_helpers.hh"
#include "error.hh"

#include "pybind11/pybind11.h"
#include "pybind11/pytypes.h"


namespace scarab_pybind
{
    std::list< std::string > export_param( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        all_members.push_back( "to_param" );
        mod.def( "to_param",
                &to_param,
                pybind11::arg( "object"),
                pybind11::arg( "hyphenate_keys" ) = false,
                "Convert native python types to a param structure." );

        // param
        all_members.push_back( "Param" );
        pybind11::class_< scarab::param >( mod, "Param", "param data structure base class and null object" )
            .def( pybind11::init< >() )
            .def( pybind11::init< scarab::param_value >() )

            .def( "__str__", &scarab::param::to_string )
            .def( "__call__", (scarab::param_value& (scarab::param::*)()) &scarab::param::operator(),
                    pybind11::return_value_policy::reference_internal )

            .def( "is_null", &scarab::param::is_null, "return whether the param object is an empty param type" )
            .def( "is_node", &scarab::param::is_node, "return whether the param object is a ParamNode" )
            .def( "is_array", &scarab::param::is_array, "return whether the param object is a ParamArray" )
            .def( "is_value", &scarab::param::is_value, "return whether the param object is a ParamValue" )

            .def( "as_array",
                    (scarab::param_array& (scarab::param::*)()) &scarab::param::as_array,
                    pybind11::return_value_policy::reference_internal,
                    "returns Param object as a ParamArray" )
            .def( "as_node",
                    (scarab::param_node& (scarab::param::*)()) &scarab::param::as_node,
                    pybind11::return_value_policy::reference_internal,
                    "returns Param object as ParamNode" )
            .def( "as_value",
                    (scarab::param_value& (scarab::param::*)()) &scarab::param::as_value,
                    pybind11::return_value_policy::reference_internal,
                    "returns Param object as ParamValue" )

            .def( "to_python",
                    &to_python,
                    pybind11::arg( "underscore_keys" ) = false,
                    "recursively converts param object to native python data structure" )

            //TODO: has_subset()

            //TODO: get_value() in its various types

            //TODO: merge()

            ;
        return all_members;
    }

} /* namespace scarab_pybind */
#endif /* PARAM_PYBIND_HH_ */
