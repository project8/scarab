/*
 * stream_check.hh
 *
 *  Created on: Jul 17, 2020
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_TESTING_STREAM_CHECK_HH_
#define SCARAB_TESTING_STREAM_CHECK_HH_

#include <sstream>


namespace scarab
{
    namespace testing
    {

        bool compare_and_clear( std::stringstream* a_stream, const std::string& a_test );

    } /* namespace testing */
} /* namespace scarab */

#endif /* SCARAB_TESTING_STREAM_CHECK_HH_ */