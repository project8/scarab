/*
 * param_yaml.hh
 *
 *  Created on: Jan 19, 2016
 *      Author: bidishasen97
 */

#ifndef PARAM_YAML_HH_
#define PARAM_YAML_HH_

#include "yaml-cpp/yaml.h"

#include "param_codec.hh"


namespace scarab
{
    class param_array;
    class param_value;

    //***************************************
    //************** INPUT ******************
    //***************************************

    /*!
     @class param_input_yaml
     @author N.S. Oblath

     @brief Convert YAML to Param

     @details
     Options: None
    */
    class SCARAB_API param_input_yaml : public param_input_codec
    {
        public:
            param_input_yaml();
            virtual ~param_input_yaml();

            virtual param_ptr_t read_file( const std::string& a_filename, const param_node& a_options = param_node() );
            virtual param_ptr_t read_string( const std::string& a_string, const param_node& a_options = param_node() );

            param_ptr_t read_node_type( const YAML::Node& a_node );
            std::unique_ptr< param_array > sequence_handler( const YAML::Node& a_node );
            std::unique_ptr< param_node > map_handler( const YAML::Node& a_node );
            std::unique_ptr< param_value > scalar_handler( const YAML::Node& a_node );
    };

    //***************************************
    //************** OUTPUT *****************
    //***************************************

    /*!
     @class param_output_yaml
     @author N.S. Oblath

     @brief Convert Param to YAML

     @details
     Options: None
    */
    class SCARAB_API param_output_yaml : public param_output_codec
    {
        public:
            param_output_yaml();
            virtual ~param_output_yaml();

            virtual bool write_file( const param& a_to_write, const std::string& a_filename, const param_node& a_options = param_node() );
            virtual bool write_string( const param& a_to_write, std::string& a_string, const param_node& a_options = param_node() );

            YAML::Node check_param_type( const param& a_to_write );
            YAML::Node param_node_handler( const param& a_to_write );
            YAML::Node param_array_handler( const param& a_to_write );
            YAML::Node param_value_handler( const param& a_to_write );
    };

} /* namespace scarab */

#endif /* PARAM_YAML_HH_ */
