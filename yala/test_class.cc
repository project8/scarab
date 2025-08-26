#include "test_class.hh"

#include "yala.hh"


static yala::logger_type< yala::spd_initializer_async_stdout_color_mt > t_logger( "test_class" );

namespace yala
{
    test_class::test_class()
    {
        LINFO( t_logger, "Creating test_class" );
        // We create a dedicated logger in the constructor because we know this will be used for a static object
        // This avoid any static initialization order issue
        //LOGGER( tlog_const, "test_static_initialization" );
        //LPROG( tlog_const, "Constructing test_class object");
    }

    test_class::~test_class()
    {
        LINFO( t_logger, "Destructing test_class" );
        // We create a dedicated logger in the destructor because we know this will be used for a static object
        // This avoid any static destruction order issue
        //LOGGER( tlog_dest, "test_static_initialization" );
        //LPROG( tlog_dest, "Destructing test_class object" );
    }

    void test_class::print_something()
    {
        LINFO( t_logger, "Printing something" );
        //LPROG( tlog, "Printing something" );
        return;
    }
}
