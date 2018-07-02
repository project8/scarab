/*
 * time.cc
 *
 *  Created on: Oct 29, 2013
 *      Author: nsoblath
 */


#define SCARAB_API_EXPORTS

#include "time.hh"

#include "date.h"

namespace scarab
{

    // Combined date & time, according to the ISO 8601 standard: e.g. 2015-01-31T22:35:58Z
    SCARAB_API char date_time_format[] = "%Y-%m-%dT%H:%M:%SZ";

    SCARAB_API std::string get_formatted_now()
    {
        using namespace std::chrono;
        return date::format(scarab::date_time_format, time_point_cast<microseconds>(system_clock::now()));
    }

} /* namespace scarab */
