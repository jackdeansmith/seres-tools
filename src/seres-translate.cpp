#include "sequence.hpp"
#include "walk.hpp"
#include "resample.hpp"

#include <getopt.h>

#include <iostream>
using std::cout; using std::cerr; using std::cin; using std::endl;
using std::istream;
#include <fstream>
using std::ifstream;
#include <string>
using std::string; using std::getline;
#include <vector>
using std::vector;

string usage = 
"USAGE:\n"
"  seres-translate [OPTIONS] <walk file>\n\n"
"FLAGS:\n"
"  -h, --help              Display this message.\n"
"  -p, --positon           Translate positions. (default)\n"
"  -b, --breakpoint        Translate breakpoints.\n\n"
"OPTIONS:\n"
"  -f, --file <file>       Accept input locations from a file rather than stdin.\n"
"  -s, --sep <separator>   Separator for input locations, default is ','\n\n"
"ARGS:\n"
"  <walk file>             A file produced by seres-resample which specifies \n"
"                          how the resampler walked through the alignment.\n"

;

//Helper function which accepts an istream and a separator and parses the stream
//to a vector of locations.
vector<size_t> ReadStream(istream& stream, char separator){
    vector<size_t> result;

    string elem;
    while(getline(stream, elem, separator)){
       if(elem.length() == 0){
           break; 
       }
       result.push_back(stoul(elem));
    }

    return result;
}

int main(int argc, char* argv[]){

    //Define the options for GNU getopt_long
    char c;
    extern char* optarg;
    extern int optind;
    const char* const shortopts = "hpb";
    const option longopts[] = {
        {"help", no_argument, nullptr, 'h'},
        {"position", no_argument, nullptr, 'p'},
        {"breakpoint", no_argument, nullptr, 'b'},
        {"file", required_argument, nullptr, 'f'},
        {"sep", required_argument, nullptr, 's'},
        {nullptr, 0, nullptr, 0}
    };

    //Flags
    bool pflag = false;
    bool bflag = false;

    //Options
    bool fflag = false;
    string farg;
    bool sflag = false;
    string sarg;

    //Run getopt long to parse and grab these
    while((c = getopt_long(argc, argv, shortopts, longopts, nullptr)) != -1){
        switch(c){
            case 'p':
                pflag = true;
                break;
            case 'b':
                bflag = true;
                break;
            case 'f':
                fflag = true;
                farg.assign(optarg);
                break;
            case 's':
                sflag = true;
                sarg.assign(optarg);
                break;
            case 'h':
                cerr << usage << endl;
                exit(0);
                break;
            case '?':
                cerr << usage << endl;
                exit(1);
                break;
        }
    }

    //Calculate the number of positional args, if there are more or less than 1,
    //then we can't continue.
    int num_positional_args = argc - optind;
    if(num_positional_args != 1){
        cerr << "Error! Exactly one positional arg must be supplied but " 
             << num_positional_args << " were found." << endl << endl;
        cerr << usage << endl;
        exit(1);
    }

    //Next, we should open the input file the user provided, if we can't then
    //warn the user and exit.
    ifstream input_walk_file(argv[optind]);
    if(!input_walk_file.is_open()){
        cerr << "Error! The input walk file \"" << argv[optind]
             << "\" could not be opened." << endl;
        cerr << "Check that it exists and you have permission to open it." << endl;
        cerr << endl;
        cerr << usage << endl;
        exit(1);
    }

    //Read a key from it
    RandomWalk walk;
    input_walk_file >> walk;

    //Next, make sure the b and p flag are set appropriatly
    if(bflag && pflag){
        cerr << "Error, the -b and -p flags are mutually exclusive." << endl; 
        cerr << endl << usage << endl;
        exit(1);
    }
    if(!pflag && !bflag){
        pflag = true; 
    }

    //Figure out what the separator character should be
    char sep = ',';
    if(sflag && sarg.length() == 1){
        sep = sarg[0]; 
    }

    //Fill this vector from whatever sounce we are given
    vector<size_t> locations;

    //Grab the locations from the input
    if(fflag){
        ifstream ifs(farg);
        if(!ifs.is_open()){
            cerr << "Error! Could not open the file \"" << farg << "\"" << endl;
            cerr << "Check to make sure it exists and you have permisstions."
                 << endl << endl;
            cerr << usage << endl;
            exit(1);
        }
        locations =  ReadStream(ifs, sep);
    }
    else{
        locations =  ReadStream(cin, sep);
    }

    //Translate all the locations
    for(auto iter = locations.begin(); iter != locations.end(); iter++){
        if(pflag){
            cout << walk.lookup_position(*iter);
        }
        else{
            cout << walk.lookup_breakpoint(*iter);
        }
        if(iter != locations.end() -1){
            cout << ", ";
        }
    }
    cout << endl;
}
