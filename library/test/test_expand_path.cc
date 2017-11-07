/*
 * test_expand_path.cc
 *
 *  Created on: Jun 30, 2017
 *      Author: obla999
 */

#include "path.hh"

#include "logger.hh"

#include <string>
#include <vector>

using namespace scarab;

LOGGER( testlog, "test_expand_path" );

int main()
{
    std::vector< std::string > t_to_expand;
    t_to_expand.push_back( "in_curr_dir.txt" );
    t_to_expand.push_back( "/tmp/in_slash_tmp.txt" );
    t_to_expand.push_back( "subdir/in_subdir.txt" );
    t_to_expand.push_back( "subdir\ with\ spaces/in_subdir_w_spaces.txt" );

    for( auto it = t_to_expand.begin(); it != t_to_expand.end(); ++it )
    {
        path t_expanded( expand_path( *it ) );
        LINFO( testlog, "Expanded <" << *it << "> to <" << t_expanded << ">" );
    }

    return 0;
}

