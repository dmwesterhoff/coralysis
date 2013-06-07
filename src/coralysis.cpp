/*  filename:   analyze.cpp
 *  author:     David M. Westerhoff
 *  version:    alpha
 *  last mod:   3/18/13
 *  descript:   driver program for the image analysis utility,
 *              purpose is to extract image data and format for
 *              use with R.
 */

#include "imgutil.h"
#include "formatter.h"
// opencv headers
#include <cv.h>
#include <highgui.h>
// c++ headers
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
// boost headers
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

using namespace cv;
using namespace std;
using namespace boost::filesystem;

path target_path;
path output_name = "output.txt";    // default output filename
vector<path> working_set;	// this vec holds rel. path of all workable .jpgs
bool recurse_flag = false;
bool read_config = false;
enum loglevels {
    SILENT,
    NORMAL,
    VERBOSE,
    DEBUG
};
int log_level = NORMAL;

// loads  all jpg's found in path to working_set(with or without recursion)
void search_path(path p, bool recurse) {	// path must be to a directory
    if (is_directory(p)) {
        if (recurse) {	// recurse through all directories in path
            for (recursive_directory_iterator end, iter(p); iter != end; ++iter) {
                path filepath = (*iter).path();
                if (boost::iequals(filepath.extension().string(),".JPG")) {
                    working_set.push_back(filepath);
                }
            }
        }
        else {
            for (directory_iterator end, iter(p); iter != end; ++iter) {
                path filepath = (*iter).path();
                if (boost::iequals(filepath.extension().string(),".JPG")) {
                    working_set.push_back(filepath);
                }
            }
        }
    }
}


int main( int argc, char* argv[] )
{
    // SUPPORTED OPTIONS
    boost::program_options::options_description descript("USAGE: ./analyze /path/to/images --[options]\n"
            "User may specify image path in any position on command line.\n"
            "Allowed options");
    descript.add_options()
		        ("help", "produce help message\n")
		        ("v", "print verbose runtime log to stdout\n")
		        ("s", "silence all normal logging activity during execution")
		        ("r", "recurse through directory and all sub-directories\n")
		        ("version", "print current software version\n")
		        ("c", "reads all options from conf.d file in current working directory\n")
		        ("w", boost::program_options::value<string>(), "specify output file name")
		        ("p", boost::program_options::value<string>(), "specify input path\n");
    // image directory to be worked on is only "positional option"
    boost::program_options::positional_options_description p;
    p.add("p", -1);

    // GRAB ALL COMMAND LINE ARGUMENTS
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(descript).positional(p).run(), vm);
    boost::program_options::notify(vm);

    // PARSE COMMAND LINE ARGUMENTS
    if (vm.count("help")) {		// print help screen if asked and exit
        cout << descript << endl;
        return 0;
    }
    if (vm.count("version")) {	// print current version info
        cout << "coralysis is still in alpha stage development." << endl;
        return 0;
    }
    if (vm.count("p")) { 	// set target path
        target_path = vm["p"].as<string>();	// get target directory path
    }
    else {	// target path must be on command line
        cerr << "please provide target path as command line argument, usage: ./analyze --help for more info" << endl;
        return 1;
    }
    if (vm.count("w")) {    // set output file name
        output_name = vm["w"].as<string>();
    }
    if (vm.count("v")) {
        log_level = VERBOSE;
    }
    if (vm.count("s")) {
        log_level = SILENT;
    }
    if (vm.count("c")) {	// change defaults to that of the conf.d file
        read_config = true;
    }
    if (vm.count("r")) {	// turn recursion on for driver program
        recurse_flag = true;
    }
    // END OPTIONS PARSE


    // DRIVER & PATH ITERATION
    try {
        search_path(target_path, recurse_flag);
        cout << "found [" << working_set.size() << "] workable jpg files." << endl;
        vector<path>::iterator iter = working_set.begin();
        imgutil iu(iter->string());
        formatter fm(iu, output_name.c_str());  // initialize formatter
        iter++;

        while(iter != working_set.end()) {  // print stats for rest of set
            imgutil iu(iter->string());
            fm.append(iu);
            iter++;
        }
        fm.close(); // close file stream
    }
    catch (const filesystem_error& ex) {
        cout << ex.what() << endl;
    }
    return 0;
}
