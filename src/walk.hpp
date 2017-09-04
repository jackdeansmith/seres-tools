/* This file defines the types which represent the actions taken by the
 * resampler in order to create a random walk replicate.
 *
 * Each random walk is composed of a sequence of WalkSegments, each encoding a
 * contiguous run where the resampler is moving left or right over columns of 
 * the original alignment and appending them to thre replicate.
 */

#pragma once

#include <vector>
#include <istream>
#include <ostream>

//Simple direction, used to represent which direction the resampler is moving at
//any point in time.
enum class Direction{Right, Left};

//Convinience function for reversing a direction
Direction ReverseDirection(const Direction&);

//Overloads for reading and writing the textual representation of right and
//left, simply the lowercase acsii chars 'l' and 'r'
std::istream& operator>>(std::istream&, Direction&);
std::ostream& operator<<(std::ostream&, const Direction&);

//A complete representation of a sequence for which the resampler did not
//reverse direction. Answers the following questions:
struct WalkSegment{
    //How far along the replicate is this sequence of homologus columns?
    size_t replicate_pos;
    //Where in the original does this sequence begin?
    size_t original_pos;
    //How long does the sequence go before the resampler changed directions?
    size_t length;
    //Which way was the resampler moving over the original during this segment?
    Direction direction;

    //Constructed exactly how you would expect
    WalkSegment() = default;
    WalkSegment(size_t r, size_t o, size_t l, Direction d): 
        replicate_pos(r), original_pos(o), length(l), direction(d){};
};

//These stream overloads give a bijection between a walk segment and it's serial
//representation.
//"A run starting at the replicate position 4 and original position 8 moving
//right for 10 sites" <-> "4:8:10:r"
std::istream& operator>>(std::istream&, WalkSegment&);
std::ostream& operator<<(std::ostream&, const WalkSegment&);

//This composite type is a whole sequence of adjacent WalkSegments which
//represents an entire run of the resampler. This object compleetly defines a
//whole resampling operation and thus can be used to create concrete replicate
//alignments or to translate positions in replicates back to positions in the
//originals.
class RandomWalk{

    //Internally it is just a sequence, extra logic ensures that it is valid
    private:
        std::vector<WalkSegment> sequence_;

    public:
        
        //Empty construction is always allowed, as is construction from a
        //sequence.
        RandomWalk() = default;
        RandomWalk(std::vector<WalkSegment>);

        //returns false if the segment could not be added TODO comment
        bool add(WalkSegment);

        //Add a new walk segment while infering the replicate position
        bool add(size_t original_pos, size_t lenght, Direction direction);

        //Functions used to lookup positions or breakpoints, will throw if
        //positions can't be looked up.
        size_t lookup_position(size_t) const;
        size_t lookup_breakpoint(size_t) const;

        size_t length() const;

        //Functions which allow access to the internal sequence, these do not
        //allow modification as this could break internal state.
        std::vector<WalkSegment>::const_iterator begin() const;
        std::vector<WalkSegment>::const_iterator end() const;
};

//Overloads for reading and writing random walks
std::istream& operator>>(std::istream&, RandomWalk&);
std::ostream& operator<<(std::ostream&, const RandomWalk&);
