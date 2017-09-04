#pragma once

#include "sequence.hpp"
#include "walk.hpp"

#include <utility>
#include <random>

/* A resampling operation is defined entirely by a key. Thus the process of
 * resampling is devided into two steps.
 *     1. A key is generated via some parameters
 *     2. The key is used in conjunction with an input alignment to produce a
 *     replicate alignment.
 */

RandomWalk GenerateRandomWalk(size_t input_length, size_t output_length, 
                              double turnaround_bias, std::mt19937_64& rng);

CharMatrix Resample(const CharMatrix& input_matrix, const RandomWalk& walk);
