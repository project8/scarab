/*
 * param_yaml.cc
 *
 *  Created on: Jan 19, 2016
 *      Author: bidishasen97
 */

#define SCARAB_API_EXPORTS

#include "param_yaml.hh"
#include "param.hh"
#include "logger.hh"

#include <sstream>
#include <fstream>

using std::string;
using std::stringstream;

namespace scarab
{
    LOGGER( slog, "param_yaml" );

    param_input_yaml::param_input_yaml()
    {

    }

    param_input_yaml::~param_input_yaml()
    {

    }

    param_node* param_input_yaml::read_file(const std::string& a_filename)
    {
        /*try
        {
        }
        catch()
        {
        }*/

        YAML::Node root_node = YAML::LoadFile(a_filename); //gets root node
        //check for errors in parsing file put load file line in try catch loop

        return param_input_yaml::read_node_type(root_node);
    }

    param* param_input_yaml::read_node_type(const YAML::Node& a_node)
    {
        if (a_node.IsNull())
        {
            return new param();
        }

        if (a_node.IsScalar())
        {
            return param_input_yaml::read_value(a_node);
        }

        if (a_node.IsSequence())
        {
            return param_input_yaml::sequence_handler(a_node);
        }

        if (a_node.IsMap())
        {

            return param_input_yaml::map_handler(a_node);

        }
    }

    param_array* param_input_yaml::sequence_handler(const YAML::Node& a_node)
    {
        param_array* t_config_array = new param_array();

        for (YAML::const_iterator counter = a_node.begin(); counter != a_node.end(); ++counter)
        {
            YAML::Node scalar_value_node = counter;
            t_config_array->push_back(param_input_yaml::read_node_type(scalar_value_node));

        }

        return t_config_array;
    }

    param_node* param_input_yaml::map_handler(const YAML::Node& a_node)
    {
        param_node* t_config_object = new param_node();

        for (YAML::const_iterator counter = a_node.begin(); counter != a_node.end(); ++counter)
        {
            t_config_object->replace ( counter->first.as<std::string>(), param_input_yaml::read_node_type( counter->second ) );
        }

        return t_config_object;
    }

    param_value* param_input_yaml::read_value(const YAML::Node& a_node)
    {
        try
        {
            return new param_value(a_node.as<unsigned>());
        }
        catch(std::bad_cast& bc)
        {
            try
            {
                return new param_value(a_node.as<bool>());
            }
            catch(std::bad_cast& bc)
            {
                try
                {
                    return new param_value(a_node.as<char>());
                }
                catch(std::bad_cast& bc)
                {
                    try
                    {
                        return new param_value(a_node.as<double>());
                    }
                    catch(std::bad_cast& bc)
                    {
                        try
                        {
                            return new param_value(a_node.as<float>());
                        }
                        catch(std::bad_cast& bc)
                        {
                            try
                            {
                                return new param_value(a_node.as<int>());
                            }
                            catch(std::bad_cast& bc)
                            {

                            }
                        }
                    }
                }
            }
        }
    }

    param_output_yaml::param_output_yaml()
    {

    }

    param_output_yaml::~param_output_yaml()
    {

    }

    bool param_output_yaml::write_file(const param& a_to_write, const std::string& a_filename)
    {
        if (a_filename.empty())
        {
            //ERROR( dlog, “Filename cannot be an empty string” );
            return false;
        }

        FILE* file = fopen(a_filename.c_str(), "w");

        if (file == NULL)
        {
            //ERROR( dlog, "Unable to open file: " << a_filename );
            return false;
        }

        YAML::Node a_node = param_output_yaml::check_param_type( a_to_write );

        std::ofstream fout(a_filename.c_str());
        fout << a_node;
        fclose(file);
        return true;
    }

   YAML::Node& param_output_yaml::check_param_type( const param& a_to_write )
   {
       if( a_to_write.is_null() )
       {
           return YAML::Node();
       }
       else if (a_to_write.is_node())
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
       WARN( slog, "Unknown param type encountered" );
       return YAML::Node();
   }

   YAML::Node& param_output_yaml::param_node_handler( const param& a_to_write )
     {
       YAML::Node t_node;

       const param_node& p_node = static_cast< param_node& > ( a_to_write );
       for (param_node::const_iterator counter = p_node.begin(); counter != p_node.end(); ++counter)
       {
           t_node[ counter->first ] = param_output_yaml::check_param_type(*counter->second);


           //make aliases for repeats
           //use the multimap::find to get location of repeated value can't use multimap. use nested for loop
           //if the key doesn't exist/repeats after this index, then add the key as is
           //if the key repeats before this index, key.toStringBuffer
           //add & to alias and concatonate with existing stringBuffer
           //new string = string buffer
           // set key
       }

       return t_node;
     }

   YAML::Node& param_output_yaml::param_array_handler( const param& a_to_write )
   {
       YAML::Node t_node;

       const param_array array = static_cast< param_array& > ( a_to_write );
       for( int count = 0; count != (int) array.size(); ++count )
       {
           t_node.push_back( param_output_yaml::check_param_type(*array.at(count)) );
       }

       return t_node;
   }

   YAML::Node& param_output_yaml::param_value_handler( const param& a_to_write )
   {
       YAML::Node t_node;

       const param_value& value = static_cast< const param_value& >( a_to_write );
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

       WARN( slog, "Unkown value type encountered" );
       return YAML::Node();
   }
}
/* namespace scarab */

