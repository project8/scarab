/*
 * scarab_api.hh
 *
 *  Created on: Jan 1, 2016
 *      Author: nsoblath
 */

#ifndef SCARAB_API_HH_
#define SCARAB_API_HH_


namespace scarab
{
    /**
     * SCARAB_API is used to modify the visibility of classes and functions for:
     *   - Windows DLLs
     *   - Pybind11
     * 
     * For Windows DLLs, the API needs to be exported to make portable DLLs.
     *  __delclspec() changes the behavior of the `SCARAB_API` macro depending on whether
     * it's being used in a header file (dllimport) or source file (dllexport).
     * 
     * For Pybind11, when using gcc or clang, if you want to be able to use the classes in another package 
     * (i.e. some other package will use the functions/classes you're binding), then you need to 
     * make them visible.  See https://pybind11.readthedocs.io/en/stable/advanced/misc.html#partitioning-code-over-multiple-extension-modules
     * for more information.  This macro is taking the place of `PYBIND11_EXPORT`.
    */
#ifdef _WIN32
#  ifdef SCARAB_API_EXPORTS
#    define SCARAB_API __declspec(dllexport)
#    define SCARAB_EXPIMP_TEMPLATE
#  else
#    define SCARAB_API __declspec(dllimport)
#    define SCARAB_EXPIMP_TEMPLATE extern
#  endif
#else
#  ifdef SCARAB_API_EXPORTS
#    define SCARAB_API
#  else
#    define SCARAB_API __attribute__((visibility("default")))
#  endif
#endif

}

#endif /* SCARAB_API_HH_ */
