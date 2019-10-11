/*
 * version_ifc.hh
 *
 *  Created on: Jul 23, 2018
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_VERSION_IFC_HH_
#define SCARAB_VERSION_IFC_HH_

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

} /* namespace scarab */

#endif /* SCARAB_VERSION_IFC_HH_ */
