/*
 * param_yaml.hh
 *
 *  Created on: Jan 19, 2016
 *      Author: bidishasen97
 */

#ifndef PARAM_YAML_HH_
#define PARAM_YAML_HH_

#include "yaml.h";

#include "param.hh";


namespace scarab
{

    //***************************************
    //************** INPUT ******************
    //***************************************

    class param_input_yaml
    {
        public:
            param_input_yaml();
            virtual ~param_input_yaml();

            static param_node* read_file(const std::string& a_filename);
            static param* read_node_type(const YAML::Node& a_node);
            static param_array* sequence_handler(const YAML::Node& a_node);
            static param_node* map_handler( const YAML::Node& a_node );
            static param_value* read_value( const YAML::Node& a_node );
    };

    //***************************************
    //************** OUTPUT *****************
    //***************************************

    class param_output_yaml
        {
            public:
                param_output_yaml();
                virtual ~param_output_yaml();

                static bool write_file();
        };

} /* namespace scarab */

#endif /* PARAM_YAML_HH_ */
