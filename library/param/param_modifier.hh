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


    /*!
     @class param_modifier
     @author N. S. Oblath

     @brief Base class for param modifiers -- visits all elements of the structure but does not modify them

     @details
     Modifier functions are marked as const --- the most intuitive behavior is that the modifier functions 
     don't change the state of the modifier.  If such change is required (e.g. an internal counter needs 
     to change at each visit), that should be marked as a mutable member of the modifier.
    */
    class SCARAB_API param_modifier
    {
        public:
            param_modifier();
            virtual ~param_modifier();

            virtual void operator()( param& ) const;
            virtual void operator()( param_array& ) const;
            virtual void operator()( param_node& ) const;
            virtual void operator()( param_value& ) const;
    };


    /*!
     @class param_modifier_callback
     @author N. S. Oblath

     @brief Param modifier allowing runtime modification of modifier behavior via callback functions

     @details
    */
    class SCARAB_API param_modifier_callback : public param_modifier
    {
        public:
            typedef std::function< void (param&) > param_callback_t;
            typedef std::function< void (param_array&) > param_array_callback_t;
            typedef std::function< void (param_node&) > param_node_callback_t;
            typedef std::function< void (param_value&) > param_value_callback_t;

            param_modifier_callback();
            virtual ~param_modifier_callback();

            virtual void operator()( param& ) const override;
            virtual void operator()( param_array& ) const override;
            virtual void operator()( param_node& ) const override;
            virtual void operator()( param_value& ) const override;

        protected:
            mv_referrable( param_callback_t, param_callback );
            mv_referrable( param_array_callback_t, param_array_callback );
            mv_referrable( param_node_callback_t, param_node_callback );
            mv_referrable( param_value_callback_t, param_value_callback );
    };

} /* namespace scarab */

#endif /* SCARAB_PARAM_MODIFIER_HH_ */
