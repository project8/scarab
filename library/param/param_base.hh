/*
 * param_base.hh
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_PARAM_BASE_HH_
#define SCARAB_PARAM_BASE_HH_

#include "error.hh"
#include "param_fwd.hh"

#include <memory>
#include <sstream>
#include <string>

namespace scarab
{
    
    /*!
     @class param
     @author N. S. Oblath

     @brief Base param type and null param object

     @details
    */
    class SCARAB_API param
    {
        public:
            using null = param;

        public:
            param();
            param( const param& orig );
            param( param&& orig );
            virtual ~param();

            param& operator=( const param& rhs );
            param& operator=( param&& );

            virtual param_ptr_t clone() const;
            virtual param_ptr_t move_clone();

            virtual void accept( const param_modifier& a_modifier );
            virtual void accept( const param_visitor& a_visitor ) const;

            virtual bool is_null() const;
            virtual bool is_value() const;
            virtual bool is_array() const;
            virtual bool is_node() const;
            std::string type() const;

            virtual bool has_subset( const param& a_subset ) const;

            param_value& as_value();
            param_array& as_array();
            param_node& as_node();

            const param_value& as_value() const;
            const param_array& as_array() const;
            const param_node& as_node() const;

            /// Assumes that the parameter is a value, and returns a reference to itself.
            const param_value& operator()() const;
            /// Assumes that the parameter is a value, and returns a reference to itself.
            param_value& operator()();

            /// Assumes that the parameter is an array, and returns a reference to the param at a_index.
            /// Behavior is undefined if a_index is out-of-range.
            /// Note that this behavior differs from the C++ STL sequence-like container behavior
            const param& operator[]( unsigned a_index ) const;
            /// Assumes that the parameter is an array, and returns a reference to the param at a_index.
            /// Behavior is undefined if a_index is out-of-range.
            /// Note that this behavior differs from the C++ STL sequence-like container behavior
            param& operator[]( unsigned a_index );

            /// Assumes that the parameter is a node, and returns a reference to the param corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            /// Note that this behavior differs from the C++ STL map-like container behavior
            const param& operator[]( const std::string& a_name ) const;
            /// Assumes that the parameter is a node, and returns a reference to the param corresponding to a_name.
            /// Throws an scarab::error if a_name is not present.
            /// Note that this behavior differs from the C++ STL map-like container behavior
            param& operator[]( const std::string& a_name );

            /// Assumes that the parameter is an array, and returns a reference to the param at a_index.
            /// Behavior is undefined if a_index is out-of-range.
            const param& at( unsigned a_index ) const;
            /// Assumes that the parameter is an array, and returns a reference to the param at a_index.
            /// Behavior is undefined if a_index is out-of-range.
            param& at( unsigned a_index );

            /// Assumes that the parameter is a node, and returns a reference to the param corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            const param& at( const std::string& a_name ) const;
            /// Assumes that the parameter is a node, and returns a reference to the param corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            param& at( const std::string& a_name );

            /// If the derived param type is param_node, forwards the reqeust to param_node; otherwise throws an error.
            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            std::string get_value( const std::string& a_name, const std::string& a_default ) const;
            std::string get_value( const std::string& a_name, const char* a_default ) const;
            /// If the derived param type is param_node, forwards the reqeust to param_node; otherwise throws an error.
            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            template< typename XValType >
            XValType get_value( const std::string& a_name, XValType a_default ) const;

            /// If the derived param type is param_node, forwards the reqeust to param_node; otherwise throws an error.
            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            std::string get_value( unsigned a_index, const std::string& a_default ) const;
            std::string get_value( unsigned a_index, const char* a_default ) const;
            /// If the derived param type is param_node, forwards the reqeust to param_node; otherwise throws an error.
            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            template< typename XValType >
            XValType get_value( unsigned a_index, XValType a_default ) const;

            void merge( const param& a_param );

            virtual std::string to_string() const;

            static unsigned s_indent_level;
    };

    SCARAB_API std::ostream& operator<<(std::ostream& out, const param& value);

} /* namespace scarab */

#endif /* SCARAB_PARAM_BASE_HH_ */
