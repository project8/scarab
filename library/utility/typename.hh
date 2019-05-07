/*
 * typename.hh
 *
 *  Created on: Feb 10, 2016
 *      Author: nsoblath
 *
 *  Based on: http://stackoverflow.com/a/4541470
 */

#ifndef SCARAB_UTILITY_TYPENAME_HH_
#define SCARAB_UTILITY_TYPENAME_HH_

#include "scarab_api.hh"

#include <string>
#include <typeinfo>

namespace scarab
{

    SCARAB_API std::string demangle( const char* a_name );

    template <class x_type>
    SCARAB_API std::string type( const x_type& a_param )
    {
        return demangle( typeid(a_param).name() );
    }

} /* namespace scarab */


#endif /* SCARAB_UTILITY_TYPENAME_HH_ */
