/*
 * param_codec.cc
 *
 *  Created on: Aug 10, 2016
 *      Author: obla999
 */

#define SCARAB_API_EXPORTS

#include "param_codec.hh"

#include "factory.hh"
#include "logger.hh"
#include "path.hh"

LOGGER( slog, "param_codec" );

namespace scarab
{

    param_input_codec::param_input_codec()
    {
    }

    param_input_codec::~param_input_codec()
    {
    }


    param_output_codec::param_output_codec()
    {
    }

    param_output_codec::~param_output_codec()
    {
    }


    param_translator::param_translator()
    {
    }

    param_translator::~param_translator()
    {
    }

    param_ptr_t param_translator::read_file( const std::string& a_filename, const param_node& a_options  )
    {
        std::string t_encoding = a_options.get_value( "encoding", "" );
        if( t_encoding.empty() )
        {
            path t_path = expand_path( a_filename );
            t_encoding = t_path.extension().string().substr( 1 ); // remove the '.' at the beginning
        }

        param_input_codec* t_codec = factory< param_input_codec >::get_instance()->create( t_encoding );
        if( t_codec == nullptr )
        {
            LERROR( slog, "Unable to find input codec for encoding <" << t_encoding << ">");
            return nullptr;
        }

        return t_codec->read_file( a_filename, a_options );
    }

    param_ptr_t param_translator::read_string( const std::string& a_string, const param_node& a_options  )
    {
        std::string t_encoding = a_options.get_value( "encoding", "" );
        if( t_encoding.empty() )
        {
            LERROR( slog, "Encoding-type option must be provided");
            return nullptr;
        }

        return read_string( a_string, t_encoding, a_options );
    }

    param_ptr_t param_translator::read_string( const std::string& a_string, const std::string& a_encoding, const param_node& a_options  )
    {
        param_input_codec* t_codec = factory< param_input_codec >::get_instance()->create( a_encoding );
        if( t_codec == nullptr )
        {
            LERROR( slog, "Unable to find input codec for encoding <" << a_encoding << ">");
            return nullptr;
        }

        return t_codec->read_string( a_string, a_options );
    }

    bool param_translator::write_file( const param& a_param, const std::string& a_filename, const param_node& a_options  )
    {
        std::string t_encoding = a_options.get_value( "encoding", "" );
        if( t_encoding.empty() )
        {
            path t_path = expand_path( a_filename );
            t_encoding = t_path.extension().string().substr( 1 ); // remove the '.' at the beginning
        }

        param_output_codec* t_codec = factory< param_output_codec >::get_instance()->create( t_encoding );
        if( t_codec == nullptr )
        {
            LERROR( slog, "Unable to find output codec for encoding <" << t_encoding << ">");
            return false;
        }

        return t_codec->write_file( a_param, a_filename, a_options );
    }

    bool param_translator::write_string( const param& a_param, std::string& a_string, const param_node& a_options  )
    {
        std::string t_encoding = a_options.get_value( "encoding", "" );
        if( t_encoding.empty() )
        {
            LERROR( slog, "Encoding-type option must be provided");
            return false;
        }

        return write_string( a_param, a_string, t_encoding, a_options );
    }

    bool param_translator::write_string( const param& a_param, std::string& a_string, const std::string& a_encoding, const param_node& a_options  )
    {
        param_output_codec* t_codec = factory< param_output_codec >::get_instance()->create( a_encoding );
        if( t_codec == nullptr )
        {
            LERROR( slog, "Unable to find output codec for encoding <" << a_encoding << ">");
            return false;
        }

        return t_codec->write_string( a_param, a_string, a_options );
    }

} /* namespace scarab */
