
#define SCARAB_API_EXPORTS

#include "error.hh"

namespace scarab
{

    error::error() :
            base_exception()
    {
    }

    error::error( const error& an_error ) :
            base_exception( an_error )
    {
    }

    error::~error() noexcept
    {
    }

}
