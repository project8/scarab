/*
 * path.cc
 *
 *  Created on: Jan 22, 2016
 *      Author: nsoblath
 */

#define SCARAB_API_EXPORTS

#include "path.hh"

//#include <iostream>
#include <regex>

using std::string;
using boost::filesystem::absolute;

namespace scarab
{
    path SCARAB_API expand_path( const string& a_path )
    {
        string t_exp_path( a_path );
#ifndef _WIN32
        // POSIX-shell-like expansion
        wordexp_t t_expansion_result;
        wordexp( a_path.c_str(), &t_expansion_result, 0 );
        if( t_expansion_result.we_wordc > 0 )
        {
            t_exp_path = string( t_expansion_result.we_wordv[0] );
            //std::cout << "#### expanded path: " << t_exp_path << std::endl;
        }
        wordfree( &t_expansion_result );
#endif
        // do canonical expansion; return the absolute path
        return absolute( t_exp_path );
    }

    std::vector<std::string> glob(const std::string& a_path, const std::string& a_pattern)
    {
        std::vector< std::string > t_file_paths;
        path t_abspath = expand_path(a_path);
        for(fs::directory_entry& dir_entry: fs::recursive_directory_iterator(t_abspath))
        {
            std::string t_file_path=dir_entry.path().string();
            if (!std::regex_search(t_file_path, std::regex(a_pattern))) continue;
            t_file_paths.push_back(t_file_path);
        }
        return t_file_paths;
    }
}
