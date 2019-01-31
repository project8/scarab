#ifndef SCARAB_NONOPTION_PARSER_HH_
#define SCARAB_NONOPTION_PARSER_HH_

#include "param.hh"

#include "member_variables.hh"

#include <string>

namespace scarab
{

    class SCARAB_API nonoption_parser
    {
        public:
            nonoption_parser( std::vector< std::string > an_args );
            virtual ~nonoption_parser();

            mv_referrable_const( param_array, ord_args );
            mv_referrable_const( param_node, kw_args );

        private:
            void parse( const std::string& an_arg );

        public:
            static const char f_value_separator = '=';
            static const char f_option_starter = '-';

    };

} /* namespace scarab */

#endif /* SCARAB_NONOPTION_PARSER_HH_ */
