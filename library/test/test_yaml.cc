/*
 * test_yaml.cpp
 *
 *  Created on: Jan 19, 2016
 *      Author: bidishasen97
 */

#include "param_yaml.hh"
#include "logger.hh"

using namespace scarab;

LOGGER( tlog, "test_yaml" );

int main()
{
    //read file
    const param* a_param_location = param_input_yaml::read_file("example.yaml");
    if( a_param_location == NULL )
    {
        ERROR( tlog, "File did not read!" );
        return -1;
    }

    //write file
    bool did_write_file = param_output_yaml::write_file(*a_param_location, "new.yaml");

    if( did_write_file )
    {
        INFO( tlog, "File written successfully" );
        return 0;
    }
    else
    {
        ERROR( tlog, "File did not write!" );
        return -1;
    }

}
