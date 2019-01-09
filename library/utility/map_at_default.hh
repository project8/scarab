/*
 * map_at_default.hh
 *
 *  Created on: Jan 8, 2019
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_UTILITY_MAPATDEFAULT
#define SCARAB_UTILITY_MAPATDEFAULT

#include <map>

namespace scarab
{
    template< typename x_key, typename x_value >
    const x_value& at( const std::map< x_key, x_value >& a_map, const x_key& a_key, const x_value& a_default )
    {
        try
        {
            return a_map.at( a_key );
        }
        catch(...)
        {
            return a_default;
        }
    }

} /* end namespace scarab */

#endif /* SCARAB_UTILITY_MAPATDEFAULT */
