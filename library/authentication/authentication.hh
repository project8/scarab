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
     The interface to this class is split into two parts, the first addressing the authentication specification, and 
     the second addressing the authentication data.

     ### Specification interface
     Groups and items are added with add_group() and add_item(), respectively.  add_item() requires specification 
     of the enclosing group, which will add that group if it's not already there.  For each item, the name and 
     default value are required, while the environment variable is optional.

     Override files and override values can be added to an existing item with set_override_file() and 
     set_override_value(), respectively.  The item they're being added to must already exist; otherwise a 
     scarab::error will be thrown.

     The authentication file is set with set_auth_file().

     When the specification is complete, process_specification() is used to convert the specification to data.

     ### Data interface

     Once the data has been created, individual items can be accessed with the get() function.
    */
    class SCARAB_API authentication
    {
        public:
            authentication();
            authentication( const authentication& ) = default;
            authentication( authentication&& ) = default;
            virtual ~authentication() = default;

            authentication& operator=( const authentication& ) = default;
            authentication& operator=( authentication&& ) = default;

        public:
            /// Adds a group to the specification; replaces the group in the spec if it already exists
            void add_group( const std::string& a_group );
            /// Adds an item to a group; adds the group if it doesn't exist; replaces the item in the spec if it already exists
            void add_item( const std::string& a_group, const std::string& a_name, const std::string& a_default, const std::string& an_env = "" );
            /// Adds a set of groups; for each group provided, replaces the group in the spec if it already exists
            void add_groups( const scarab::param_node& a_groups_node );
            /// Sets an override file for an existing item; if the item does not already exist, throws scarab::error
            void set_override_file( const std::string& a_group, const std::string& a_name, const std::string& a_filename );
            /// Sets an override value for an existing item; if the item does not already exist, throws scarab::error
            void set_override_value( const std::string& a_group, const std::string& a_name, const std::string& a_value );

            /// Sets the filename for an authentication file; note that auth files are not the preferred way of setting up authentication
            void set_auth_file( const std::string& a_filename, const scarab::param_node& a_read_opts = scarab::param_node() );

            void process_spec();

            mv_referrable( scarab::param_node, spec );

        protected:
            param_ptr_t load_from_auth_file( const std::string& a_auth_file, const scarab::param_node& a_read_opts ) const;
            std::string read_from_file( const std::string& a_filename ) const;

        public:
            /// Check whether a group exists in the data
            bool has( const std::string& a_group ) const;
            /// Check whether a group/item combination exists in the data
            bool has( const std::string& a_group, const std::string& an_item ) const;

            /// Retrieve a particular item from the authentication data
            /// Will throw std::out_of_range if the group or item does not exist
            std::string get( const std::string& a_group, const std::string& an_item ) const;
            /// Retrieve a particular item from the authentication data
            /// Returns the default if the group or item does not exist
            std::string get( const std::string& a_group, const std::string& an_item, const std::string& a_default ) const;

            mv_referrable( scarab::param_node, data );

        public:
            /// Check that a file exists and return its path; throws scarab::error if the file does not exist
            path check_path( const std::string& a_filename ) const;

    };

} /* namespace scarab */

#endif /* SCARAB_AUTHENTICATION_HH_ */
