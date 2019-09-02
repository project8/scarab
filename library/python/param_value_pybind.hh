/*
 *
 *  Created on: Feb 1, 2018
 *      Author: N. Oblath, L. Gladstone, B.H. LaRoque
 */

#include "param.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{

    void export_param_value( pybind11::module& mod )
    {
        // param_value
        pybind11::class_< scarab::param_value, scarab::param >( mod, "ParamValue" )
            // initialization overloads by type
            .def( pybind11::init< bool >() )
            .def( pybind11::init< unsigned >() )
            .def( pybind11::init< int >() )
            .def( pybind11::init< double >() )
            .def( pybind11::init< const std::string& >() )
            .def( pybind11::init< const char* >() )

            // python __ special functions
            .def( "__str__", &scarab::param_value::to_string )
            .def( "__eq__", &scarab::param_value::operator== )

            .def( "type", &scarab::param_value::type, "returns a string representation of the stored data type" )
            // type checking methods
            .def( "is_bool", (bool (scarab::param_value::*)() const) &scarab::param_value::is_bool,
                    "Return whether the param_value stores a boolean value" )
            .def( "is_uint", (bool (scarab::param_value::*)() const) &scarab::param_value::is_uint,
                    "Return whether the param_value stores an unsigned integer value" )
            .def( "is_int", (bool (scarab::param_value::*)() const) &scarab::param_value::is_int,
                    "Return whether the param_value stores a signed integer value" )
            .def( "is_double", (bool (scarab::param_value::*)() const) &scarab::param_value::is_double,
                    "Return whether the param_value stores a double value" )
            .def( "is_string", (bool (scarab::param_value::*)() const) &scarab::param_value::is_string,
                    "Return whether the param_value stores a string value" )

            // output cast to native types
            .def( "as_bool", (bool (scarab::param_value::*)() const) &scarab::param_value::as_bool,
                    "Get parameter value as a bool" )
            .def( "as_uint", (unsigned (scarab::param_value::*)() const) &scarab::param_value::as_uint,
                    "Get parameter value as an unsigned integer" )
            .def( "as_int", (int (scarab::param_value::*)() const) &scarab::param_value::as_int,
                    "Get parameter value as a signed integer" )
            .def( "as_double", (double (scarab::param_value::*)() const) &scarab::param_value::as_double,
                    "Get parameter value as a float" )
            .def( "as_string", (const std::string& (scarab::param_value::*)() const) &scarab::param_value::as_string,
                    "Get parameter value as a string" )

            // set method overloads
            .def( "set", (void (scarab::param_value::*)(bool)) &scarab::param_value::set,
                    "Set a bool value" )
            //.def( "set", (void (scarab::param_value::*)(unsigned)) &scarab::param_value::set<uint64_t>, "Set an unsigned integer value" )
            .def("set",&scarab::param_value::set<uint64_t>,
                    "Set an unsigned integer value")
            //.def( "set", (void (scarab::param_value::*)(int)) &scarab::param_value::set<int64_t>, "Set a signed integer value" )
            .def("set", &scarab::param_value::set<int64_t>,
                    "Set a signed integer value")
            .def( "set", (void (scarab::param_value::*)(double)) &scarab::param_value::set,
                    "Set an float value" )
            .def( "set", (void (scarab::param_value::*)(std::string)) &scarab::param_value::set,
                    "Set an string value" )

            //TODO: empty(), clear(), has_subset()

            /*
            .def( "to_python", [](const scarab::param_value& an_obj){
                        pybind11::object to_return;
                        if (an_obj.is_bool()) to_return =  pybind11::cast(an_obj.as_bool());
                        else if (an_obj.is_uint()) to_return = pybind11::cast(an_obj.as_uint());
                        else if (an_obj.is_int()) to_return = pybind11::cast(an_obj.as_int());
                        else if (an_obj.is_double()) to_return = pybind11::cast(an_obj.as_double());
                        else if (an_obj.is_string()) to_return = pybind11::cast(an_obj.as_string());
                        return to_return;
                    } )
            */
            ;
    }

} /* namespace scarab_pybind */
