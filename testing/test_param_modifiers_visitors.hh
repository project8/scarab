/*
 * test_param_modifiers_visitors.hh
 *
 *  Created on: May 30, 2017
 *      Author: obla999
 *
 *  The variables in the test_acc struct include a test of doxygen-compliant documentation
 */

#ifndef SCARAB_TEST_PARAM_MODIFIERS_VISITORS_HH_
#define SCARAB_TEST_PARAM_MODIFIERS_VISITORS_HH_

#include "param.hh"
#include "param_modifier.hh"
#include "param_visitor.hh"

#include "logger.hh"

LOGGER( tlog_pmh, "test_param_visitor" )


namespace scarab_testing
{
    using scarab::param;
    using scarab::param_array;
    using scarab::param_node;
    using scarab::param_value;
    using scarab::param_modifier_callback;
    using scarab::param_visitor_callback;
    

    class param_modifier_callback_tester : public param_modifier_callback
    {
        public:
            param_modifier_callback_tester() :
                    param_modifier_callback(),
                    f_param_count( 0 ),
                    f_param_array_count( 0 ),
                    f_param_node_count( 0 ),
                    f_param_value_count( 0 )
            {
                f_param_callback = [this](param&) {
                    LDEBUG( tlog_pmh, "param callback" );
                    this->set_param_count( this->get_param_count() + 1 );
                };
                f_param_array_callback = [this](param_array& an_array) {
                    LDEBUG( tlog_pmh, "param_array callback: " << an_array );
                    this->set_param_array_count( this->get_param_array_count() + 1 );
                    for( auto& it: an_array )
                    {
                        it.accept( *this );
                    }
                };
                f_param_node_callback = [this](param_node& a_node) {
                    LDEBUG( tlog_pmh, "param_node callback: " << a_node );
                    this->set_param_node_count( this->get_param_node_count() + 1 );
                    for( auto& it: a_node )
                    {
                        it.accept( *this );
                    }
                };
                f_param_value_callback = [this](param_value& a_value) {
                    LDEBUG( tlog_pmh, "param_value callback: " << a_value );
                    this->set_param_value_count( this->get_param_value_count() + 1 );
                    a_value.set( 42 );
                };
            }
            mv_accessible( int, param_count );
            mv_accessible( int, param_array_count );
            mv_accessible( int, param_node_count );
            mv_accessible( int, param_value_count );
    };


    class param_visitor_callback_tester : public param_visitor_callback
    {
        public:
            param_visitor_callback_tester() :
                    param_visitor_callback(),
                    f_param_count( 0 ),
                    f_param_array_count( 0 ),
                    f_param_node_count( 0 ),
                    f_param_value_count( 0 ),
                    f_check_magic_value( false ),
                    f_magic_value( 42 )
            {
                f_param_callback = [this](const param&) {
                    LDEBUG( tlog_pmh, "param callback" );
                    this->set_param_count( this->get_param_count() + 1 );
                };
                f_param_array_callback = [this](const param_array& an_array) {
                    LDEBUG( tlog_pmh, "param_array callback: " << an_array );
                    this->set_param_array_count( this->get_param_array_count() + 1 );
                    for( const auto& it: an_array )
                    {
                        it.accept( *this );
                    }
                };
                f_param_node_callback = [this](const param_node& a_node) {
                    LDEBUG( tlog_pmh, "param_node callback: " << a_node );
                    this->set_param_node_count( this->get_param_node_count() + 1 );
                    for( const auto& it: a_node )
                    {
                        it.accept( *this );
                    }
                };
                f_param_value_callback = [this](const param_value& a_value) {
                    LDEBUG( tlog_pmh, "param_value callback: " << a_value );
                    this->set_param_value_count( this->get_param_value_count() + 1 );
                };
            }
            mv_accessible( int, param_count );
            mv_accessible( int, param_array_count );
            mv_accessible( int, param_node_count );
            mv_accessible( int, param_value_count );

            mv_accessible( bool, check_magic_value );
            mv_accessible( int, magic_value );
    };

} /* namespace scarab_testing */

#endif /* SCARAB_TEST_PARAM_MODIFIERS_VISITORS_HH_ */
