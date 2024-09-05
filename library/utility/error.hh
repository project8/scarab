#ifndef SCARAB_EXCEPTION_HH_
#define SCARAB_EXCEPTION_HH_

#include "base_exception.hh"
#include "scarab_api.hh"

namespace scarab
{

    class SCARAB_API error : public typed_exception< error >
    {
        public:
            error() = default;
            error(const std::string& a_filename, int a_line );
            error( const error& ) = default;
            virtual ~error() noexcept = default;
    };

}

#endif /* SCARAB_EXCEPTION_HH_ */
