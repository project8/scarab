# Find the FFTW includes and library.
# 
# The default and primary FFTW installation uses double-precision floating-point values.
# FFTW can provide single- and long-double-precision versions as well.
# 
# The different versions of FFTW can be found using the COMPONENTS argument of find_package().
# The available components are:
#   DOUBLE
#     NOTE: This is the default.  If you don't specify any COMPONENTS, this is what you'll get.
#     include: fftw3.h
#     lib: fftw3
#     threads lib: fftw3_threads
#   SINGLE
#     include: fftw3.h
#     lib: fftw3f
#     threads lib: fftw3f_threads
#   LONGDOUBLE
#     include: fftw3.h
#     lib: fftw3l
#     threads lib: fftw3l_threads
#
# This module defines
# FFTW_FOUND.  If true, you can use the FFTW routines
# FFTW_INCLUDE_DIR, where to locate fftw3.h file
# FFTW_LIBRARIES, the libraries to link against to use fftw3
# FFTW_LIBRARY_DIRS, the directories containing the FFTW libraries
# FFTW_THREADS_FOUND, If true, can use multithreaded FFTW routines
# FFTW_THREADS_LIBRARIES, where to find the FFTW threads libraries
#

# The default component is DOUBLE
# If the components list is empty, put "DOUBLE" in
# Otherwise just use the components specified
if( NOT FFTW_FIND_COMPONENTS )
    list( APPEND FFTW_FIND_COMPONENTS "DOUBLE" )
endif( NOT FFTW_FIND_COMPONENTS )
message( STATUS "Will look for these FFTW components ${FFTW_FIND_COMPONENTS}" )

set( FFTW_FOUND TRUE )
set( FFTW_THREADS_FOUND TRUE )

# The include file is the same for all FFTW components
find_path( FFTW_INCLUDE_DIR fftw3.h
    $ENV{FFTW_DIR}/include
    $ENV{FFTW3} $ENV{FFTW3}/include $ENV{FFTW3}/api
    /usr/local/include
    /usr/include
    /opt/fftw3/include
    DOC "Find the directory containing fftw3.h"
)

if( NOT FFTW_INCLUDE_DIR )
    message( WARNING "Did not find fftw3.h" )
    set( FFTW_FOUND FALSE )
endif( NOT FFTW_INCLUDE_DIR )

set( FFTW_LIBRARY_DIRS )
set( FFTW_LIBRARY )
set( FFTW_LIBRARIES )

