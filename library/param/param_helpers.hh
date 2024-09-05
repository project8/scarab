/*
 * param_helpers.hh
 *
 *  Created on: Jan 10, 2019
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_HELPERS_HH_
#define SCARAB_PARAM_HELPERS_HH_

#include "param_fwd.hh"

#include "scarab_api.hh"

namespace scarab
{
    /*!
     @struct simple_parser
     @author N. S. Oblath

     @brief Parses a string that references a location in a param structure

     @details
    */
    struct SCARAB_API simple_parser
    {
        public:
            /// Converts an address into a nested param structure, and optionally attaches a value
            static param_ptr_t parse_address( const std::string& an_addr, param_ptr_t a_value = param_ptr_t() );

            /// Attempts to determine whether a value encoded as a string is an int, and unsigned int, or a floating-point number
            static param_ptr_t parse_value( const std::string& a_value );

        private:
            static void add_next( param& a_parent, const std::string& an_addr_in_parent, const std::string& a_next_addr, param_ptr_t a_value = param_ptr_t() );

            static param_ptr_t new_param_from_addr( const std::string& an_addr );

        public:
            static const char f_node_separator = '.';
    };

    // "args" and "kwarg" are named after the similar concepts in Python

    // Notes on the args setup:
    // * args are intended for one use: temporarily holding argument values when calling certain param_array functions.
    // * From an API design perspective, they're only truly required for the param_array constructors:
    //   In principle one would like to be able to initialize a param_array with a sequence of values passed directly to the constructor.
    //   However, consider the case where the constructor arguments consist of just a single array, e.g.
    //     param_array new_array(existing_array);  // let's say existing_array == [1, 2, 3]
    //   In this case you have an ambiguous situation:
    //   - The argument could be intended to be the first element in the created array;
    //     So the resulting array is new_array == [ [1, 2, 3] ]; or
    //   - The argument could be intended to be copied from, with its contents copied as the contents of the created array;
    //     So the resulting array is new_array == [1, 2, 3]
    // * To get around this potential ambiguity, we introduce a new class that designates when the arguments are intended to be array elements themselves.
    struct SCARAB_API args
    {
        template< typename T, typename... Ts >
        explicit args( T&& a_value, Ts&&... a_values );

        param_ptr_t f_array_ptr;
    };

    // Notes on the kwargs setup:
    // * kwargs are intended for one use: temporarily holding argument names and values when calling certain param_node functions.
    // * The technique used for the kwargs was taken from Pybind11's implementation of keyword arguments.
    //   See the implementation here: https://github.com/pybind/pybind11/blob/59f03ee389c283cde65bd800c8f32ea690daf3fd/include/pybind11/cast.h
    // * I wanted to be able to create the kwarg base object as a constexpr for efficiency.
    //   To do this, however, the object created is required to be a literal type, which means its members have to be literal types.
    //   That's the reason for the use of the derived class, kwarg_v.  The shared_ptr is not a literal type, and therefore any object containing it is not a literal type.
    //   So the kwarg is created as a constexpr at compile time, and then replaced by the non-constexpr at runtime.
    // * The string literal _a is just a syntactic addition to the kwarg setup.
    //   It was taken from the same Pybind11 reference as above.
    // * As currently setup, the only way to correctly create a kwarg is to use this construction:
    //     kwarg("a_name")=[whatever value]
    //   or with _a:
    //     "a_name"_a=[whatever value]
    //   This is because a kwarg base object is not complete without the f_derived pointer,
    //   and you only get the derived object correctly setup using kwarg::operator=().
    struct kwarg_v;
    struct SCARAB_API kwarg
    {
        /// Construct the argument with a name
        constexpr explicit kwarg( const char* a_name ) :
                f_name( a_name ),
                f_derived( nullptr )
        {}

        /// Assign a value to this argument
        template< typename T >
        kwarg_v operator=( T&& a_value );

        /// Access the name
        const char* name() const { return f_name; }
        /// Access the value
        const std::shared_ptr< param >& value() const;

        /// The argument name
        const char* f_name; // use const char* here so that constructor can be constexpr

        kwarg_v* f_derived;
    };

    struct SCARAB_API kwarg_v : kwarg
    {
        template< typename T >
        kwarg_v( kwarg&& a_base, T&& a_value );

        /// The argument value
        std::shared_ptr< param > f_value;
    };

    inline const std::shared_ptr< param >& kwarg::value() const
    {
        return f_derived->f_value;
    }

    /// Syntactic sugar to enable simple-looking keyword arguments
    constexpr kwarg operator""_a( const char *name, size_t ) { return kwarg(name); }

    /// Convenience macro to bring args, kwargs, and _a into the current namespace
    #define using_param_args_and_kwargs \
        using scarab::args; \
        using scarab::kwarg; \
        using scarab::operator""_a;
}

#endif /* SCARAB_PARAM_HELPERS_HH_ */
