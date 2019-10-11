/*
 * version_semantic.hh
 *
 *  Created on: Jul 23, 2018
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_VERSION_SEMANTIC_HH_
#define SCARAB_VERSION_SEMANTIC_HH_

#include "version_ifc.hh"

#include "member_variables.hh"

#include <memory>

namespace scarab
{

    class SCARAB_API version_semantic : public version_ifc
    {
        public:
            version_semantic();
            version_semantic( unsigned a_maj_ver, unsigned a_min_ver, unsigned a_patch_ver );
            version_semantic( const std::string& a_ver );
            version_semantic( const version_semantic& a_orig );
            ~version_semantic();

            version_semantic& operator=( const version_semantic& a_orig );

            /// Less-than operator to compare version information only
            bool operator<( const version_semantic& a_other );
            /// Equality operator to compare version information only
            bool operator==( const version_semantic& a_other );
            /// Greater-than operator to compare version information only
            bool operator>( const version_semantic& a_other );

            virtual unsigned major_version() const;
            virtual unsigned minor_version() const;
            virtual unsigned patch_version() const;

            virtual const std::string& version_str() const;

            virtual std::string version_info_string() const;

        public:
            bool parse( const std::string& a_ver );

            bool combine( unsigned a_maj_ver, unsigned a_min_ver, unsigned a_patch_ver );

            bool update_exe_host_user();

        protected:
            static char s_delimeter;

            unsigned f_major_ver;
            unsigned f_minor_ver;
            unsigned f_patch_ver;
            std::string f_version;

            mv_referrable( std::string, package );
            mv_referrable( std::string, commit );

            mv_referrable( std::string, exe_name );
            mv_referrable( std::string, hostname );
            mv_referrable( std::string, username );
    };

    typedef std::shared_ptr< version_semantic > version_semantic_ptr_t;


    inline unsigned version_semantic::major_version() const
    {
        return f_major_ver;
    }
    inline unsigned version_semantic::minor_version() const
    {
        return f_minor_ver;
    }
    inline unsigned version_semantic::patch_version() const
    {
        return f_patch_ver;
    }

    inline const std::string& version_semantic::version_str() const
    {
        return f_version;
    }

} /* namespace scarab */

#endif /* SCARAB_VERSION_SEMANTIC_HH_ */
