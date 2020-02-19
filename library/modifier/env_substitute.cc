/*
 * env_substitute.cc
 *
 *  Created on: Feb 19, 2020
 *      Author: N. Buzinsky
 */

#define SCARAB_API_EXPORTS

#include "env_substitute.hh"
#include "logger.hh"

#include <iostream>
#include <cstdlib>
#include <regex>

namespace scarab
{
    LOGGER( mm2log, "env_substitute" );

    void env_substitute::modify(param_node &a_config) //use better type checking?
    {
        for( auto it = a_config.begin(); it != a_config.end(); ++it )
        {
            std::string old_value = (*it).to_string();
            std::string new_value = substitute_environmentals(old_value);
            if( new_value != old_value)
                a_config.replace(it.name(),new_value);
        }
        return;
    }
    
    std::string env_substitute::substitute_environmentals(std::string config_field) const
    {
        std::string new_value = config_field;

        if(valid_env_name(config_field))
        {
            config_field.erase(0,1); //erase leading dollar sign
            LDEBUG( mm2log, "Found environmental variable in config: " << config_field);
            char *p  = std::getenv(config_field.c_str());

            if(!p)
            {
                LERROR( mm2log, "Variable $" << config_field << " in config not in environment! Leaving as string (not recommended)!");
                return new_value;
            }

            new_value = std::string(p);
        }

        return new_value;
    }

    bool env_substitute::valid_env_name(const std::string& env_name) const
    {
        const std::regex re{"^\\$DRIPLINE_[A-Z0-9][A-Z0-9_]*$"};
        return std::regex_match(env_name, re);
    }

} /* namespace scarab */
