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

     @brief Loads authentication information from configuration information, an authentication file or environment variables

     @details
     The interface to this class is split into two parts, the first addressing the authentication design, and 
     the second addressing the authentication data.

     ### Design interface
     Groups and items are added with add_group() and add_item(), respectively.  add_item() requires specification 
     of the enclosing group, which will add that group if it's not already there.  For each item, the name and 
     default value are required, while the environment variable is optional.

     The authentication file is set with set_auth_file().

     When the design is complete, process_design() is used to convert the design to data.

     ### Data interface

     Once the data has been created, individual items can be accessed with the get() function.
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
            param_ptr_t load_from_file( const std::string& a_auth_file, const scarab::param_node& a_read_opts );
            
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
