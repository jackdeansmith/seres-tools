#include "sequence.hpp"
#include "walk.hpp"
#include "resample.hpp"

#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include <iostream>
using std::cout; using std::cerr; using std::endl;
#include <fstream>
using std::ifstream; using std::ofstream;
#include <vector>
using std::vector;
#include <string>
using std::string; using std::to_string; 
#include <random>
using std::mt19937_64;

string usage =
"USAGE:\n"
"  seres-resample [OPTIONS] <input alignment>\n\n"
"Options:\n"
"  -h, --help               Print out this message. \n"
"  -b, --bias <bias>        The probability of the resampler reversing direction\n"
"                           at each site. Must be in (0..1]. Default is 0.01 \n"
"  -l, --length <length>    The length of each resampled replicate. Default is\n"
"                           the length of the input alignment.\n"
"  -n, --number <num>       How many resampled replicates to produce.\n"
"                           Default is 1.\n"
"  -d, --dir <output dir>   The directory the output replicates should be put in\n"
"                           Defaults to the current working directory.\n"
"  -s, --seed <rng-seed>    The seed for the PRNG (mt19937_64). \n"
"                           Defaults to time in miliseconds since epoch.\n"
"ARGS:\n"
"  <input alignment>        A FASTA formatted multiple sequence alignment file.\n"
;

//A function which is called by main, performs all the actual resampling after
//the input is parsed and validated.
void SERESResample(size_t number, size_t length, double bias, mt19937_64& rng,
                   const CharMatrix& input_sequence, const vector<string>& taxa){

    //We do number individual resampling runs starting from 1
    for(size_t trial_num=1; trial_num<=number; trial_num++){

        //Generate a new random walk using our parameters
        RandomWalk walk = 
            GenerateRandomWalk(input_sequence.length(), length, bias, rng);

        //Resample using our new random walk
        CharMatrix replicate_matrix = Resample(input_sequence, walk); 

        //Open the output files
        string walk_file_string = "replicate-" + to_string(trial_num) + ".walk";
        string rep_file_string  = "replicate-" + to_string(trial_num) + ".fasta";
        ofstream walk_file(walk_file_string);
        ofstream rep_file(rep_file_string);

        //Write the replicate alignment
        WriteFASTA(rep_file, replicate_matrix, taxa);

        //Write the walk
        walk_file << walk << endl;
    }
}

//Main function, primarily parses args
int main(int argc, char* argv[]){

    //Define the options for GNU getopt_long
    char c;
    extern char* optarg;
    extern int optind;
    const char* const shortopts = "hb:l:n:d:s:";
    const option longopts[] = {
        {"help", no_argument, nullptr, 'h'},
        {"bias", required_argument, nullptr, 'b'},
        {"length", required_argument, nullptr, 'l'},
        {"number", required_argument, nullptr, 'n'},
        {"dir", required_argument, nullptr, 'd'},
        {"seed", required_argument, nullptr, 's'},
        {nullptr, 0, nullptr, 0}
    };

    //These flags will get set to true and the args will get set when they are
    //supplied on the CLI.
    bool bflag = false;
    string barg;
    bool lflag = false;
    string larg;
    bool nflag = false;
    string narg;
    bool dflag = false;
    string darg;
    bool sflag = false;
    string sarg;

    //Run getopt long to parse and grab these
    while((c = getopt_long(argc, argv, shortopts, longopts, nullptr)) != -1){
        switch(c){
            case 'b':
                bflag = true;
                barg.assign(optarg);
                break;
            case 'l':
                lflag = true;
                larg.assign(optarg);
                break;
            case 'n':
                nflag = true;
                narg.assign(optarg);
                break;
            case 'd':
                dflag = true;
                darg.assign(optarg);
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
    ifstream input_alignment_file(argv[optind]);
    if(!input_alignment_file.is_open()){
        cerr << "Error! The input alignment file \"" << argv[optind]
             << "\" could not be opened." << endl;
        cerr << "Check that it exists and you have permission to open it." << endl;
        cerr << endl;
        cerr << usage << endl;
        exit(1);
    }

    //Nice, now we need to parse the input alignment file into a char matrix and
    //a vector of taxa.
    CharMatrix input_sequences;
    vector<string> input_taxa;
    try{
        ReadFASTA(input_alignment_file, input_sequences, input_taxa);
    }
    catch (std::runtime_error e){
        cerr << "The provided fasta file \"" << argv[optind] << "\""
                " is not an alignment." << endl;
        cerr << "Check that all the sequences it contains are the same length." 
             << endl << endl;
        cerr << usage << endl;
        exit(1);
    }

    //Now lets deal with the directory argument, if the user set it.
    if(dflag){
        int result = chdir(darg.c_str());
        if(result != 0){
            cerr << "Error! The directory \"" << darg << "\" could not be opened." 
                 << endl;
            cerr << "Make sure that it exists and you have permissions to access it"
                 << endl << endl;
            cerr << usage << endl;
            exit(1);
        }
    }

    //Next, let's deal with the bias arg
    double bias = 0.01;   //Default value
    if(bflag){
        try{
            bias = stod(barg);
        }
        catch (std::invalid_argument e){
            cerr << "Error! The provided bias arg \"" << barg << "\"," << endl;
            cerr << "could not be converted to a real number value." << endl;
            cerr << endl;
            cerr << usage << endl;
            exit(1);
        }

        if(bias < 0 || bias >= 1){
            cerr << "Error! The bias parameter must be in (0..1]." 
                 << endl << endl;
            cerr << usage << endl; 
            exit(1);
        }
    }

    //Deal with the length parameter TODO more extensive testing
    size_t length = input_sequences.length();   //Default value
    if(lflag){
        try{
            length = stoul(larg); 
        }
        catch (std::invalid_argument e){
            cerr << "Error! The length arg \"" << larg << "\", "  << endl;
            cerr << "could not be converted to an non-negative integer value.";
            cerr << endl << endl;
            cerr << usage << endl;
            exit(1);
        }
    }

    //deal witht the number of replicates
    size_t number = 1; //Default value
    if(nflag){
        try{
            number = stoul(narg); 
        }
        catch (std::invalid_argument e){
            cerr << "Error! The number arg \"" << narg << "\", "  << endl;
            cerr << "could not be converted to an non-negative integer value.";
            cerr << endl << endl;
            cerr << usage << endl;
            exit(1);
        }
    }

    //Finally, we need to deal with seeding the RNG
    mt19937_64 rng;
    if(sflag){
        size_t seed;
        try{
            seed = stoul(sarg); 
        }
        catch (std::invalid_argument e){
            cerr << "Error! The seed arg \"" << sarg << "\", "  << endl;
            cerr << "could not be converted to an non-negative integer value.";
            cerr << endl << endl;
            cerr << usage << endl;
            exit(1);
        }
        rng.seed(seed);
    }
    else{
        struct timeval tp;
        gettimeofday(&tp, NULL);
        long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; 
        rng.seed(ms);
    }

    //The last step, farm off the resampling work to another function.
    SERESResample(number, length, bias, rng, input_sequences, input_taxa);

    return 0;
}
