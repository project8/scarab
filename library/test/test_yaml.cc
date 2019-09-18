/*
 * test_yaml.cc
 *
 *  Created on: Jan 19, 2016
 *      Author: bidishasen97
 *
 *      Usage: test_yaml [filename]
 */

#include "param.hh"
#include "param_yaml.hh"
#include "logger.hh"

using namespace scarab;

LOGGER( slog, "test_yaml" );

int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        LERROR( slog, "Please provide the test YAML file to be read: test_yaml.yaml" );
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

    LINFO( slog, "Checking types" );
    if( ! t_param_location->is_node() )
    {
        LERROR( slog, "Top level is not a node" );
    }
    else
    {
        const param_node& t_top = t_param_location->as_node();
        if( ! t_top["array"].is_array() )
        {
            LERROR( slog, "\"array\" is not an array" );
        }
        if( ! t_top["node"].is_node() )
        {
            LERROR( slog, "\"node\" is not a node" );
        }
        if( ! t_top["value-bool"].is_value() )
        {
            LERROR( slog, "\"value-bool\" is not a value" );
        }
        else
        {
            if( ! t_top["value-bool"]().is_bool() )
            {
                LERROR( slog, "\"value-bool\" is not bool" );
            }
        }
        if( ! t_top["value-float"].is_value() )
        {
            LERROR( slog, "\"value-float\" is not a value" );
        }
        else
        {
            if( ! t_top["value-float"]().is_double() )
            {
                LERROR( slog, "\"value-float\" is not float" );
            }
        }
        if( ! t_top["value-int"].is_value() )
        {
            LERROR( slog, "\"value-int\" is not a value" );
        }
        else
        {
            if( ! t_top["value-int"]().is_int() )
            {
                LERROR( slog, "\"value-int\" is not int" );
            }
        }
        if( ! t_top["value-string"].is_value() )
        {
            LERROR( slog, "\"value-string\" is not a value" );
        }
        else
        {
            if( ! t_top["value-string"]().is_string() )
            {
                LERROR( slog, "\"value-string\" is not string" );
            }
        }
        if( ! t_top["value-unsigned"].is_value() )
        {
            LERROR( slog, "\"value-unsigned\" is not a value" );
        }
        else
        {
            if( ! t_top["value-unsigned"]().is_uint() )
            {
                LERROR( slog, "\"value-unsigned\" is not unsigned" );
            }
        }
        LINFO( slog, "If there were no errors, then the type tests passed" );
    }
    
    
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
