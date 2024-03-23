/*
 * param_visitor.hh
 *
 *  Created on: Mar 18, 2024
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_VISITOR_HH_
#define SCARAB_PARAM_VISITOR_HH_

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
     @class param_visitor
     @author N. S. Oblath

     @brief Base class for param visitors -- visits all elements of the param structure

     @details
     Visitor functions are marked as const --- the most intuitive behavior is that the visitor functions 
     don't modifiy the state of the visitor.  If such modification is required (e.g. an internal counter needs 
     to change at each visit), that should be marked as a mutable member of the visitor.
    */
    class SCARAB_API param_visitor
    {
        public:
            param_visitor();
            virtual ~param_visitor();

            virtual void operator()( const param& ) const;
            virtual void operator()( const param_array& ) const;
            virtual void operator()( const param_node& ) const;
            virtual void operator()( const param_value& ) const;
    };


    /*!
     @class param_visitor_callback
     @author N. S. Oblath

     @brief Visitor class allowing runtime updating of visitor behavior via callback functions

     @details
    */
    class SCARAB_API param_visitor_callback : public param_visitor
    {
        public:
            typedef std::function< void (const param&) > param_callback_t;
            typedef std::function< void (const param_array&) > param_array_callback_t;
            typedef std::function< void (const param_node&) > param_node_callback_t;
            typedef std::function< void (const param_value&) > param_value_callback_t;

            param_visitor_callback();
            virtual ~param_visitor_callback();

            virtual void operator()( const param& ) const;
            virtual void operator()( const param_array& ) const;
            virtual void operator()( const param_node& ) const;
            virtual void operator()( const param_value& ) const;

        protected:
            mv_referrable( param_callback_t, param_callback );
            mv_referrable( param_array_callback_t, param_array_callback );
            mv_referrable( param_node_callback_t, param_node_callback );
            mv_referrable( param_value_callback_t, param_value_callback );
    };

} /* namespace scarab */

#endif /* SCARAB_PARAM_VISITOR_HH_ */
