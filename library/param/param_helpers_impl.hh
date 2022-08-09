/*
 * param_helpers_impl.hh
 *
 *  Created on: Aug 8, 2022
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_HELPERS_IMPL_HH_
#define SCARAB_PARAM_HELPERS_IMPL_HH_

#include "param_helpers.hh"

#include "param_array.hh"
#include "param_node.hh"
#include "param_value.hh"

namespace scarab
{
    template< typename T, typename... Ts >
    pa_args::pa_args( T&& a_value, Ts&&... a_values ) :
            f_array_ptr( new param_array() )
    {
        static_cast< param_array* >(f_array_ptr.get())->push_back( std::forward<T>(a_value), std::forward<Ts>(a_values)... );
    }

    template< typename T >
    named_arg_v named_arg::operator=( T&& a_value )
    {
        return { std::move(*this), std::forward<T>(a_value) };
    }

    template< typename T >
    named_arg_v::named_arg_v( named_arg&& a_base, T&& a_value ) :
            named_arg( std::move(a_base) ),
            f_value()
    {
        f_derived = this;

        using T_noref = typename std::remove_reference< T >::type;
        static_assert( std::is_same_v< param_ptr_t, T_noref > || 
                       std::is_base_of_v< param, T_noref > || 
                       std::is_convertible_v< T_noref, param_value > );

        if constexpr ( std::is_same_v< param_ptr_t, T_noref > )
        {
            f_value = std::move(a_value);
        }
        if constexpr ( std::is_base_of_v< param, T_noref > )
        {
            if constexpr ( std::is_lvalue_reference_v< T > ) // then do a copy
            {
                f_value = a_value.clone();
            }
            else // then do a move
            {
                f_value = a_value.move_clone();
            }
        }
        else if constexpr ( std::is_convertible_v< T_noref, param_value > )
        {
            f_value = param_ptr_t( new param_value( a_value ) );
        }
    }

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
}

#endif /* SCARAB_PARAM_HELPERS_IMPL_HH_ */
