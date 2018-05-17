/*
 * mt_authentication.hh
 *
 *  Created on: Jun 15, 2015
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_AUTHENTICATION_HH_
#define SCARAB_AUTHENTICATION_HH_

#include "member_variables.hh"
#include "param.hh"
#include "scarab_api.hh"

namespace scarab
{

    class SCARAB_API authentication : public param_node
    {
        public:
            authentication( const std::string& a_auth_file );
            ~authentication();

        public:
            bool load( const std::string& a_auth_file );

        public:
            mv_referrable( std::string, auth_filename );
            mv_accessible_noset( bool, is_loaded );

    };

} /* namespace scarab */

#endif /* SCARAB_AUTHENTICATION_HH_ */
