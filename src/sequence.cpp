#include "sequence.hpp"
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <string>
using std::string; using std::getline;
#include <vector>
using std::vector;
#include <istream>
using std::istream; using std::endl;
#include <ostream>
using std::ostream;

//Primitive constructor just allocates the memory
CharMatrix::CharMatrix(size_t height, size_t length): 
    height_(height), length_(length){
    block_ = new char[height * length];
}

//Destructor just frees the memory
CharMatrix::~CharMatrix(){
    delete[] block_;
}

//Copy constructor, uses std::copy to move memory around
CharMatrix::CharMatrix(const CharMatrix& other):
    height_(other.height_), length_(other.length_){
    block_ = new char[height_ * length_];
    std::copy(other.block_, other.block_ + length_ * height_, block_);
}

//Move constructor
CharMatrix::CharMatrix(CharMatrix&& other): CharMatrix(){
    swap(*this, other); 
}

//Copy assignment operator, uses copy swap idiom
CharMatrix& CharMatrix::operator=(CharMatrix other){
    swap(*this, other);
    return *this;
}

//Getter and setter for internal characters, not memory safe, could cause
//undefined behavior if passed the wrong indicies.
char CharMatrix::get(size_t row_index, size_t col_index) const{
    return block_[row_index * length_ + col_index];
}
void CharMatrix::set(size_t row_index, size_t col_index, char c){
    block_[row_index * length_ + col_index] = c;
}

//Methods which return references, no undefined behavior.
char& CharMatrix::at(size_t row_index, size_t col_index){
    if(row_index >= height_ || col_index >= length_){
        throw std::out_of_range("CharMatrix::at() called with bad indicies!");
    }
    return block_[row_index * length_ + col_index];
}
const char& CharMatrix::at(size_t row_index, size_t col_index) const{
    if(row_index >= height_ || col_index >= length_){
        throw std::out_of_range("CharMatrix::at() called with bad indicies!");
    }
    return block_[row_index * length_ + col_index];
}

//Read a FASTA formatted multiple sequence alignment from the provided istream
//into the provided CharMatrix and taxa vector. This function will throw if the
//provided sequences are not all the same length or if the number of taxa
//provided does not match the number of names provided.
void ReadFASTA(istream& stream, CharMatrix& matrix, vector<string>& taxa){

    //Clear out the taxa vector and create a new vector for our sequences
    taxa.clear();
    vector<string> sequences;

    //Read all lines from the input, skipping blank lines.
    string line;
    while(getline(stream, line)){
        if(line.size() == 0){
            continue;
        }

        //If we find a taxa name...
        if(line[0] == '>'){

            //Add the name to the taxa vector
            line.erase(0, 1); 
            taxa.push_back(line);

            //And add a new empty entry to our sequences, we know another
            //sequence comes next
            sequences.push_back("");
        }

        //Otherwise, we add to the last sequence
        else{
            sequences.back() += line;
        }
    }

    //Check to make sure everything worked out
    if(taxa.size() != sequences.size()){
        throw std::runtime_error("FASTA file not an alignment");
    }

    //Check to make sure all the sequences are the same length
    for(size_t i = 1; i < sequences.size(); i++){
        if(sequences[i].size() != sequences[i-1].size()){
            throw std::runtime_error("FASTA file not an alignment");
        } 
    }
    if(sequences[0].size() == 0){
        throw std::runtime_error("FASTA file not an alignment");
    }

    //Stage 2, create a CharMatrix with those rows, assign it to the matrix
    //param.
    size_t num_rows = sequences.size();
    size_t num_cols = sequences[0].size();
    matrix = CharMatrix(num_rows, num_cols);

    for(size_t row_index=0; row_index<num_rows; row_index++){
        for(size_t col_index=0; col_index<num_cols; col_index++){
            matrix.set(row_index, col_index, sequences[row_index][col_index]);
        }
    }
}

//Write a FASTA formatted multiple sequence alignmet to the provided ostream
//using the matrix parameter to pass sequences and the taxa parameter to give
//names for each row. Throws if taxa.length() != matrix.height()
void WriteFASTA(ostream& stream, const CharMatrix& matrix, 
                const vector<string>& taxa){

    //Get the number of rows and columns from the matrix
    size_t num_rows = matrix.height();
    size_t num_cols = matrix.length();

    //If the user provided us with too many or too few taxon names...
    if(num_rows != taxa.size()){
        //...Throw TODO
        exit(1);
    }

    //For each row index...
    for(size_t row_index=0; row_index < num_rows; row_index++){

        //Write the corisponding taxa name in FASTA format
        stream << '>' << taxa.at(row_index) << endl; 

        //Then write out the contents of the row on the next line
        for(size_t col_index = 0; col_index < num_cols; col_index++){
            stream << matrix.get(row_index, col_index);
        }
        stream << endl;
    }
}
