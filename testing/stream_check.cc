/*
 * stream_check.cc
 *
 *  Created on: Jul 17, 2020
 *      Author: N.S. Oblath
 */

#include "stream_check.hh"


namespace scarab
{
    namespace testing
    {

        bool compare_and_clear( std::stringstream* a_stream, const std::string& a_test )
        {
            std::string t_log( a_stream->str() );
            if( t_log.empty() && a_test.empty() ) return true;
            bool t_result = t_log.compare( t_log.size() - a_test.size(), a_test.size(), a_test );
            a_stream->str("");
            return t_result;
        }

    } /* namespace testing */
} /* namespace scarab */
