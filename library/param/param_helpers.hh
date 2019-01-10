/*
 * param_helpers.hh
 *
 *  Created on: Jan 10, 2019
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_HELPERS_HH_
#define SCARAB_PARAM_HELPERS_HH_

#include "param.hh"

namespace scarab
{
    struct SCARAB_API simple_parser
    {
        public:
            /// Converts an address into a nested param structure, and optionally attaches a value
            static param_ptr_t parse_address( const std::string& an_addr, param_ptr_t a_value = param_ptr_t() );

            /// Attempts to determine whether a value encoded as a string is an int, and unsigned int, or a floating-point number
            static param_ptr_t parse_value( const std::string& a_value );

        private:
            static void add_next( param& a_parent, const std::string& an_addr_in_parent, const std::string& a_next_addr, param_ptr_t a_value = param_ptr_t() );

            static param_ptr_t new_param_from_addr( const std::string& an_addr );

        public:
            static const char f_node_separator = '.';
    };
}

#endif /* SCARAB_PARAM_HELPERS_HH_ */
