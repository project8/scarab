/*
 * null_modifier.cc
 *
 *  Created on: Feb 12, 2020
 *      Author: N. Buzinsky
 */

#define SCARAB_API_EXPORTS

#include "null_modifier.hh"
#include <iostream>


namespace scarab
{
    //LOGGER( mmlog, "null_modifier" );

    void null_modifier::modify(param_node &a_config)
    {
        //LDEBUG( mmlog, "Complex modifications going on ....." );
        std::cout<<"Complex modifications going on ....."<<std::endl;
        return;
    }

} /* namespace scarab */
