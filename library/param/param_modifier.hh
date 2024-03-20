/*
 * param_modifier.hh
 *
 *  Created on: Mar 20, 2024
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_MODIFIER_HH_
#define SCARAB_PARAM_MODIFIER_HH_

#include <functional>

#include "member_variables.hh"
#include "scarab_api.hh"

namespace scarab
{
    class param;
    class param_array;
    class param_node;
    class param_value;

    //using params = std::variant< param, param_array, param_node, param_value >;

    class SCARAB_API param_modifier
    {
        public:
            param_modifier();
            virtual ~param_modifier();

            virtual void operator()( param& ) = 0;
            virtual void operator()( param_array& ) = 0;
            virtual void operator()( param_node& ) = 0;
            virtual void operator()( param_value& ) = 0;
    };


    class SCARAB_API param_modifier_callback : public param_modifier
    {
        public:
            typedef std::function< void (param&) > param_callback_t;
            typedef std::function< void (param_array&) > param_array_callback_t;
            typedef std::function< void (param_node&) > param_node_callback_t;
            typedef std::function< void (param_value&) > param_value_callback_t;

            param_modifier_callback();
            virtual ~param_modifier_callback();

            virtual void operator()( param& );
            virtual void operator()( param_array& );
            virtual void operator()( param_node& );
            virtual void operator()( param_value& );

        protected:
            mv_referrable( param_callback_t, param_callback );
            mv_referrable( param_array_callback_t, param_array_callback );
            mv_referrable( param_node_callback_t, param_node_callback );
            mv_referrable( param_value_callback_t, param_value_callback );
    };

} /* namespace scarab */

#endif /* SCARAB_PARAM_MODIFIER_HH_ */
