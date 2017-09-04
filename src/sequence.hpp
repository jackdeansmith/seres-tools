#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <istream>
#include <ostream>

//Char Matrix is a light RAII wrapper for a raw two dimentional array of chars.
//It is used in this package to represent sequenece alignments.
class CharMatrix{
    private:

        //Internal data, just an array and a height/length
        size_t height_ = 0;          //The # of rows
        size_t length_ = 0;          //The # of cols
        char* block_ = nullptr;      //Pointer to memory with data. 

    public:

        //Default constructor allowed and safe, second constructor just
        //allocates memory, makes no guarentees about contents of the memory.
        CharMatrix() = default;
        CharMatrix(size_t height, size_t length);

        //This object uses the copy swap idiom
        ~CharMatrix();
        CharMatrix(const CharMatrix& other);
        CharMatrix(CharMatrix&& other);
        CharMatrix& operator=(CharMatrix other);
        friend void swap(CharMatrix& first, CharMatrix& second){
            using std::swap; 
            swap(first.height_, second.height_);
            swap(first.length_, second.length_);
            swap(first.block_, second.block_);
        }
        
        //Getters for the dimentions of the matrix, inline.
        size_t length() const{return length_;};
        size_t height() const{return height_;};

        //Getter and setter for elements of the matrix.
        //Warning! These are not memory safe, calling them with invalid indicies
        //results in undefined behavior.
        char get(size_t row_index, size_t col_index) const;
        void set(size_t row_index, size_t col_index, char c);

        //Methods which return references to a particular scored character, two
        //versions for const and non-const access.
        //These are memory safe and will throw an exception if used improperly.
              char& at(size_t row_index, size_t col_index)      ;
        const char& at(size_t row_index, size_t col_index) const;
};

//These functions allow reading and writing of FASTA formatted multiple sequence
//alignments from arbirarty iostreams. Each takes a stream, a CharMatrix, and a
//vector of strings which name the taxa. Both may throw std::runtime_error if
//parsing or writing fails. 
void ReadFASTA(std::istream&, CharMatrix&, std::vector<std::string>&);
void WriteFASTA(std::ostream&, const CharMatrix&, const std::vector<std::string>&);
