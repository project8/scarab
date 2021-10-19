/*
 * test_path.cc
 *
 *  Created on: Oct 13, 2021
 *      Author: P.T. Surukuchi
 *
 */

#include "path.hh"

#include "catch.hpp"

#include <iostream>


using scarab::path;

TEST_CASE( "path", "[utility]" )
{
    SECTION( "glob" )
    {
        std::cout << "Glob Test" << std::endl;
        std::string dir_path="../testing/glob_path";
        
        // build a test vector of txt files to test against the output of glob function
        //this only works asssuming you are in a build directory
        std::vector<std::string > test_file_paths;
        test_file_paths.push_back(scarab::expand_path(dir_path).string()+"/sub1/sub1sub1/glob_sub1sub1.txt");
        test_file_paths.push_back(scarab::expand_path(dir_path).string()+"/sub1/sub1sub2/glob_sub1sub2.txt");
        test_file_paths.push_back(scarab::expand_path(dir_path).string()+"/sub2/sub2sub1/sub2sub1sub1/glob_sub2sub1sub1_1.txt");
        test_file_paths.push_back(scarab::expand_path(dir_path).string()+"/sub2/sub2sub1/sub2sub1sub1/glob_sub2sub1sub1_2.txt");
        test_file_paths.push_back(scarab::expand_path(dir_path).string()+"/sub2/sub2sub1/sub2sub1sub2/glob_sub2sub1sub2.txt");

        std::vector< std::string > file_paths=scarab::glob(dir_path,"[0-2].txt$");
        std::sort (file_paths.begin(),file_paths.end());// sort to make sure they are in order
        for(unsigned int i=0;i<file_paths.size();++i){
            REQUIRE(test_file_paths[i]==file_paths[i]);
        }
    }
}

