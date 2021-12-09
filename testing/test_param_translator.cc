/*
 * test_param_translator.cc
 *
 *  Created on: Dec 9, 2021
 *      Author: N.S. Oblath
 *
 */

#include "param_codec.hh"

#include "logger.hh"

#include "catch.hpp"

LOGGER( testlog, "test_param_translator" )

using namespace scarab;


TEST_CASE( "param_translator", "[param]" )
{
#ifdef USE_CODEC_YAML
    SECTION( "YAML" )
    {
        LINFO( testlog, "Testing YAML translator" )

        std::string test_yaml(
            "a-node:\n"
            "  name1: 1\n"
            "  name2: 2\n"
            "a-array:\n"
            "  - 3\n"
            "  - 4"
        );

        LINFO( testlog, "YAML test string:\n" << test_yaml );

        param_translator translator;

        param_ptr_t translated_yaml = translator.read_string( test_yaml, "yaml" );
        REQUIRE( translated_yaml );
        REQUIRE( (*translated_yaml)["a-node"]["name1"]().as_int() == 1 );
        REQUIRE( (*translated_yaml)["a-node"]["name2"]().as_int() == 2 );
        REQUIRE( (*translated_yaml)["a-array"][0]().as_int() == 3 );
        REQUIRE( (*translated_yaml)["a-array"][1]().as_int() == 4 );

        std::string test_yaml_fromparam;
        REQUIRE( translator.write_string( *translated_yaml, test_yaml_fromparam, "yaml" ) );

        LINFO( testlog, "Back to YAML:\n" << test_yaml_fromparam );
    }
#endif

#ifdef USE_CODEC_JSON
    SECTION( "JSON" )
    {
        LINFO( testlog, "Testing JSON translator" )

        std::string test_json(
            "{\n"
            "  \"a-node\": {\n"
            "    \"name1\": 1,\n"
            "    \"name2\": 2\n"
            "  },\n"
            "  \"a-array\": [3, 4]"
            "}"

        );

        LINFO( testlog, "JSON test string:\n" << test_json );

        param_translator translator;
        param_ptr_t translated_json = translator.read_string( test_json, "json" );
        REQUIRE( translated_json );
        REQUIRE( (*translated_json)["a-node"]["name1"]().as_int() == 1 );
        REQUIRE( (*translated_json)["a-node"]["name2"]().as_int() == 2 );
        REQUIRE( (*translated_json)["a-array"][0]().as_int() == 3 );
        REQUIRE( (*translated_json)["a-array"][1]().as_int() == 4 );

        std::string test_json_fromparam;
        REQUIRE( translator.write_string( *translated_json, test_json_fromparam, "json" ) );

        LINFO( testlog, "Back to JSON:\n" << test_json_fromparam );

    }

#endif

}



