/*
 * param_env_modifier.hh
 *
 *  Created on: Mar 21, 2024
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_ENV_MODIFIER_HH_
#define SCARAB_PARAM_ENV_MODIFIER_HH_

#include "param_modifier.hh"

#include "param_value.hh"

namespace scarab
{
    /*!
     @class param_env_modifier
     @author N. S. Oblath

     @brief Param modifier that substitutes values from environment variables

     @details
     This class uses a param_value visitor to modify string values in particular.  
     The value visitor will replace all environment variable substitutions requested in a string.
    */
    class SCARAB_API param_env_modifier : public param_modifier
    {
        public:
            param_env_modifier();
            virtual ~param_env_modifier();

            using param_modifier::operator(); // bring all of param_modifier's operator() into scope
            virtual void operator()( param_value& ) const override;

            /*!
            @class param_env_modifier::modify_string
            @author N. S. Oblath

            @brief Value visitor that does environment variable substitutions on strings
            */
            class modify_string : public boost::static_visitor< void >
            {
                public:
                    modify_string( const param_env_modifier* a_parent ) :
                            f_pattern_buffer(),
                            f_parent( a_parent )
                    {
                        f_pattern_buffer = a_parent->prefix() + a_parent->var_pattern() + a_parent->postfix();
                    }
                    void operator()( std::string& a_string ) const
                    {
                        while( substitute_env(a_string) ) {}
                        return;
                    }
                    template< typename T >
                    void operator()( T ) const
                    {
                        return;
                    }

                    /// returns true if a substitution was made; false if the string had not ENV{} token
                    /// throws std::regex on regex error
                    /// throws scarab::error on environment variable present but not found or regex returned unexpected results
                    bool substitute_env( std::string& a_string ) const;
                    /// throws std::regex on regex error
                    /// throws scarab::error on environment variable not found
                    std::string get_env( const std::string& a_var ) const;

                protected:
                    mv_referrable( std::string, pattern_buffer );
                    const param_env_modifier* f_parent;
            };

            mv_referrable( std::string, prefix );
            mv_referrable( std::string, var_pattern );
            mv_referrable( std::string, postfix );
    };

    inline void param_env_modifier::operator()( param_value& a_value ) const
    {
        a_value.accept_value_visitor< void >( param_env_modifier::modify_string( this ) );
        return;
    }

} /* namespace scarab */

#endif /* SCARAB_PARAM_ENV_MODIFIER_HH_ */
