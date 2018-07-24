Applications built with Scarab
==============================

An application essentially encapsulates some behavior or capability with a user interface.  Scarab provides tools to create an interface that allows the user to easily and repeatably configure and run an application.

The main set of classes that a developer will encounter are:

    * ``main_app``: the top-level application class
    * ``app``: application base class; used for subcommands
    * The set of ``param`` classes: data structure classes that hold the configuration information

Configuring an Application
--------------------------

The user tells the application how to run using its configuration.  The master configuration can be put together using several stages of input:

    #. Default configuration: the developer of the application can provide a default hard-coded configuration via ``main_app::default_config()``
    #. Configuration file: supplied via the command line (``-c`` or ``--config``)
    #. Non-option command-line arguments (see below)
    #. Option arguments setup to modify the configuration

Starting from the top down, successive stages of configuration will add to or overwrite (for duplicated configuration keys) the master configuration.  The configuration can be accessed with ``main_app::master_config()``.

Using the Command Line
----------------------

Components of a command::

    > my_exe [subcommand] [options] [non-option arguments]

Subcommands
    An executable using subcommands typically has multiple main modes of operation, each of which is identified by a subcommand.

Options
    An option modifies the behavior of the executable in some predefined way (unknown options are not allowed).  Options are indicated by a hyphen and a single character (e.g. ``-c``) or two hyphens and a string name (e.g. ``--config``).  Options can either be flags, indicating some meaning just by their presence, or they can have some value attached to them.  Options can be given as:

    *    ``-a`` (flag)
    *    ``-abc`` (flags can be combined)
    *    ``-f filename`` (option)
    *    ``-ffilename`` (no space required)
    *    ``-abcf filename`` (flags and option can be combined)
    *    ``--long`` (long flag)
    *    ``--file filename`` (space)
    *    ``--file=filename`` (equals)

    Several options are available with any scarab-based application:

    * ``-h`` or ``--help`` will print the usage message and exit
    * ``-v`` or ``--version`` will print the version information and exit
    * ``-c`` or ``--config`` allows the user to provide a configuration file
    * ``--verbosity`` allows the user to set the global maximum verbosity on a scale of 0 (trace) to 6 (fatal)

Non-Option Arguments
    Arguments that do not start with one or two hyphens are called non-option arguments.  They are parsed in two ways, and made available through the ``main-app`` class:

    * Keyword arguments have a key and a value separated by a ``'='``.  They're merged into the global configuration, and they are also made available in a map-like structure, ``main-app.nonoption_kw_args()``.  The syntax options are:
 
        *    ``key=`` (NULL value)
        *    ``key={true|false}`` (boolean value)
        *    ``key=string`` (string value)
        *    ``key=[numeric]`` (numeric value, either integer, unsigned integer, or float; scientific notation, e.g. ``1e7``, is allowed for floats)

      The key can be a simple string, which indicates a key/value pair in a flat map, or it can indicate a value within a nested hierarchical structure of maps and arrays.  The only requirement on the structure is that the top level is a map with a string key.  For example, the keyword argument ``key_one.2.key_two=true`` would indicate the following structure (using YAML notation for convenience)::

        ---
        key_one:
            - NULL
            - NULL
            - key_two: true

    * Ordered arguments have a value (NULL, boolean, string, or numeric) but no key.  They are not merged with the global configuration, and are made available in an array-like structure: ``main_app::nonoption_ord_args()``.

Creating an Application
-----------------------

The simplest example
####################

::

    #include "application.hh"

    using namespace scarab;

    int main( int argc, char **argv )
    {
        main_app the_main;

        CLI11_PARSE( the_main, argc, argv );

        return 0;
    }

Example with a callback
#######################

This example captures the behavior of the application in a class, and then runs the ``execute()`` function with a callback.

::

    #include "application.hh"
    #include "logger.hh"

    LOGGER( testlog, "test_app_with_callback" );

    namespace scarab
    {
        struct do_a_thing
        {
            do_a_thing() : f_value( 5 ) {}

            void execute( const main_app& an_app )
            {
                // configure to run
                f_value = an_app.master_config().get_value( "value", f_value );

                // do a thing!
                LPROG( testlog, "My value is: " << f_value );

                return;
            }

            int f_value;
        };
    }

    using namespace scarab;

    int main( int argc, char **argv )
    {
        main_app the_main;

        auto t_dat_callback = [&](){
            do_a_thing t_dat;
            t_dat.execute( the_main );
        };

        the_main.callback( t_dat_callback );

        CLI11_PARSE( the_main, argc, argv );

        return 0;
    }
