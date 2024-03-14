/*
 * env_substitute.hh
 *
 *  Created on: Feb 19, 2020
 *      Author: N. Buzinsky
 */

#ifndef SCARAB_ENV_SUBSTITUTE_HH_
#define SCARAB_ENV_SUBSTITUTE_HH_

#include "modifier.hh"

#include <string>

namespace scarab
{
    class param;
    class param_array;
    class param_node;
    class param_value;

    class SCARAB_API env_substitute : public modifier
    {
        public:
            void modify( param_node &a_config );

        private:
            void recurse( param_node* a_node );
            void recurse( param_array* an_array );
            void recurse( param_value* a_value );
            //void recurse( param* a_param );


            std::string substitute_environmentals( std::string config_field ) const;
            bool valid_env_name( const std::string& env_name ) const;
            template <class param_iterator>
            void recurse_param( param_iterator &a_param );
    };

} /* namespace scarab */

#endif /* SCARAB_ENV_SUBSTITUTE_HH_ */
