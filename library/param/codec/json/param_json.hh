/*
 * param_json.hh
 *
 *  Created on: March 27, 2015
 *      Author: nsoblath, bhlaroque
 */

#ifndef SCARAB_PARAM_JSON_HH_
#define SCARAB_PARAM_JSON_HH_

#include "param_codec.hh"

#include "logger.hh"
#include "param.hh"

#include "rapidjson/fwd.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"

#include <deque>
#include <map>
#include <sstream>
#include <string>


namespace scarab
{
    LOGGER( dlog_param_json, "param_json" );

    //***************************************
    //************** INPUT ******************
    //***************************************

    /*!
     @class param_input_json
     @author N.S. Oblath

     @brief Convert JSON to Param

     @details
     Options: None
    */
    class SCARAB_API param_input_json : public param_input_codec
    {
        public:
            param_input_json();
            virtual ~param_input_json();

            virtual param_ptr_t read_file( const std::string& a_filename, const param_node& a_options = param_node() );
            virtual param_ptr_t read_string( const std::string& a_json_str, const param_node& a_options = param_node() );
            param_ptr_t read_document( const rapidjson::Document& a_document );
            param_ptr_t read_value( const rapidjson::Value& a_value );
    };

    //***************************************
    //************** OUTPUT *****************
    //***************************************

    /*!
     @class param_output_json
     @author N.S. Oblath

     @brief Convert Param to JSON

     @details
     Options:
       - JSON Style
           Pretty print: { "style": param_output_json::k_pretty } or { "style": "pretty" }
           Compact (default): { "style": param_output_json::k_compact } or { "style": "compact" } or anything else
    */
    class SCARAB_API param_output_json : public param_output_codec
    {
        public:
            typedef rapidjson::Writer< rapidjson::FileWriteStream > rj_file_writer;
            typedef rapidjson::PrettyWriter< rapidjson::FileWriteStream > rj_pretty_file_writer;
            typedef rapidjson::Writer< rapidjson::StringBuffer > rj_string_writer;
            typedef rapidjson::PrettyWriter< rapidjson::StringBuffer > rj_pretty_string_writer;

            enum json_writing_style
            {
                k_compact = 0,
                k_pretty = 1
            };

        public:
            param_output_json();
            virtual ~param_output_json();

            virtual bool write_file( const param& a_to_write, const std::string& a_filename, const param_node& a_options = param_node() );
            virtual bool write_string( const param& a_to_write, std::string& a_string, const param_node& a_options = param_node() );

            template< class XWriter >
            bool write_param( const param& a_to_write, XWriter* a_writer );
            template< class XWriter >
            bool write_param_null( const param& a_to_write, XWriter* a_writer );
            template< class XWriter >
            bool write_param_value( const param_value& a_to_write, XWriter* a_writer );
            template< class XWriter >
            bool write_param_array( const param_array& a_to_write, XWriter* a_writer );
            template< class XWriter >
            bool write_param_node( const param_node& a_to_write, XWriter* a_writer );

    };

    template< class XWriter >
    bool param_output_json::write_param( const param& a_to_write, XWriter* a_writer )
    {
        if( a_to_write.is_null() )
        {
            return param_output_json::write_param_null( a_to_write, a_writer );
        }
        if( a_to_write.is_value() )
        {
            return param_output_json::write_param_value( a_to_write.as_value(), a_writer );
        }
        if( a_to_write.is_array() )
        {
            return param_output_json::write_param_array( a_to_write.as_array(), a_writer );
        }
        if( a_to_write.is_node() )
        {
            return param_output_json::write_param_node( a_to_write.as_node(), a_writer );
        }
        LWARN( dlog_param_json, "parameter not written: <" << a_to_write << ">" );
        return false;
    }
    template< class XWriter >
    bool param_output_json::write_param_null( const param& /*a_to_write*/, XWriter* a_writer )
    {
        //LWARN( dlog_param_json, "writing null" );
        a_writer->Null();
        return true;
    }
    template< class XWriter >
    bool param_output_json::write_param_value( const param_value& a_to_write, XWriter* a_writer )
    {
        //LWARN( dlog_param_json, "writing value" );
        if( a_to_write.is_string() )
        {
            a_writer->String( a_to_write.as_string().c_str() );
            //LWARN( dlog_param_json, "writing string to json: " << a_to_write.as_string() );
        }
        else if( a_to_write.is_bool() )
        {
            a_writer->Bool( a_to_write.as_bool() );
            //LWARN( dlog_param_json, "writing bool to json: " << a_to_write.as_bool() );
        }
        else if( a_to_write.is_int() )
        {
            a_writer->Int64( a_to_write.as_int() );
            //LWARN( dlog_param_json, "writing int to json: " << a_to_write.as_int() );
        }
        else if( a_to_write.is_uint() )
        {
            a_writer->Uint64( a_to_write.as_uint() );
            //LWARN( dlog_param_json, "writing uint to json: " << a_to_write.as_uint() );
        }
        else if( a_to_write.is_double() )
        {
            a_writer->Double( a_to_write.as_double() );
            //LWARN( dlog_param_json, "writing double to json: " << a_to_write.as_double() );
        }
        return true;
    }
    template< class XWriter >
    bool param_output_json::write_param_array( const param_array& a_to_write, XWriter* a_writer )
    {
        //LWARN( dlog_param_json, "writing array" );
        a_writer->StartArray();
        for( param_array::const_iterator it = a_to_write.begin(); it != a_to_write.end(); ++it )
        {
            if( ! param_output_json::write_param( *it, a_writer ) )
            {
                LERROR( dlog_param_json, "Error while writing parameter array" );
                return false;
            }
        }
        a_writer->EndArray();
        return true;
    }
    template< class XWriter >
    bool param_output_json::write_param_node( const param_node& a_to_write, XWriter* a_writer )
    {
        //LWARN( dlog_param_json, "writing node" );
        a_writer->StartObject();
        for( param_node::const_iterator it = a_to_write.begin(); it != a_to_write.end(); ++it )
        {
            a_writer->String( it.name().c_str() );
            if( ! param_output_json::write_param( *it, a_writer ) )
            {
                LERROR( dlog_param_json, "Error while writing parameter node" );
                return false;
            }
        }
        a_writer->EndObject();
        return true;
    }


} /* namespace scarab */

#endif /* SCARAB_PARAM_JSON_HH_ */
