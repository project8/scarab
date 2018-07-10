/*
 * param_base.hh
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_PARAM_BASE_HH_
#define SCARAB_PARAM_BASE_HH_

#include "error.hh"

#include <memory>
#include <sstream>
#include <string>

namespace scarab
{
    class param_value;
    class param_array;
    class param_node;

    class SCARAB_API param
    {
        public:
            param();
            param( const param& orig );
            param( param&& orig );
            virtual ~param();

            param& operator=( const param& rhs );
            param& operator=( param&& );

            virtual std::unique_ptr< param > clone() const;
            virtual std::unique_ptr< param > move_clone();

            virtual bool is_null() const;
            virtual bool is_value() const;
            virtual bool is_array() const;
            virtual bool is_node() const;

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
            const param& operator[]( unsigned a_index ) const;
            /// Assumes that the parameter is an array, and returns a reference to the param at a_index.
            /// Behavior is undefined if a_index is out-of-range.
            param& operator[]( unsigned a_index );

            /// Assumes that the parameter is a node, and returns a reference to the param corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            const param& operator[]( const std::string& a_name ) const;
            /// Assumes that the parameter is a node, and returns a reference to the param corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            param& operator[]( const std::string& a_name );

            virtual std::string to_string() const;

            static unsigned s_indent_level;
    };

    SCARAB_API std::ostream& operator<<(std::ostream& out, const param& value);

} /* namespace scarab */

#endif /* SCARAB_PARAM_BASE_HH_ */
