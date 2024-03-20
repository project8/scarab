/*
 * param_visitor.hh
 *
 *  Created on: Mar 18, 2024
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_VISITOR_HH_
#define SCARAB_PARAM_VISITOR_HH_

#include <functional>
//#include <variant>

#include "member_variables.hh"
#include "scarab_api.hh"

namespace scarab
{
    class param;
    class param_array;
    class param_node;
    class param_value;

    //using params = std::variant< param, param_array, param_node, param_value >;

    class SCARAB_API param_visitor
    {
        public:
            param_visitor();
            virtual ~param_visitor();

            virtual void operator()( const param& ) = 0;
            virtual void operator()( const param_array& ) = 0;
            virtual void operator()( const param_node& ) = 0;
            virtual void operator()( const param_value& ) = 0;
    };


    class SCARAB_API param_visitor_callback : public param_visitor
    {
        public:
            typedef std::function< void (const param&) > param_callback_t;
            typedef std::function< void (const param_array&) > param_array_callback_t;
            typedef std::function< void (const param_node&) > param_node_callback_t;
            typedef std::function< void (const param_value&) > param_value_callback_t;

            param_visitor_callback();
            virtual ~param_visitor_callback();

            virtual void operator()( const param& );
            virtual void operator()( const param_array& );
            virtual void operator()( const param_node& );
            virtual void operator()( const param_value& );

        protected:
            mv_referrable( param_callback_t, param_callback );
            mv_referrable( param_array_callback_t, param_array_callback );
            mv_referrable( param_node_callback_t, param_node_callback );
            mv_referrable( param_value_callback_t, param_value_callback );
    };

} /* namespace scarab */

#endif /* SCARAB_PARAM_VISITOR_HH_ */
