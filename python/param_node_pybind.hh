/*
 *  Created on: Feb 1, 2018
 *      Author: N. Oblath, L. Gladstone, B.H. LaRoque
 */

#ifndef PARAM_NODE_PYBIND_HH_
#define PARAM_NODE_PYBIND_HH_

#include "param.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{

    std::list< std::string > export_param_node( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        // param_node
        all_members.push_back( "ParamNode" );
        pybind11::class_< scarab::param_node, scarab::param >( mod, "ParamNode", "param data structure object for holding a collection of object indexable by string keys" )
            .def( pybind11::init< >() )

            .def( "__str__", &scarab::param_node::to_string )
            .def( "__len__", &scarab::param_node::size )
            .def( "__getitem__", (scarab::param& (scarab::param_node::*)(const std::string&)) &scarab::param_node::operator[],
                    pybind11::return_value_policy::reference_internal)
            .def( "__setitem__", [](scarab::param_node& an_obj, std::string& a_name, scarab::param& a_value){ an_obj.replace( a_name, a_value ); } )
            .def( "__contains__", &scarab::param_node::has )
            .def( "__iter__", [](const scarab::param_node& an_obj){ return pybind11::make_iterator(an_obj.begin(), an_obj.end()); },
                    pybind11::keep_alive<0, 1>() /* keep object alive while the iterator exists */ )

            .def( "count",
                    &scarab::param_node::count,
                    pybind11::arg( "key" ),
                    "returns the number of occurances of provided key" )

            //TODO: has_subset()

            .def( "empty", &scarab::param_node::empty, "returns whether the map container is empty")

            .def( "add",
                    (bool (scarab::param_node::*)(const std::string&, const scarab::param&)) &scarab::param_node::add,
                    pybind11::arg( "key" ),
                    pybind11::arg( "value" ),
                    "Add a param object to a node at a particular key")

            // Get value of the parameter, bringing along the default value
            .def( "get_value",
                    (bool (scarab::param_node::*)(const std::string&, bool) const) &scarab::param_node::get_value<bool>,
                    pybind11::arg( "key" ),
                    pybind11::arg( "default" ),
                    "Get parameter node value at [key] or return [default] (as a bool)" )
            .def( "get_value",
                    (unsigned (scarab::param_node::*)(const std::string&, unsigned) const) &scarab::param_node::get_value<uint>,
                    pybind11::arg( "key" ),
                    pybind11::arg( "default" ),
                    "Get parameter node value at [key] or return [default] (as an unsigned integer)" )
            .def( "get_value",
                    (int (scarab::param_node::*)(const std::string&, int) const) &scarab::param_node::get_value<int>,
                    pybind11::arg( "key" ),
                    pybind11::arg( "default" ),
                    "Get parameter node value at [key] or return [default] (as a signed integer)" )
            .def( "get_value",
                    (double (scarab::param_node::*)(const std::string&, double) const) &scarab::param_node::get_value<double>,
                    pybind11::arg( "key" ),
                    pybind11::arg( "default" ),
                    "Get parameter node value at [key] or return [default] (as a float)" )
            .def( "get_value",
                    (std::string (scarab::param_node::*)(const std::string&, const std::string& ) const) &scarab::param_node::get_value,
                    pybind11::arg( "key" ),
                    pybind11::arg( "default" ),
                    "Get parameter node value at [key] or return [default] (as a string)" )

            //TODO: merge(), erase(), remove(), clear()


            ;
        return all_members;
    }
} /* namespace scarab_pybind */
#endif /* PARAM_NODE_PYBIND_HH_ */
