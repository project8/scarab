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


    struct SCARAB_API pa_args
    {
        template< typename T, typename... Ts >
        explicit pa_args( T&& a_value, Ts&&... a_values );

        param_ptr_t f_array_ptr;
    };

    struct named_arg_v;
    struct SCARAB_API named_arg
    {
        /// Construct the argument with a name
        constexpr explicit named_arg( const char* a_name ) :
                f_name( a_name ),
                f_derived( nullptr )
        {}

        /// Assign a value to this argument
        template< typename T >
        named_arg_v operator=( T&& a_value );

        /// Access the name
        const char* name() const { return f_name; }
        /// Access the value
        const std::shared_ptr< param >& value() const;

        /// The argument name
        const char* f_name; // use const char* here so that constructor can be constexpr

        named_arg_v* f_derived;
    };

    struct SCARAB_API named_arg_v : named_arg
    {
        template< typename T >
        named_arg_v( named_arg&& a_base, T&& a_value );

        /// The argument value
        std::shared_ptr< param > f_value;
    };

    inline const std::shared_ptr< param >& named_arg::value() const
    {
        return f_derived->f_value;
    }

    /// Syntactic sugar to enable simple-looking keyword arguments
    constexpr named_arg operator""_a( const char *name, size_t ) { return named_arg(name); }

/*
    struct node_arg_v : node_arg
    {
        template <typename T>
        arg_v(const char *name, T &&x);
//            : arg_v(arg(name), std::forward<T>(x), descr) {}

        /// The argument value
        std::unique_ptr< param > value;

    }
*/
/* from pybind11/cast.h
    struct arg {
    /// Constructs an argument with the name of the argument; if null or omitted, this is a
    /// positional argument.
    constexpr explicit arg(const char *name = nullptr)
        : name(name), flag_noconvert(false), flag_none(true) {}
    /// Assign a value to this argument
    template <typename T>
    arg_v operator=(T &&value) const;
    /// Indicate that the type should not be converted in the type caster
    arg &noconvert(bool flag = true) {
        flag_noconvert = flag;
        return *this;
    }
    /// Indicates that the argument should/shouldn't allow None (e.g. for nullable pointer args)
    arg &none(bool flag = true) {
        flag_none = flag;
        return *this;
    }

    const char *name;        ///< If non-null, this is a named kwargs argument
    bool flag_noconvert : 1; ///< If set, do not allow conversion (requires a supporting type
                             ///< caster!)
    bool flag_none : 1;      ///< If set (the default), allow None to be passed to this argument
};

/// \ingroup annotations
/// Annotation for arguments with values
struct arg_v : arg {
private:
    template <typename T>
    arg_v(arg &&base, T &&x, const char *descr = nullptr)
        : arg(base), value(reinterpret_steal<object>(detail::make_caster<T>::cast(
                         std::forward<T>(x), return_value_policy::automatic, {}))),
          descr(descr)
#if defined(PYBIND11_DETAILED_ERROR_MESSAGES)
          ,
          type(type_id<T>())
#endif
    {
        // Workaround! See:
        // https://github.com/pybind/pybind11/issues/2336
        // https://github.com/pybind/pybind11/pull/2685#issuecomment-731286700
        if (PyErr_Occurred()) {
            PyErr_Clear();
        }
    }

public:
    /// Direct construction with name, default, and description
    template <typename T>
    arg_v(const char *name, T &&x, const char *descr = nullptr)
        : arg_v(arg(name), std::forward<T>(x), descr) {}

    /// Called internally when invoking `py::arg("a") = value`
    template <typename T>
    arg_v(const arg &base, T &&x, const char *descr = nullptr)
        : arg_v(arg(base), std::forward<T>(x), descr) {}

    /// Same as `arg::noconvert()`, but returns *this as arg_v&, not arg&
    arg_v &noconvert(bool flag = true) {
        arg::noconvert(flag);
        return *this;
    }

    /// Same as `arg::nonone()`, but returns *this as arg_v&, not arg&
    arg_v &none(bool flag = true) {
        arg::none(flag);
        return *this;
    }

    /// The default value
    object value;
    /// The (optional) description of the default value
    const char *descr;
#if defined(PYBIND11_DETAILED_ERROR_MESSAGES)
    /// The C++ type name of the default value (only available when compiled in debug mode)
    std::string type;
#endif
};

/// \ingroup annotations
/// Annotation indicating that all following arguments are keyword-only; the is the equivalent of
/// an unnamed '*' argument
struct kw_only {};

/// \ingroup annotations
/// Annotation indicating that all previous arguments are positional-only; the is the equivalent of
/// an unnamed '/' argument (in Python 3.8)
struct pos_only {};

template <typename T>
arg_v arg::operator=(T &&value) const {
    return {*this, std::forward<T>(value)};
}

*/

/*
inline namespace literals {
constexpr arg operator"" _a(const char *name, size_t) { return arg(name); }
} // namespace literals
*/
}

#endif /* SCARAB_PARAM_HELPERS_HH_ */
