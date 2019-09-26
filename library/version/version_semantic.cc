/*
 * version_semantic.cc
 *
 *  Created on: Jul 23, 2018
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "version_semantic.hh"

#include "logger.hh"
#include "path.hh"

#include <sstream>

#ifdef _WIN32
#include <Windows.h> // for extracting the exe name, gethostname and GetUserName
#elif __APPLE__
#include <mach-o/dyld.h> // for extracting the exe name
#include <unistd.h> // for gethostname and getlogin_r
#include <pwd.h> // for struct passwd
#elif __linux
#include <unistd.h> // for readlink, gethostname and getlogin_r
#include <pwd.h> // for struct passwd
#endif

using std::string;

namespace scarab
{
    LOGGER( slog, "version_semantic" );

    char version_semantic::s_delimeter = '.';

    version_semantic::version_semantic() :
            version_ifc(),
            f_major_ver(),
            f_minor_ver(),
            f_patch_ver(),
            f_version(),
            f_package(),
            f_commit(),
            f_exe_name(),
            f_hostname(),
            f_username()
    {
        update_exe_host_user();
    }

    version_semantic::version_semantic( unsigned a_maj_ver, unsigned a_min_ver, unsigned a_patch_ver ) :
            version_ifc(),
            f_major_ver( a_maj_ver ),
            f_minor_ver( a_min_ver ),
            f_patch_ver( a_patch_ver ),
            f_version(),
            f_package(),
            f_commit(),
            f_exe_name(),
            f_hostname(),
            f_username()
    {
        combine( a_maj_ver, a_min_ver, a_patch_ver );
        update_exe_host_user();
    }

    version_semantic::version_semantic( const std::string& a_ver ) :
            version_ifc(),
            f_major_ver( 0 ),
            f_minor_ver( 0 ),
            f_patch_ver( 0 ),
            f_version( a_ver ),
            f_package(),
            f_commit(),
            f_exe_name(),
            f_hostname(),
            f_username()
    {
        parse( a_ver );
        update_exe_host_user();
    }

    version_semantic::version_semantic( const version_semantic& a_orig ) :
            version_ifc(),
            f_major_ver( a_orig.f_major_ver ),
            f_minor_ver( a_orig.f_minor_ver ),
            f_patch_ver( a_orig.f_patch_ver ),
            f_version( a_orig.f_version ),
            f_package( a_orig.f_package ),
            f_commit( a_orig.f_commit ),
            f_exe_name( a_orig.f_exe_name ),
            f_hostname( a_orig.f_hostname ),
            f_username( a_orig.f_username )
    {
    }

    version_semantic::~version_semantic()
    {
    }

    version_semantic& version_semantic::operator=( const version_semantic& a_orig )
    {
        this->version_ifc::operator=( a_orig );
        f_major_ver = a_orig.f_major_ver;
        f_minor_ver = a_orig.f_minor_ver;
        f_patch_ver = a_orig.f_patch_ver;
        f_version = a_orig.f_version;
        f_package = a_orig.f_package;
        f_commit = a_orig.f_commit;
        f_exe_name = a_orig.f_exe_name;
        f_hostname = a_orig.f_hostname;
        f_username = a_orig.f_username;
        return *this;
    }

    bool version_semantic::operator<( const version_semantic& a_other )
    {
        if( f_major_ver < a_other.f_major_ver ) return true;
        if( f_major_ver > a_other.f_major_ver ) return false;
        if( f_minor_ver < a_other.f_minor_ver ) return true;
        if( f_minor_ver > a_other.f_minor_ver ) return false;
        if( f_patch_ver < a_other.f_patch_ver ) return true;
        return false;
    }

    bool version_semantic::operator==( const version_semantic& a_other )
    {
        return f_major_ver == a_other.f_major_ver &&
               f_minor_ver == a_other.f_minor_ver &&
               f_patch_ver == a_other.f_patch_ver;
    }

    bool version_semantic::operator>( const version_semantic& a_other )
    {
        if( f_major_ver > a_other.f_major_ver ) return true;
        if( f_major_ver < a_other.f_major_ver ) return false;
        if( f_minor_ver > a_other.f_minor_ver ) return true;
        if( f_minor_ver < a_other.f_minor_ver ) return false;
        if( f_patch_ver > a_other.f_patch_ver ) return true;
        return false;
    }

    bool version_semantic::parse( const std::string& a_ver )
    {
        if( a_ver.empty() || a_ver == "unknown" )
        {
            f_major_ver = 0;
            f_minor_ver = 0;
            f_patch_ver = 0;
            f_version = a_ver;
            return true;
        }

        size_t t_start = 0;
        if( a_ver[0] == 'v' )
        {
            t_start = 1;
        }
        size_t t_delim_pos_1 = a_ver.find( s_delimeter, 0 );
        if( t_delim_pos_1 == std::string::npos )
        {
            LERROR( slog, "version string <" << a_ver << "> is not in the right format (did not find first delimeter)" );
            return false;
        }
        std::stringstream t_maj_ver_str;
        t_maj_ver_str << a_ver.substr( t_start, t_delim_pos_1 );

        size_t t_delim_pos_2 = a_ver.find( s_delimeter, t_delim_pos_1 + 1 );
        if( t_delim_pos_2 == std::string::npos )
        {
            LERROR( slog, "version string <" << a_ver << "> is not in the right format (did not find second delimeter)" );
            return false;
        }
        std::stringstream t_min_ver_str;
        t_min_ver_str << a_ver.substr(t_delim_pos_1 + 1, t_delim_pos_2 );

        std::stringstream t_patch_ver;
        t_patch_ver << a_ver.substr( t_delim_pos_2 + 1 );

        t_maj_ver_str >> f_major_ver;
        t_min_ver_str >> f_minor_ver;
        t_patch_ver >> f_patch_ver;
        f_version = a_ver;

        return true;
    }

    bool version_semantic::combine( unsigned a_maj_ver, unsigned a_min_ver, unsigned a_patch_ver )
    {
        std::stringstream t_ver_str;
        t_ver_str << a_maj_ver << s_delimeter << a_min_ver << s_delimeter << a_patch_ver;
        f_version = t_ver_str.str();
        return true;
    }

    bool version_semantic::update_exe_host_user()
    {
        const size_t t_bufsize = 1024;

        // Username
#ifdef _WIN32
        char t_username_buf[ t_bufsize ];
        DWORD t_bufsize_win = t_bufsize;
        if( GetUserName( t_username_buf, &t_bufsize_win ) )
        {
            f_username = string( t_username_buf );
        }
        else
        {
#else
        //if( getlogin_r( t_username_buf, t_bufsize ) == 0 )
        passwd* t_passwd = getpwuid( getuid() );
        if( t_passwd != nullptr )
        {
            f_username = string( t_passwd->pw_name );
        }
        else
        {
            LWARN( slog, "Error reported while getting passwd info: " << strerror( errno ) );
#endif
            LWARN( slog, "Unable to get the username" );
        }

        // Hostname
        char t_hostname_buf[ t_bufsize ];
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
#endif
        // gethostname is the same on posix and windows
        if( gethostname( t_hostname_buf, t_bufsize ) == 0 )
        {
            f_hostname = string( t_hostname_buf );
        }
        else
        {
            LWARN( slog, "Unable to get the hostname" );
        }
#ifdef _WIN32
        WSACleanup();
#endif

        // name of executable
        //f_exe_name = t_parser.get_value( t_name_exe, f_exe_name );
#ifdef _WIN32
        TCHAR t_exe_buf[ MAX_PATH ];
        if( ! GetModuleFileName( NULL, t_exe_buf, MAX_PATH ) )
#elif __APPLE__
        char t_exe_buf[ 2048 ];
        uint32_t t_exe_bufsize = sizeof( t_exe_buf );
        if( _NSGetExecutablePath( t_exe_buf, &t_exe_bufsize ) != 0 )
#elif __linux
        const size_t t_exe_bufsize = 2048;
        char t_exe_buf[ t_exe_bufsize ];
        ssize_t t_exe_name_len = readlink( "/proc/self/exe", t_exe_buf, t_exe_bufsize );
        if( t_exe_name_len >= 0 )
        {
            t_exe_buf[t_exe_name_len] = '\0';
        }
        else
        //if( readlink( "/proc/self/exe", t_exe_buf, t_exe_bufsize ) < 0 )
#endif
        {
            LWARN( slog, "Could not retrieve executable file name" );
#ifdef __APPLE__
            LWARN( slog, "Executable name buffer is too small; needs size %u\n" << t_bufsize );
#endif
        }
        f_exe_name = string( t_exe_buf );

        return true;
    }

    std::string version_semantic::version_info_string() const
    {
        std::stringstream t_info_stream;
        path t_exe_path( f_exe_name );
        t_info_stream << "Executable: " << t_exe_path.filename() << '\n';
        t_info_stream << "Location: " << t_exe_path.parent_path() << '\n';
        t_info_stream << "Built with " << f_package << " version " << f_version << '\n';
        t_info_stream << "Git commit: " << f_commit;
        std::string t_version_info( t_info_stream.str() );
        return t_version_info;
    }

} /* namespace scarab */
