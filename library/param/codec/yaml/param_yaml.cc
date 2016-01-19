/*
 * param_yaml.cc
 *
 *  Created on: Jan 19, 2016
 *      Author: bidishasen97
 */

#include "param_yaml.hh"
#include <sstream>
#include <fstream>

using std::string;
using std::stringstream;

namespace scarab
{

    param_input_yaml::param_input_yaml()
    {

    }

    param_input_yaml::~param_input_yaml()
    {

    }

    param_node* param_input_yaml::read_file(const std::string& a_filename)
    {
        YAML::Node root_node = YAML::LoadFile(a_filename); //get root node
        //difference with LoadAllFromFile
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

}
/* namespace scarab */

