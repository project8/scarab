/*
 * version_base.hh
 *
 *  Created on: Jul 23, 2018
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_VERSION_BASE_HH_
#define SCARAB_VERSION_BASE_HH_

#include "scarab_api.hh"

#include <string>

namespace scarab
{

    class SCARAB_API version_ifc
    {
        public:
            version_ifc();
            version_ifc( const version_ifc& );
            virtual ~version_ifc();

            version_ifc& operator=( const version_ifc& );

            virtual unsigned major_version() const = 0;
            virtual unsigned minor_version() const = 0;
            virtual unsigned patch_version() const = 0;

            virtual const std::string& version_str() const = 0;

            virtual const std::string& package() const = 0;
            virtual const std::string& commit() const = 0;

            virtual const std::string& exe_name() const = 0;
            virtual const std::string& hostname() const = 0;
            virtual const std::string& username() const = 0;

            virtual std::string version_info_string() const = 0;
    };

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

            virtual const std::string& package() const;
            virtual const std::string& commit() const;

            virtual const std::string& exe_name() const;
            virtual const std::string& hostname() const;
            virtual const std::string& username() const;

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

            std::string f_package;
            std::string f_commit;

            std::string f_exe_name;
            std::string f_hostname;
            std::string f_username;
    };


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

    inline const std::string& version_semantic::package() const
    {
        return f_package;
    }

    inline const std::string& version_semantic::commit() const
    {
        return f_commit;
    }

    inline const std::string& version_semantic::exe_name() const
    {
        return f_exe_name;
    }
    inline const std::string& version_semantic::hostname() const
    {
        return f_hostname;
    }
    inline const std::string& version_semantic::username() const
    {
        return f_username;
    }

} /* namespace scarab */

#endif /* SCARAB_VERSION_BASE_HH_ */
