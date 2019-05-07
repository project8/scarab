/*
 * test_yaml.cc
 *
 *  Created on: Jan 19, 2016
 *      Author: bidishasen97
 *
 *      Usage: test_yaml [filename]
 */

#define SCARAB_API_EXPORTS

#include "param.hh"
#include "param_yaml.hh"
#include "logger.hh"

using namespace scarab;

LOGGER( slog, "test_yaml" );

int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        LERROR( slog, "Please provide a YAML file to read" );
        return -1;
    }

    //read file
    param_input_yaml t_input;
    param_ptr_t t_param_location( t_input.read_file( argv[1] ) );
    if( ! t_param_location )
    {
        LERROR( slog, "File did not read!" );
        return -1;
    }

    LINFO( slog, "File read and parsed: \n" << *t_param_location );

    //write file
    param_output_yaml t_output;
    bool t_did_write_file = t_output.write_file( *t_param_location, "test_output.yaml" );

    if( ! t_did_write_file )
    {
        LERROR( slog, "File did not write!" );
        return -1;
    }

    LINFO( slog, "File written successfully (test_output.yaml)" );
    return 0;
}
