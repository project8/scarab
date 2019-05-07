/*
 * test_digital.cc
 *
 *  Created on: Feb 15, 2019
 *      Author: N.S. Oblath
 *
 *  Expected output (approximately for the floating-point side of the DAC tests)
 *
 *      ADC Unsigned Test
 *      -0.1 --> 0
 *      0 --> 0
 *      0.1 --> 26
 *      0.5 --> 128
 *      0.9 --> 230
 *      1 --> 255
 *      1.1 --> 255
 *
 *      ADC Signed Test
 *      -0.6 --> -128
 *      -0.5 --> -128
 *      -0.4 --> -102
 *      0 --> 0
 *      0.4 --> 102
 *      0.5 --> 127
 *      0.6 --> 127
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

#define SCARAB_API_EXPORTS

#include "digital.hh"

#include <iostream>
#include <vector>

using namespace scarab;

int main()
{
    std::cout << "ADC Unsigned Test" << std::endl;
    // 8 bit digitized data
    // V offset = 0 V
    // V range = 1 V
    // Expected analog data: varying between 0 V and 1 V
    dig_calib_params t_params_1;
    get_calib_params( 8, 1, 0., 1., true, &t_params_1 );

    std::vector< double > t_inputs_1{ -0.1, 0., 0.1, 0.5, 0.9, 1.0, 1.1 };
    std::vector< unsigned > t_outputs_1;

    for( auto t_input : t_inputs_1 )
    {
        t_outputs_1.push_back( a2d< double, unsigned >( t_input, &t_params_1 ) );
        std::cout << t_input << " --> " << t_outputs_1.back() << std::endl;
    }

    std::cout << std::endl;

    std::cout << "ADC Signed Test" << std::endl;
    // 8 bit digitized data
    // V offset = 0 V
    // V range = 1 V
    // Expected analog data: symmetric about 0 V with maximum deviation of +/- 0.5 V

    dig_calib_params t_params_2;
    get_calib_params( 8, 1, 0., 1., true, &t_params_2 );

    std::vector< double > t_inputs_2{ -0.6, -0.5, -0.4, 0., 0.4, 0.5, 0.6 };
    std::vector< int > t_outputs_2;

    for( auto t_input : t_inputs_2 )
    {
        t_outputs_2.push_back( a2d< double, int >( t_input, &t_params_2 ) );
        std::cout << t_input << " --> " << t_outputs_2.back() << std::endl;
    }

    std::cout << std::endl;

    std::cout << "DAC Unsigned Test" << std::endl;
    for( auto t_output : t_outputs_1 )
    {
        std::cout << t_output << " --> " << d2a< unsigned, double >( t_output, &t_params_1 ) << std::endl;
    }

    std::cout << std::endl;

    std::cout << "DAC Signed Test" << std::endl;
    for( auto t_output : t_outputs_2 )
    {
        std::cout << t_output << " --> " << d2a< signed, double >( t_output, &t_params_2 ) << std::endl;
    }

    return 0;
}


