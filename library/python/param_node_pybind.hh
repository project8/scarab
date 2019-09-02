/*
 *  Created on: Feb 1, 2018
 *      Author: N. Oblath, L. Gladstone, B.H. LaRoque
 */

#include "param.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{

    void export_param_node( pybind11::module& mod )
    {
        // param_node
        pybind11::class_< scarab::param_node, scarab::param >( mod, "ParamNode" )
            .def( pybind11::init< >() )

            .def( "__str__", &scarab::param_node::to_string )
            .def( "__len__", &scarab::param_node::size )
            .def( "__getitem__", (scarab::param& (scarab::param_node::*)(const std::string&)) &scarab::param_node::operator[],
                    pybind11::return_value_policy::reference_internal)
            .def( "__setitem__", [](scarab::param_node& an_obj, std::string& a_name, scarab::param& a_value){ an_obj.replace( a_name, a_value ); } )
            .def( "__contains__", &scarab::param_node::has )
            .def( "__iter__", [](const scarab::param_node& an_obj){ return pybind11::make_iterator(an_obj.begin(), an_obj.end()); },
                    pybind11::keep_alive<0, 1>() /* keep object alive while the iterator exists */ )

            .def( "count", &scarab::param_node::count )

            .def( "is_null", &scarab::param_node::is_null )
            .def( "is_node", &scarab::param_node::is_node )

            //TODO: has_subset()

            .def( "empty", &scarab::param_node::empty )

            .def( "add", (bool (scarab::param_node::*)(const std::string&, const scarab::param&)) &scarab::param_node::add,
                    "Add a param object to a node")

            //TODO: remove this; it's not part of the actual param API
            .def( "at", (scarab::param& (scarab::param_node::*)(const std::string&)) &scarab::param_node::operator[],
                    "Get the param object for a given key" )

            // Get value of the parameter, bringing along the default value
            .def( "get_value", (bool (scarab::param_node::*)(const std::string&, bool) const) &scarab::param_node::get_value<bool>,
                    "Get parameter node value as a bool" )
            .def( "get_value", (unsigned (scarab::param_node::*)(const std::string&, unsigned) const) &scarab::param_node::get_value<uint>,
                    "Get parameter node value as an unsigned integer" )
            .def( "get_value", (int (scarab::param_node::*)(const std::string&, int) const) &scarab::param_node::get_value<int>,
                    "Get parameter node value as a signed integer" )
            .def( "get_value", (double (scarab::param_node::*)(const std::string&, double) const) &scarab::param_node::get_value<double>,
                    "Get parameter node value as a float" )
            .def( "get_value", (std::string (scarab::param_node::*)(const std::string&, const std::string& ) const) &scarab::param_node::get_value,
                    "Get parameter node value as a string" )

            //TODO: replace()

            //TODO: merge(), erase(), remove(), clear()

            //TODO: iterators


            ;
    }
} /* namespace scarab_pybind */
