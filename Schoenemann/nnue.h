#include "movegen/chess.hpp"
#include <vector>
#include <iostream>


const int HIDDEN_SIZE = 16;
const int SCALE = 400;
const int QA = 255;
const int QB = 64;

inline int32_t crelu(int16_t x);

// Quantized format for the network
struct Network {
    std::array<std::array<int16_t, HIDDEN_SIZE>, 768> feature_weights;
    std::array<int16_t, HIDDEN_SIZE> feature_bias;
    std::array<int16_t, 2 * HIDDEN_SIZE> output_weights;
    int16_t output_bias;

    int32_t evaluate(const std::array<int16_t, HIDDEN_SIZE>& us,
        const std::array<int16_t, HIDDEN_SIZE>& them) const {
        int32_t output = output_bias;
        for (std::size_t i = 0; i < HIDDEN_SIZE; ++i) {
            output += crelu(us[i]) * output_weights[i];
        }
        for (std::size_t i = 0; i < HIDDEN_SIZE; ++i) {
            output += crelu(them[i]) * output_weights[HIDDEN_SIZE + i];
        }
        output *= SCALE;
        output /= (QA * QB);
        return output;
    }
};

// A column of the feature-weights matrix
struct Accumulator {
    std::array<int16_t, HIDDEN_SIZE> vals;

    Accumulator(const Network& net) {
        vals = net.feature_bias;
    }

    void add_feature(std::size_t feature_idx, const Network& net) {
        for (std::size_t i = 0; i < HIDDEN_SIZE; ++i) {
            vals[i] += net.feature_weights[feature_idx][i];
        }
    }

    void remove_feature(std::size_t feature_idx, const Network& net) {
        for (std::size_t i = 0; i < HIDDEN_SIZE; ++i) {
            vals[i] -= net.feature_weights[feature_idx][i];
        }
    }
};


int evaluate_position(uint64_t hash, const Network& network);
void load_network(Network& network, const std::string& filepath);
void zobrist_to_accumulators(uint64_t zobrist, Accumulator& us, Accumulator& them, const Network& net);