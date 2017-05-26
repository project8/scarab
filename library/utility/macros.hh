/*
 * macros.hh
 *
 *  Created on: May 25, 2017
 *      Author: obla999
 */

#ifndef SCARAB_UTILITY_MACROS_HH_
#define SCARAB_UTILITY_MACROS_HH_


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define __FILE_LINE__      __FILE__ "(" TOSTRING(__LINE__) ")"
#define __FILENAME_LINE__  (strrchr(__FILE__, '/') ? strrchr(__FILE_LINE__, '/') + 1 : __FILE_LINE__)

#if defined(_MSC_VER)
#if _MSC_VER >= 1300
#define __FUNC__ __FUNCSIG__
#endif
#else
#if defined(__GNUC__)
#define __FUNC__ __PRETTY_FUNCTION__
#endif
#endif
#if !defined(__FUNC__)
#define __FUNC__ ""
#endif


#endif /* SCARAB_UTILITY_MACROS_HH_ */
