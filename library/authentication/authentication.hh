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
    /*!
     @class authentication
     @author N. S. Oblath

     @brief Loads authentication information from a file or environment variables

     @details
    */
    class SCARAB_API authentication
    {
        public:
            authentication();
            virtual ~authentication();

        public:
            void add_group( const std::string& a_group );
            void add_item( const std::string& a_group, const std::string& a_name, const std::string& a_default, const std::string& an_env = "" );

            void set_auth_file( const std::string& a_filename, const scarab::param_node& a_read_opts = scarab::param_node() );

            void process_design();

            mv_referrable( scarab::param_node, design );

        protected:
            void load_from_file( const std::string& a_auth_file, const scarab::param_node& a_read_opts );
            
        public:
            /// Retrieve a particular item from the authentication data
            /// Will throw std::out_of_range if the group or item does not exist
            std::string get(const std::string& a_group, const std::string& an_item) const;

            mv_referrable( scarab::param_node, data );

    };

/*
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
*/
} /* namespace scarab */

#endif /* SCARAB_AUTHENTICATION_HH_ */
