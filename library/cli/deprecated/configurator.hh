/*
 * configurator.hh
 *
 *  Created on: Nov 5, 2013
 *      Author: nsoblath
 */

#ifndef SCARAB_CONFIGURATOR_HH_
#define SCARAB_CONFIGURATOR_HH_

#include "error.hh"
#include "param.hh"

namespace scarab
{
    class configurator
    {
        public:
            configurator( int an_argc, char** an_argv, const scarab::param_node& a_default = scarab::param_node() );
            virtual ~configurator();

            const std::string& exe_name() const;

            scarab::param_node& config();
            const scarab::param_node& config() const;

            bool help_flag() const;
            bool version_flag() const;

            template< typename XReturnType >
            XReturnType get( const std::string& a_name ) const;

            template< typename XReturnType >
            XReturnType get( const std::string& a_name, XReturnType a_default ) const;

        private:
            std::string f_exe_name; // the name of the executable being used

            scarab::param_node f_master_config;
            bool f_help_flag;
            bool f_version_flag;
    };

    template< typename XReturnType >
    XReturnType configurator::get( const std::string& a_name ) const
    {
        return f_master_config.get_value< XReturnType >( a_name );
    }

    template< typename XReturnType >
    XReturnType configurator::get( const std::string& a_name, XReturnType a_default ) const
    {
        return f_master_config.get_value< XReturnType >( a_name, a_default );
    }

    inline bool configurator::help_flag() const
    {
        return f_help_flag;
    }

    inline bool configurator::version_flag() const
    {
        return f_version_flag;
    }

    inline const std::string& configurator::exe_name() const
    {
        return f_exe_name;
    }

    inline scarab::param_node& configurator::config()
    {
        return f_master_config;
    }

    inline const scarab::param_node& configurator::config() const
    {
        return f_master_config;
    }


} /* namespace psyllid */
#endif /* PSYLLID_CONFIGURATOR_HH_ */
