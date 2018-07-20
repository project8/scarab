/*
 * param_yaml.cc
 *
 *  Created on: Jan 19, 2016
 *      Author: bidishasen97
 */

#define SCARAB_API_EXPORTS

#include "param_yaml.hh"

#include "logger.hh"
#include "param.hh"

#include <sstream>
#include <fstream>

using std::string;
using std::stringstream;

namespace scarab
{
    LOGGER( slog, "param_yaml" );

    REGISTER_PARAM_INPUT_CODEC( param_input_yaml, "yaml" );

    param_input_yaml::param_input_yaml()
    {}

    param_input_yaml::~param_input_yaml()
    {}

    param_ptr_t param_input_yaml::read_file( const std::string& a_filename, const param_node& )
    {
        try
        {
            YAML::Node root_node = YAML::LoadFile( a_filename );
            return param_input_yaml::read_node_type( root_node );
        }
        catch( YAML::Exception& e )
        {
            LERROR( slog, "YAML error: " << e.what() );
            return std::unique_ptr< param >();
        }
    }

    std::unique_ptr< param > param_input_yaml::read_string( const std::string& a_string, const param_node& )
    {
        try
        {
            YAML::Node root_node = YAML::Load( a_string );
            return param_input_yaml::read_node_type( root_node );
        }
        catch( YAML::Exception& e )
        {
            LERROR( slog, "YAML error: " << e.what() );
            return std::unique_ptr< param >();
        }
    }

    param_ptr_t param_input_yaml::read_node_type( const YAML::Node& a_node )
    {
        try
        {
            if( a_node.IsNull() )
            {
                return param_ptr_t( new param() );
            }
            if( a_node.IsScalar() )
            {
                return param_input_yaml::scalar_handler(a_node);
            }
            if( a_node.IsSequence() )
            {
                return param_input_yaml::sequence_handler(a_node);
            }
            if( a_node.IsMap() )
            {
                return param_input_yaml::map_handler(a_node);
            }
        }
        catch( YAML::Exception& e )
        {
            LERROR(slog, "YAML error in read_node_type: " << e.what());
            throw error() << "YAML error: " << e.what();
        }
        LDEBUG( slog, "YAML unknown" );
        throw error() << "Unknown YAML encountered";
        return std::unique_ptr< param >();
    }

    std::unique_ptr< param_array > param_input_yaml::sequence_handler( const YAML::Node& a_node )
    {
        try
        {
            std::unique_ptr< param_array > t_array_as_param( new param_array() );

            for( YAML::const_iterator counter = a_node.begin(); counter != a_node.end(); ++counter )
            {
                t_array_as_param->push_back( param_input_yaml::read_node_type( *counter ) );
            }

            return t_array_as_param;
        }
        catch( YAML::Exception& e )
        {
            LERROR( slog, "YAML error in sequence_handler: " << e.what() );
            return std::unique_ptr< param_array >();
        }
    }

    std::unique_ptr< param_node > param_input_yaml::map_handler( const YAML::Node& a_node )
    {
        try
        {
            std::unique_ptr< param_node > t_map_as_param( new param_node() );

            for( YAML::const_iterator counter = a_node.begin(); counter != a_node.end(); ++counter )
            {
                t_map_as_param->replace( counter->first.as< std::string >(), param_input_yaml::read_node_type( counter->second ) );
            }

            return t_map_as_param;
        }
        catch( YAML::Exception& e )
        {
            LERROR( slog, "YAML error in map_handler: " << e.what() );
            return std::unique_ptr< param_node >();
        }
    }

    std::unique_ptr< param_value > param_input_yaml::scalar_handler( const YAML::Node& a_node )
    {
        try
        {
            // YAML::Node stores values as strings (in YAML::detail::node_data), so don't worry about trying different value types
            return std::unique_ptr< param_value >( new param_value( a_node.Scalar() ) );
        }
        catch ( YAML::Exception& e )
        {
            LERROR( slog, "YAML error in scalar_handler: " << e.what() )
            return std::unique_ptr< param_value >();
        }
    }


    REGISTER_PARAM_OUTPUT_CODEC( param_output_yaml, "yaml" );

    param_output_yaml::param_output_yaml()
    {}

    param_output_yaml::~param_output_yaml()
    {}

    bool param_output_yaml::write_file( const param& a_to_write, const std::string& a_filename, const param_node& )
    {
        if( a_filename.empty() )
        {
            LERROR( slog, "Filename cannot be an empty string" );
            return false;
        }

        FILE* file = fopen( a_filename.c_str(), "w" );

        if (file == NULL)
        {
            LERROR( slog, "Unable to open file: " << a_filename );
            return false;
        }

        YAML::Node a_node = param_output_yaml::check_param_type(a_to_write);

        std::ofstream fout( a_filename.c_str() );
        fout << a_node;
        fclose( file );

        return true;
    }

    bool param_output_yaml::write_string( const param& a_to_write, std::string& a_string, const param_node& )
    {
        YAML::Node a_node = param_output_yaml::check_param_type( a_to_write );

        std::stringstream t_out;
        t_out << a_node;
        a_string = t_out.str();

        return true;
    }

    YAML::Node param_output_yaml::check_param_type( const param& a_to_write )
    {
        if( a_to_write.is_null() )
        {
            return YAML::Node();
        }
        else if( a_to_write.is_node() )
        {
            return param_output_yaml::param_node_handler( a_to_write );
        }
        else if( a_to_write.is_array() )
        {
            return param_output_yaml::param_array_handler( a_to_write );
        }
        else if( a_to_write.is_value() )
        {
            return param_output_yaml::param_value_handler( a_to_write );
        }
        LWARN( slog, "Unknown param type encountered" );
        return YAML::Node();
    }

    YAML::Node param_output_yaml::param_node_handler( const param& a_to_write )
    {
        YAML::Node t_node;

        const param_node& p_node = a_to_write.as_node();
        for( param_node::const_iterator counter = p_node.begin(); counter != p_node.end(); ++counter )
        {
            t_node[counter.name()] = param_output_yaml::check_param_type( *counter );
        }

        return t_node;
    }

    YAML::Node param_output_yaml::param_array_handler( const param& a_to_write )
    {
        YAML::Node t_node;

        const param_array& array = a_to_write.as_array();
        for ( int count = 0; count != (int) array.size(); ++count )
        {
            t_node.push_back(param_output_yaml::check_param_type( array[count] ) );
        }

        return t_node;
    }

    YAML::Node param_output_yaml::param_value_handler( const param& a_to_write )
    {
        YAML::Node t_node;

        const param_value& value = static_cast< const param_value& >(a_to_write);
        if( value.is_bool() )
        {
            return t_node = value.as_bool();
        }

        else if( value.is_uint() )
        {
            return t_node = value.as_uint();
        }

        else if( value.is_double() )
        {
            return t_node = value.as_double();
        }

        else if( value.is_int() )
        {
            return t_node = value.as_int();
        }

        else if( value.is_string() )
        {
            return t_node = value.as_string();
        }

        LWARN( slog, "Unkown value type encountered" );
        return YAML::Node();
    }
}
/* namespace scarab */

