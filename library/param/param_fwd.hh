/*
 * param_fwd.hh
 *
 *  Created on: Aug 8, 2022
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_FWD_HH_
#define SCARAB_PARAM_FWD_HH_

#include <memory>

namespace scarab
{
    class param;
    class param_value;
    class param_array;
    class param_node;

    typedef std::unique_ptr< param > param_ptr_t;

    class param_visitor;
}

#endif /* SCARAB_PARAM_FWD_HH_ */
