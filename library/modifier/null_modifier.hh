/*
 * null_modifier.hh
 *
 *  Created on: Feb 12, 2020
 *      Author: N. Buzinsky
 */

#ifndef SCARAB_NULL_MODIFIER_HH_
#define SCARAB_NULL_MODIFIER_HH_

#include "modifier.hh"

namespace scarab
{

    class SCARAB_API null_modifier : public modifier
    {
        public:
            void modify(param_node &a_config);
    };

} /* namespace scarab */

#endif /* SCARAB__NULL_MODIFIER_HH_ */
