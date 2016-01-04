#ifndef SCARAB_EXCEPTION_HH_
#define SCARAB_EXCEPTION_HH_

#include <sstream>
#include <exception>

#include "scarab_api.hh"

namespace scarab
{

    class SCARAB_API error : public ::std::exception
    {
        public:
            error();
            error( const error& );
            ~error() throw ();

            template< class x_streamable >
            error& operator<<( const x_streamable& a_fragment )
            {
                f_error << a_fragment;
                return *this;
            }

            virtual const char* what() const throw();

        private:
            ::std::stringstream f_error;
    };

}

#endif /* SCARAB_EXCEPTION_HH_ */
