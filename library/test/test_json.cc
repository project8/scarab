/*
 * test_json.cc
 *
 *  Created on: Jul 18, 2018
 *      Author: N.S. Oblath
 *
 *      Usage: test_json [filename]
 */

#include "param.hh"
#include "param_json.hh"
#include "logger.hh"

using namespace scarab;

LOGGER( slog, "test_json" );

int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        LERROR( slog, "Please provide a JSON file to read" );
        return -1;
    }

    //read file
    param_input_json t_input;
    param_ptr_t t_param_location( t_input.read_file( argv[1] ) );
    if( ! t_param_location )
    {
        LERROR( slog, "File did not read!" );
        return -1;
    }

    LINFO( slog, "File read and parsed: \n" << *t_param_location );

    //write file
    param_output_json t_output;
    bool t_did_write_file = t_output.write_file( *t_param_location, "test_output.json" );

    if( ! t_did_write_file )
    {
        LERROR( slog, "File did not write!" );
        return -1;
    }

    LINFO( slog, "File written successfully (test_output.json)" );
    return 0;
}
