/*
 * version_wrapper.hh
 *
 *  Created on: Jul 23, 2018
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_VERSION_WRAPPER_HH_
#define SCARAB_VERSION_WRAPPER_HH_

#include "scarab_version.hh"

#include "singleton.hh"

namespace scarab
{
    class SCARAB_API version_wrapper : public singleton< version_wrapper >, public version_ifc
    {
        protected:
            friend class singleton< version_wrapper >;
            friend class destroyer< version_wrapper >;
            version_wrapper() : f_imp( new version() ) {}
            ~version_wrapper() {}

        public:
            void set_imp( version_semantic_ptr_t a_imp )
            {
                f_imp = a_imp;
                return;
            }

        public:
            virtual unsigned major_version() const
            {
                return f_imp->major_version();
            }
            virtual unsigned minor_version() const
            {
                return f_imp->minor_version();
            }
            virtual unsigned patch_version() const
            {
                return f_imp->patch_version();
            }

            virtual const std::string& version_str() const
            {
                return f_imp->version_str();
            }

            virtual const std::string& package() const
            {
                return f_imp->package();
            }
            virtual const std::string& commit() const
            {
                return f_imp->commit();
            }

            virtual const std::string& exe_name() const
            {
                return f_imp->exe_name();
            }
            virtual const std::string& hostname() const
            {
                return f_imp->hostname();
            }
            virtual const std::string& username() const
            {
                return f_imp->username();
            }

            virtual std::string version_info_string() const
            {
                return f_imp->version_info_string();
            }

        private:
            version_semantic_ptr_t f_imp;
    };

}

#endif /* SCARAB_VERSION_WRAPPER_HH_ */
