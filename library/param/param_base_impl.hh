/*
 * param_base_impl.hh
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_PARAM_BASE_IMPL_HH_
#define SCARAB_PARAM_BASE_IMPL_HH_

#include "param_base.hh"

#include "param_array.hh"
#include "param_node.hh"
#include "param_value.hh"

#include "param_modifier.hh"
#include "param_visitor.hh"

namespace scarab
{

    inline param& param::operator=( const param& )
    {
        return *this;
    }

    inline param& param::operator=( param&& )
    {
        return *this;
    }

    inline void param::accept( const param_modifier& a_modifier )
    {
        a_modifier( *this );
        return;
    }

    inline void param::accept( const param_visitor& a_visitor ) const
    {
        a_visitor( *this );
        return;
    }

    inline param_value& param::as_value()
    {
        if( this->is_value() ) return *static_cast< param_value* >( this );
        throw error() << "Param object is not a value";
    }

    inline param_array& param::as_array()
    {
        if( this->is_array() ) return *static_cast< param_array* >( this );
        throw error() << "Param object is not an array";
    }

    inline param_node& param::as_node()
    {
        if( this->is_node() ) return *static_cast< param_node* >( this );
        throw error() << "Param object is not a node";
    }

    inline const param_value& param::as_value() const
    {
        if( this->is_value() ) return *static_cast< const param_value* >( this );
        throw error() << "Param object is not a value";
    }

    inline const param_array& param::as_array() const
    {
        if( this->is_array() ) return *static_cast< const param_array* >( this );
        throw error() << "Param object is not an array";
    }

    inline const param_node& param::as_node() const
    {
        if( this->is_node() ) return *static_cast< const param_node* >( this );
        throw error() << "Param object is not a node";
    }

    inline const param_value& param::operator()() const
    {
        return as_value();
    }

    inline param_value& param::operator()()
    {
        return as_value();
    }

    inline const param& param::operator[]( unsigned a_index ) const
    {
        return as_array()[ a_index ];
    }

    inline param& param::operator[]( unsigned a_index )
    {
        return as_array()[ a_index ];
    }

    inline const param& param::operator[]( const std::string& a_name ) const
    {
        return as_node()[ a_name ];
    }

    inline param& param::operator[]( const std::string& a_name )
    {
        return as_node()[ a_name ];
    }

    inline const param& param::at( unsigned a_index ) const
    {
        return as_array().at( a_index );
    }

    inline param& param::at( unsigned a_index )
    {
        return as_array().at( a_index );
    }

    inline const param& param::at( const std::string& a_name ) const
    {
        return as_node().at( a_name );
    }

    inline param& param::at( const std::string& a_name )
    {
        return as_node().at( a_name );
    }

    inline std::string param::get_value( const std::string& a_name, const std::string& a_default ) const
    {
        return as_node().get_value( a_name, a_default );
    }

    inline std::string param::get_value( const std::string& a_name, const char* a_default ) const
    {
        return as_node().get_value( a_name, a_default );
    }

    template< typename XValType >
    inline XValType param::get_value( const std::string& a_name, XValType a_default ) const
    {
        return as_node().get_value( a_name, a_default );
    }

    inline std::string param::get_value( unsigned a_index, const std::string& a_default ) const
    {
        return as_array().get_value( a_index, a_default );
    }

    inline std::string param::get_value( unsigned a_index, const char* a_default ) const
    {
        return as_array().get_value( a_index, a_default );
    }

    template< typename XValType >
    inline XValType param::get_value( unsigned a_index, XValType a_default ) const
    {
        return as_array().get_value( a_index, a_default );
    }

    inline std::string param::to_string() const
    {
        return std::string();
    }

} /* namespace scarab */

#endif /* SCARAB_PARAM_BASE_IMPL_HH_ */
