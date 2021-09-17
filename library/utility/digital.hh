/*
 * digital.hh
 *
 * Created on: ~2016
 *     Author: N.S. Oblath
 *
 *
 */

#ifndef SCARAB_DIGITAL_HH_
#define SCARAB_DIGITAL_HH_

#include "scarab_api.hh"

#include <cmath>
#include <type_traits>

namespace scarab
{

    /** \struct dig_calib_params
        \brief Collection of parameters used for converting between analog and digital data

        Typically this struct is filled in with one of the get_calib_params functions.

        Notes on parameters that are typically specified by the user:

        \param bit_depth The number of bits used by the digitized data (<= to the size of the data type)

        \param data_type_size The size of the digitized data type in bytes

        \param v_range The voltage range covered by the digitization

        \param v_offset The voltage offset for the measurement.  The specific meaning of this depends on whether
        you're recording your data as variations about some middle point, or as positive variations above a pedestal.
        In the former case, it's assumed (by how the a2d function is setup) that you'll be using a signed datatype for
        the digitized data.  In the latter case, it's assumed that you'll be using an unsigned datatype.

        \param bits_right_aligned Whether the bits within the data type are right aligned or left-aligned

        \param dac_gain Some digitizers libraries provide the DAC gain that should be used for reconstructing analog values.
     */
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


    /// Calculate the digitizer calibration parameters with basic parameters: number of bits, Voffset, and Vrange
    SCARAB_API void get_calib_params( unsigned n_bits, unsigned data_type_size, double v_offset, double v_range, bool bits_r_aligned, dig_calib_params *params );
    /// Calculate the digitizer calibration parameters when given the DAC gain (e.g. from a digitizer's own calibration)
    SCARAB_API void get_calib_params2( unsigned n_bits, unsigned data_type_size, double v_offset, double v_range, double dac_gain, bool bits_r_aligned, dig_calib_params *params );


    //namespace detail {
    //    enum class is_signed {};
    //    enum class is_unsigned {};
    //}

    template< typename type >
    using enable_if_signed = typename std::enable_if< std::is_signed<type>::value >::type*;

    template< typename type >
    using enable_if_unsigned = typename std::enable_if< std::is_unsigned<type>::value >::type*;

    /// Convert a signed or unsigned digital value to an analog value.
    template< typename dig_type, typename an_type >
    an_type d2a( dig_type dig, const struct dig_calib_params* params )
    {
        return params->v_offset + params->dac_gain * ( an_type )dig;
    }

    /// Convert an analog value to an unsigned digital value.
    template< typename an_type, typename dig_type,
                enable_if_unsigned< dig_type > = nullptr  >
    dig_type a2d( an_type analog, const struct dig_calib_params* params )
    {
        analog = ( analog - params->v_offset ) * params->inv_v_range * (an_type)(params->levels);
        if( analog > (an_type)(params->levels - 1) ) analog = params->levels - 1;
        else if( analog < 0. ) analog = 0.;
        return (dig_type)std::round( analog );
    }

    ///Convert an analog value to a signed digital value.
    template< typename an_type, typename dig_type,
              enable_if_signed< dig_type > = nullptr >
    dig_type a2d( an_type analog, const struct dig_calib_params* params )
    {
        double half_levels = params->levels * 0.5;
        analog = ( analog - params->v_offset ) * params->inv_v_range * (an_type)(params->levels);
        if( analog > (an_type)(half_levels - 1) ) analog = half_levels - 1;
        else if( analog < -half_levels ) analog = -half_levels;
        return (dig_type)std::round( analog );
    }

}

#endif // SCARAB_DIGITAL_HH_