# Loop over the requested components
foreach( COMPONENT ${FFTW_FIND_COMPONENTS} )
    string( TOUPPER ${COMPONENT} UPPERCOMPONENT )

    if( COMPONENT STREQUAL "DOUBLE" )
        message( STATUS "Looking for the double-precision FFTW library" )
        set( LIB_NAMES fftw3 fftw3-3 )
        set( THREAD_LIB_NAMES fftw3_threads fftw3-3_threads )
    elseif( COMPONENT STREQUAL "SINGLE" )
        message( STATUS "Looking for the single-precision FFTW library" )
        set( LIB_NAMES fftw3f fftw3-3f )
        set( THREAD_LIB_NAMES fftw3f_threads fftw3-3f_threads )
    elseif( COMPONENT STREQUAL "LONGDOUBLE" )
        message( STATUS "Looking for the long-double-precision FFTW library" )
        set( LIB_NAMES fftw3l fftw3-3l )
        set( THREAD_LIB_NAMES fftw3l_threads fftw3-3l_threads )
    else()
        message( WARNING "Invalid COMPONENT <${COMPONENT}> will be ignored" )
        unset( LIB_NAMES )
        unset( THREAD_LIB_NAMES )
    endif()

    if( LIB_NAMES AND THREAD_LIB_NAMES )

        # FFTW library
        unset( COMP_LIBRARY CACHE )
        find_library( COMP_LIBRARY NAMES ${LIB_NAMES} PATHS
            ${FFTW_LIBRARY_DIRS}
            $ENV{FFTW_DIR}/lib
            $ENV{FFTW3} $ENV{FFTW3}/lib $ENV{FFTW3}/.libs
            /usr/local/lib
            /usr/lib 
            /opt/local/lib
            /opt/fftw3/lib
            DOC "Find the fftw3 library"
        )
        
        if( COMP_LIBRARY )
            list( APPEND FFTW_LIBRARIES ${COMP_LIBRARY} )
            get_filename_component( LIBRARY_DIR ${COMP_LIBRARY} DIRECTORY )
            list( APPEND FFTW_LIBRARY_DIRS ${LIBRARY_DIR} )
        else( COMP_LIBRARY )
            message( WARNING "Did not find the FFTW library" )
            set( FFTW_FOUND FALSE )
        endif( COMP_LIBRARY )
            
        # Threads library
        unset( COMP_THREADS_LIBRARY CACHE )
        find_library( COMP_THREADS_LIBRARY NAMES ${THREAD_LIB_NAMES} PATHS
            ${FFTW_LIBRARY_DIRS}
            $ENV{FFTW_DIR}/lib
            $ENV{FFTW3} $ENV{FFTW3}/lib $ENV{FFTW3}/.libs
            /usr/local/lib
            /usr/lib 
            /opt/local/lib
            /opt/fftw3/lib
            DOC "Specify the fftw3 threads library here."
        )
    
        if( COMP_THREADS_LIBRARY )
            list( APPEND FFTW_THREADS_LIBRARIES ${COMP_THREADS_LIBRARY} )
            get_filename_component( LIBRARY_DIR ${COMP_THREADS_LIBRARY} DIRECTORY )
            list( APPEND FFTW_LIBRARY_DIRS ${LIBRARY_DIR} )
        else( COMP_THREADS_LIBRARY )
            message( WARNING "Did not find the FFTW threads library" )
            set( FFTW_THREADS_FOUND FALSE )
        endif( COMP_THREADS_LIBRARY )
    
        # get a thread library if necessary (not necessary for C++11 if using std::threads)
        if( FFTW_THREADS_FOUND )
            find_package( Threads )
            # since a library is not necessary under C++11, sometimes there won't be one here
            if( CMAKE_THREAD_LIBS_INIT )
                list( APPEND FFTW_THREADS_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )
                get_filename_component( LIBRARY_DIR ${CMAKE_THREAD_LIBS_INIT} DIRECTORY )
                list( APPEND FFTW_LIBRARY_DIRS ${LIBRARY_DIR} )
            endif( CMAKE_THREAD_LIBS_INIT )
        endif( FFTW_THREADS_FOUND )
         
    endif( LIB_NAMES AND THREAD_LIB_NAMES )   
endforeach( COMPONENT ${FFTW_FIND_COMPONENTS} )

list( REMOVE_DUPLICATES FFTW_LIBRARY_DIRS )

# Wrap up
if( FFTW_FOUND )
  if( NOT FFTW_FIND_QUIETLY )
     message( STATUS "Found fftw3 includes at ${FFTW_INCLUDE_DIR}" )
     message( STATUS "Found fftw3 libraries: ${FFTW_LIBRARIES}" )
     message( STATUS "Found fftw3 libraries at ${FFTW_LIBRARY_DIRS}" )
  endif()
endif( FFTW_FOUND )
  
if( FFTW_THREADS_FOUND )
    if( NOT FFTW_FIND_QUIETLY )
        message( STATUS "Found fftw3 threads and threads libraries at ${FFTW_THREADS_LIBRARIES}" )
    endif( NOT FFTW_FIND_QUIETLY )
endif( FFTW_THREADS_FOUND )

mark_as_advanced(FFTW_FOUND FFTW_LIBRARIES FFTW_INCLUDE_DIR FFTW_LIBRARY_DIRS FFTW_THREADS_FOUND)
if( FFTW_THREADS_FOUND )
  mark_as_advanced(FFTW_THREADS_LIBRARIES)
endif( FFTW_THREADS_FOUND )
