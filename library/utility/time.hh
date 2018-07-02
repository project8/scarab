#ifndef SCARAB_TIME_HH_
#define SCARAB_TIME_HH_

#include "scarab_api.hh"

#include <string>

namespace scarab
{

    extern SCARAB_API char date_time_format[];

    SCARAB_API std::string get_formatted_now();

} /* namespace scarab */

#endif // SCARAB_TIME_HH_
