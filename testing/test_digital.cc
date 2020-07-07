/*
 * test_digital.cc
 *
 *  Created on: Feb 15, 2019
 *      Author: N.S. Oblath
 *
 *  Expected output (approximately for the floating-point side of the DAC tests)
 *

 *
 *      DAC Unsigned Test
 *      0 --> 0
 *      0 --> 0
 *      26 --> 0.101562
 *      128 --> 0.5
 *      230 --> 0.898438
 *      255 --> 0.996094
 *      255 --> 0.996094
 *
 *      DAC Signed Test
 *      -128 --> -0.5
 *      -128 --> -0.5
 *      -102 --> -0.398438
 *      0 --> 0
 *      102 --> 0.398438
 *      127 --> 0.496094
 *      127 --> 0.496094
 *
 */

#include "digital.hh"

#include "catch.hpp"

#include <iostream>


TEST_CASE( "digital", "[utility]" )
{
    SECTION( "adc_unsigned" )
    {
        std::cout << "ADC Unsigned Test" << std::endl;
        // 8 bit digitized data
        // V offset = 0 V
        // V range = 1 V
        // Expected analog data: varying between 0 V and 1 V

        scarab::dig_calib_params t_params;
        scarab::get_calib_params( 8, 1, 0., 1., true, &t_params );

        std::vector< double > t_analog{ -0.1, 0., 0.1, 0.5, 0.9, 1.0, 1.1 };
        std::vector< unsigned > t_digital{ 0, 0, 26, 128, 230, 255, 255 };

        for( unsigned i = 0; i < t_analog.size(); ++i )
        {
            REQUIRE( scarab::a2d< double, unsigned >( t_analog[i], &t_params ) == t_digital[i] );
        }
    }

    SECTION( "adc_signed" )
    {
        std::cout << "ADC Signed Test" << std::endl;
        // 8 bit digitized data
        // V offset = 0 V
        // V range = 1 V
        // Expected analog data: symmetric about 0 V with maximum deviation of +/- 0.5 V

        scarab::dig_calib_params t_params;
        scarab::get_calib_params( 8, 1, 0., 1., true, &t_params );

        std::vector< double > t_analog{ -0.6, -0.5, -0.4, 0., 0.4, 0.5, 0.6 };
        std::vector< int > t_digital{ -128, -128, -102, 0, 102, 127, 127 };

        for( unsigned i = 0; i < t_analog.size(); ++i )
        {
            REQUIRE( scarab::a2d< double, int >( t_analog[i], &t_params ) == t_digital[i] );
        }
    }

    SECTION( "dac_unsigned" )
    {
        std::cout << "DAC Unsigned Test" << std::endl;
        // 8 bit digitized data
        // V offset = 0 V
        // V range = 1 V
        // Expected analog data: varying between 0 V and 1 V

        scarab::dig_calib_params t_params;
        scarab::get_calib_params( 8, 1, 0., 1., true, &t_params );

        std::vector< double > t_analog{ 0., 0.1, 0.5, 0.9, 1.0 };
        std::vector< unsigned > t_digital{ 0, 26, 128, 230, 255 };

        const double t_tolerance = t_params.v_range * t_params.inv_levels;

        for( unsigned i = 0; i < t_digital.size(); ++i )
        {
            REQUIRE_THAT( (scarab::d2a< unsigned, double >( t_digital[i], &t_params )), Catch::WithinAbs( t_analog[i], t_tolerance ) );
        }
    }

    SECTION( "dac_signed" )
    {
        std::cout << "DAC Signed Test" << std::endl;
        // 8 bit digitized data
        // V offset = 0 V
        // V range = 1 V
        // Expected analog data: varying between 0 V and 1 V

        scarab::dig_calib_params t_params;
        scarab::get_calib_params( 8, 1, 0., 1., true, &t_params );

        std::vector< double > t_analog{ -0.5, -0.4, 0., 0.4, 0.5 };
        std::vector< int > t_digital{ -128, -102, 0, 102, 127 };

        const double t_tolerance = t_params.v_range * t_params.inv_levels;

        for( unsigned i = 0; i < t_digital.size(); ++i )
        {
            REQUIRE_THAT( (scarab::d2a< int, double >( t_digital[i], &t_params )), Catch::WithinAbs( t_analog[i], t_tolerance ) );
        }
    }
}
