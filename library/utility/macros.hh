/*
 * macros.hh
 *
 *  Created on: May 25, 2017
 *      Author: obla999
 */

#ifndef SCARAB_UTILITY_MACROS_HH_
#define SCARAB_UTILITY_MACROS_HH_

// Return value constants
#define RETURN_SUCCESS 0
#define RETURN_ERROR 1
#define RETURN_CANCELED 2
#define RETURN_REVOKED 3

#define PASTE2( x, y ) x##y
#define PASTE( x, y ) PASTE2( x, y )

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define __FILE_LINE__      __FILE__ "(" TOSTRING(__LINE__) ")"
#define __FILENAME_LINE__  (strrchr(__FILE__, '/') ? strrchr(__FILE_LINE__, '/') + 1 : __FILE_LINE__)

#if !defined(__FUNC__)
#if defined(_MSC_VER)
#define __FUNC__ __FUNCSIG__
#elif defined(__GNUC__)
#define __FUNC__ __PRETTY_FUNCTION__
#else
#define __FUNC__ ""
#endif
#endif

#endif /* SCARAB_UTILITY_MACROS_HH_ */
