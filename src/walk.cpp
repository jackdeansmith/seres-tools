#include "walk.hpp"
#include <istream>
using std::istream;
#include <ostream>
using std::ostream;
#include <vector>
using std::vector;
#include <algorithm>
using std::upper_bound;

#include <iostream>
using std::cerr;

//Reverse direciton function
Direction ReverseDirection(const Direction& input){
    if(input == Direction::Right){
        return Direction::Left;
    }
    else{
        return Direction::Right;
    }
}

//Overloads for printing and reading the textual representations of left and
//right, the lowercase chars 'l' and 'r' respectively.
istream& operator>>(istream& stream, Direction& direction){
    char c;
    stream >> c;
    if(c == 'l'){
        direction = Direction::Left;    
    }
    else if (c == 'r'){
        direction = Direction::Right; 
    }
    else{
        stream.setstate(std::ios::failbit);
    }
    return stream;
}
ostream& operator<<(ostream& stream, const Direction& direction){
    if(direction == Direction::Left){
        stream << 'l';
    }
    else{
        stream << 'r';
    }
    return stream;
}

//Overloads for printing and grabbing textual representations of walk segments.
istream& operator>>(istream& stream, WalkSegment& segment){
    char c;
    stream >> segment.replicate_pos;
    stream >> c; if(c != ':') stream.setstate(std::ios::failbit);
    stream >> segment.original_pos;
    stream >> c; if(c != ':') stream.setstate(std::ios::failbit);
    stream >> segment.length;
    stream >> c; if(c != ':') stream.setstate(std::ios::failbit);
    stream >> segment.direction;
    return stream;
}
ostream& operator<<(ostream& stream, const WalkSegment& segment){
    stream << segment.replicate_pos << ':';
    stream << segment.original_pos << ':';
    stream << segment.length << ':';
    stream << segment.direction;
    return stream;
}

//Constructor
RandomWalk::RandomWalk(vector<WalkSegment> invec): RandomWalk(){

}

bool RandomWalk::add(WalkSegment current){


    //The trivial case is easy, if the sequence is empty, anything is valid
    if(sequence_.empty()){
        sequence_.push_back(current); 
        return true;
    }

    //We can determine if the new ws is allowed by comparing it to the back of
    //the current sequence and seeing if it meets our chriteria.
    const WalkSegment& last = sequence_.back();

    //Replicate position must be exactly one greater
    if(current.replicate_pos != last.replicate_pos + last.length){
        return false; 
    }

    //Direction must reverse
    if(current.direction != ReverseDirection(last.direction)){
        return false; 
    }

    //And the position must be one after or one before the last positon in the
    //last segment, depending on the direction.
    if(last.direction == Direction::Right){
        size_t last_pos = last.original_pos + last.length - 1;
        if(current.original_pos != last_pos - 1){
            return false; 
        }
    }
    else{
        size_t last_pos = last.original_pos - last.length + 1;
        if(current.original_pos != last_pos + 1){
            return false; 
        }
    }

    sequence_.push_back(current);
    return true;
}

bool RandomWalk::add(size_t original_pos, size_t run_length, Direction direction){
    size_t replicate_pos = length();
    return add(WalkSegment(replicate_pos, original_pos, run_length, direction));
}

size_t RandomWalk::length() const{
    if(sequence_.empty()){
        return 0; 
    }

    return sequence_.back().replicate_pos + sequence_.back().length;
}

//Functions used to lookup positions or breakpoints, will throw if
//positions can't be looked up.
size_t RandomWalk::lookup_position(size_t pos) const{

    //Find the segment with the largest replicate position not larger than the
    //querry, use binary search for this.
    auto iter = upper_bound(sequence_.begin(), sequence_.end(), pos,
                [](size_t pos, WalkSegment ws){return ws.replicate_pos > pos;});
    iter--;


    //Figure out how much we are off by
    size_t diff = pos - iter->replicate_pos;

    //And use that to calculate the final position
    if(iter->direction == Direction::Right){
        return iter -> original_pos + diff;    
    }
    else{
        return iter -> original_pos - diff;    
    }
}
size_t RandomWalk::lookup_breakpoint(size_t bkpt) const{

    //Find the segment with the largest replicate position not larger than the
    //querry, use binary search for this.
    auto iter = upper_bound(sequence_.begin(), sequence_.end(), bkpt,
                [](size_t pos, WalkSegment ws){return ws.replicate_pos > pos;});
    iter--;


    //Figure out how much we are off by
    size_t diff = bkpt - iter->replicate_pos;

    //And use that to calculate the final position
    if(iter->direction == Direction::Right){
        return iter -> original_pos + diff;    
    }
    else{
        return iter -> original_pos - diff + 1;    
    }

}

//Functions which allow access to the internal sequence, these do not
//allow modification as this could break internal state.
vector<WalkSegment>::const_iterator RandomWalk::begin() const{
    return sequence_.cbegin();
}
vector<WalkSegment>::const_iterator RandomWalk::end() const{
    return sequence_.cend();
}

//Stream overloads for the walk object
istream& operator>>(istream& stream, RandomWalk& walk){
    WalkSegment ws;
    while(stream >> ws){
        walk.add(ws);
        char c;
        stream >> c;
    }
    return stream;
}

ostream& operator<<(ostream& stream, const RandomWalk& walk){
    for (auto iter = walk.begin(); iter != walk.end(); iter++){
        stream << *iter;
        if(iter != walk.end() - 1){
            stream << ", ";
        }
    }
    stream << ';';
    return stream;
}
