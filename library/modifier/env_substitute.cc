/*
 * env_substitute.cc
 *
 *  Created on: Feb 19, 2020
 *      Author: N. Buzinsky
 */

#define SCARAB_API_EXPORTS

#include "env_substitute.hh"
#include "logger.hh"
#include "param_array.hh"

#include <iostream>
#include <cstdlib>
#include <regex>

namespace scarab
{
    LOGGER( mm2log, "env_substitute" );

    void env_substitute::modify(param_node &a_config)
    {
        for( auto it = a_config.begin(); it != a_config.end(); ++it )
        {
            recurse_param(it);
        }
        return;
    }

    //takes in param_(array/node)_iterator, recursively goes through until value is found
    template <class param_iterator>
    void env_substitute::recurse_param(param_iterator &a_param)
    {
        if(a_param->is_array())
        {
            auto &a_param_array = a_param->as_array();
            for(auto it = a_param_array.begin(); it != a_param_array.end(); ++it) recurse_param(it);
        }
        else if(a_param->is_node())
        {
            auto &a_param_node = a_param->as_node();
            for(auto it = a_param_node.begin(); it != a_param_node.end(); ++it) recurse_param(it);
        }
        else if(a_param->is_value())
        {
            auto &a_param_value = a_param->as_value();
            if(a_param_value.is_string())
            {
                std::string old_value = a_param_value.to_string();
                std::string new_value = substitute_environmentals(old_value);
                if(old_value != new_value)
                {
                    LDEBUG( mm2log, "Substituting value:  " << old_value <<"  -->  " <<new_value);
                    a_param_value.set(new_value);
                }
            }
        }
    }
    
    std::string env_substitute::substitute_environmentals(std::string old_value) const
    {
        std::string new_value = old_value;
        if(valid_env_name(old_value))
        {
            old_value.erase(0,1); //erase leading dollar sign
            LDEBUG( mm2log, "Found environmental variable in config: " << old_value);
            char *p  = std::getenv(old_value.c_str());

            if(!p)
            {
                LERROR( mm2log, "Variable $" << old_value << " in config not in environment! Leaving as string (not recommended)!");
            }
            else
            {
                new_value = std::string(p);
            }
        }
        return new_value;
    }

    bool env_substitute::valid_env_name(const std::string& env_name) const
    {
        const std::regex re{"^\\$DRIPLINE_[A-Z0-9][A-Z0-9_]*$"};
        return std::regex_match(env_name, re);
    }

} /* namespace scarab */
