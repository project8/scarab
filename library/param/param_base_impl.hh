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

    inline std::unique_ptr< param > param::clone() const
    {
        //std::cout << "param::clone()" << std::endl;
        return std::unique_ptr< param >( new param( *this ) );
    }

    inline std::unique_ptr< param > param::move_clone()
    {
        //std::cout << "param::clone()" << std::endl;
        return std::unique_ptr< param >( new param( std::move(*this) ) );
    }

    inline bool param::is_null() const
    {
        return true;
    }

    inline bool param::is_value() const
    {
        return false;
    }

    inline bool param::is_array() const
    {
        return false;
    }

    inline bool param::is_node() const
    {
        return false;
    }

    inline param_value& param::as_value()
    {
        if( this->is_value() ) return *static_cast< param_value* >( this);
        throw error() << "Param object is not a value";
    }

    inline param_array& param::as_array()
    {
        if( this->is_value() ) return *static_cast< param_array* >( this);
        throw error() << "Param object is not an array";
    }

    inline param_node& param::as_node()
    {
        if( this->is_value() ) return *static_cast< param_node* >( this);
        throw error() << "Param object is not a node";
    }

    inline const param_value& param::as_value() const
    {
        if( this->is_value() ) return *static_cast< const param_value* >( this);
        throw error() << "Param object is not a value";
    }

    inline const param_array& param::as_array() const
    {
        if( this->is_value() ) return *static_cast< const param_array* >( this);
        throw error() << "Param object is not an array";
    }

    inline const param_node& param::as_node() const
    {
        if( this->is_value() ) return *static_cast< const param_node* >( this);
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

    inline std::string param::to_string() const
    {
        return std::string();
    }

} /* namespace scarab */

#endif /* SCARAB_PARAM_BASE_IMPL_HH_ */
