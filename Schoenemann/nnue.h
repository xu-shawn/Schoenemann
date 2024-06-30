#pragma once
#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <memory>
#include <iostream>
#include <bit>
#include "movegen/chess.hpp"

using namespace chess;

// An early attempt at supporting NNUE evaluation.
// Not meant to be as a permanent solution in its current form.

// Network constants
const int FeatureSize = 768;
const int HiddenSize = 128;

struct NetworkRepresentation {
	std::array<std::array<int16_t, HiddenSize>, FeatureSize> FeatureWeights;
	std::array<int16_t, HiddenSize> FeatureBias;
	std::array<int16_t, HiddenSize * 2> OutputWeights;
	int16_t OutputBias;
};


static NetworkRepresentation* Network;

void LoadNetwork();
int NNEvaluate(const Board& board);

int Popsquare(uint64_t& number);
uint8_t Mirror(const uint8_t sq);

inline int32_t CReLU(const int16_t value) {
	return std::clamp<int32_t>(value, 0, 255);
}