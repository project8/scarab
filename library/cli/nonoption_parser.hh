#ifndef SCARAB_NONOPTION_PARSER_HH_
#define SCARAB_NONOPTION_PARSER_HH_

#include "param.hh"

#include "member_variables.hh"

#include <string>

namespace scarab
{

    class nonoption_parser
    {
        public:
            nonoption_parser( std::vector< std::string > an_args );
            virtual ~nonoption_parser();

            //void parse( int an_argc, char** an_argv );

            mv_referrable_const( param_array, ord_args );
            mv_referrable_const( param_node, kw_args );

        private:
            void parse( const std::string& an_arg );

            param_ptr_t parse_kw_arg( const std::string& an_addr, const std::string& a_value );

            void add_next( param& a_parent, const std::string& an_addr_in_parent, const std::string& a_next_addr, const std::string& a_value );

            param_ptr_t new_param_from_addr( const std::string& an_addr );

            param_ptr_t parse_value( const std::string& a_value );

        public:
            static const char f_value_separator = '=';
            static const char f_node_separator = '.';

    };

} /* namespace scarab */

#endif /* SCARAB_NONOPTION_PARSER_HH_ */
