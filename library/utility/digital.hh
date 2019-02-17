/*
 * digital.h
 *
 * written by jared kofron <jared.kofron@gmail.com>
 *
 * functions for converting from D2A and A2D.  prototypes are declared in
 * digital.h.  versions are included for both floats and doubles.  there are
 * two styles of the functions inspired by the GSL way of doing things - a 
 * normal version which returns the simple type of interest, and an 'error' 
 * version which can return information about bad arguments.
 */

#ifndef SCARAB_DIGITAL_HH_
#define SCARAB_DIGITAL_HH_

#include "scarab_api.hh"

#include <cmath>
#include <type_traits>

namespace scarab
{

    struct SCARAB_API dig_calib_params
    {
        unsigned bit_depth;
        unsigned levels;
        unsigned data_type_size;
        double v_range;
        double v_offset;
        double inv_levels;
        double inv_v_range;
        double dac_gain;
        bool bits_right_aligned;
    };



    SCARAB_API void get_calib_params( unsigned n_bits, unsigned data_type_size, double v_offset, double v_range, bool bits_r_aligned, dig_calib_params *params );
    SCARAB_API void get_calib_params2( unsigned n_bits, unsigned data_type_size, double v_offset, double v_range, double dac_gain, bool bits_r_aligned, dig_calib_params *params );


    namespace detail {
        enum class is_signed {};
        enum class is_unsigned {};
    }

    template< typename condition >
    using enable_if_signed = typename std::enable_if< condition::value, detail::is_signed >::type;

    template< typename condition >
    using enable_if_unsigned = typename std::enable_if< condition::value, detail::is_unsigned >::type;

    /*
     * convert an unsigned digital <=64 bit value to a double or float.
     */
    template< typename dig_type, typename an_type,
              enable_if_unsigned< std::is_unsigned<dig_type> >... >
    an_type d2a( dig_type dig, const struct dig_calib_params* params )
    {
        return params->v_offset + params->dac_gain * ( an_type )dig;
    }

    /*
     * convert a signed digital <=64 bit value to a double or float.
     */
    template< typename dig_type, typename an_type,
              enable_if_signed< std::is_signed<dig_type> >... >
    an_type d2a( dig_type dig, const struct dig_calib_params* params )
    {
        return params->v_offset + params->dac_gain * (( an_type )dig + 0.5 * params->levels);
    }

    /*
     * convert an analog value to an unsigned digital value.
     */
    template< typename an_type, typename dig_type,
                enable_if_unsigned< std::is_unsigned<dig_type> >... >
              //typename = typename std::enable_if< std::is_unsigned<dig_type>::value, void >::type >
    dig_type a2d( an_type analog, const struct dig_calib_params* params )
    {
        analog = ( analog - params->v_offset ) * params->inv_v_range * (an_type)(params->levels);
        if( analog > (an_type)(params->levels - 1) ) analog = params->levels - 1;
        else if( analog < 0. ) analog = 0.;
        return (dig_type)std::round( analog );
    }

    /*
     * convert an analog value to a signed digital value.
     */
    template< typename an_type, typename dig_type,
              enable_if_signed< std::is_signed<dig_type> >... >
              //typename = typename std::enable_if< std::is_signed<dig_type>::value, void >::type >
    dig_type a2d( an_type analog, const struct dig_calib_params* params )
    {
        analog = ( analog - params->v_offset ) * params->inv_v_range * (an_type)(params->levels);
        if( analog > (an_type)(params->levels - 1) ) analog = params->levels - 1;
        else if( analog < 0. ) analog = 0.;
        return (dig_type)std::round( analog - 0.5 * params->levels );
    }

}

#endif // SCARAB_DIGITAL_HH_
