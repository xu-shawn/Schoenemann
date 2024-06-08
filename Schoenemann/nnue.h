#ifndef NNUE_H
#define NNUE_H

#include "movegen/chess.hpp"
#include "incbin.h"
#include <algorithm>
#include <iostream>
#include <cstdint> 
#include <vector>
#include <array> 

using namespace chess;

//Forward deklaration
struct Network;

//Consts
const short HIDDEN_SIZE = 64;
const short SCALE = 400;
const int32_t QA = 255;
const short QB = 64;
const int INPUT_WEIGHTS = 768;

//The Activation function
//Gets a 16 bit number as an input and outputs the corresponding number as a 32 bit number
//It also make sure that it is not outside the bound of QA
int32_t crelu(int16_t x);

struct Accumulator
{
    std::array<int16_t, HIDDEN_SIZE> vals;

    // Default constructor
    Accumulator() = default;

    // Constructor to get the network bias
    Accumulator(const Network& network);

    // Add a feature to the Accumulator
    void addFeature(size_t featureIdx, const Network& network);

    // Remove a feature from the Accumulator
    void removeFeature(size_t featureIdx, const Network& network);
};


struct Network
{
    std::array<Accumulator, 768> featureWeights;
    std::array<int16_t, HIDDEN_SIZE> featureBias;
    std::array<int16_t, 2 * HIDDEN_SIZE> outputWeights;
    int16_t outputBias;

    // Default constructor
    Network() = default;

    // Evaluates the network
    int32_t evaluate(const Accumulator& us, const Accumulator& them) const;
};

int evaluatePosition(Board& board);

#endif // NNUE_H