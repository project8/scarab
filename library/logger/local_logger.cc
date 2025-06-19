/*
 * logger.cxx
 * based on KLogger.cxx from KATRIN's Kasper
 *
 *  Created on: 18.11.2011
 *      Author: Marco Haag <marco.haag@kit.edu>
 */

#define SCARAB_API_EXPORTS

#include "local_logger.hh"

#include <chrono>
#include <thread>

using namespace std;

namespace scarab
{

    local_logger::local_logger( const std::string& a_name ) :
            f_buffer_stream(),
            f_buffer(),
            f_threshold(ELevel::eInfo)
    {
        std::cerr << "#### local_logger constructor" << std::endl;
        // Use static initialization to ensure that there's a Quill Backend ready to print messages whenever the first logger is created

        //logger::all_loggers().insert(this);
    }

    local_logger::~local_logger()
    {
        std::cerr << "#### Local_logger destructor" << std::endl;
        //all_loggers().erase(this);
    }

    ELevel local_logger::get_threshold() const
    {
        return f_threshold;
    }

    void local_logger::set_threshold( ELevel a_threshold )
    {
        f_threshold = a_threshold;
        return;
    }

//    ELevel logger::get_global_threshold()
//    {
//        return f_global_threshold;
//    }

//    void logger::set_global_threshold( ELevel a_threshold )
//    {
//        logger::f_global_threshold = a_threshold;
//        std::set< logger* >& t_all_loggers = logger::all_loggers();
//        for( auto t_logger_ptr : t_all_loggers )
//        {
//            t_logger_ptr->set_threshold( a_threshold );
//        }
//        return;
//    }
//
//    std::set< logger* >& logger::all_loggers()
//    {
//        static std::set< logger* > s_all_loggers;
//        return s_all_loggers;
//    }

} // namespace scarab
