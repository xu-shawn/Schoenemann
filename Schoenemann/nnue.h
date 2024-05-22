#include "movegen/chess.hpp"
#include <vector>
#include <iostream>


const int HIDDEN_SIZE = 16;
const int SCALE = 400;
const int QA = 255;
const int QB = 64;

inline int32_t crelu(int16_t x);

// Network structure
struct Network {
    std::array<std::array<int16_t, HIDDEN_SIZE>, 768> feature_weights;
    std::array<int16_t, HIDDEN_SIZE> feature_bias;
    std::array<int16_t, 2 * HIDDEN_SIZE> output_weights;
    int16_t output_bias;

    // Evaluate function
    int evaluate(const std::array<int16_t, HIDDEN_SIZE>& us, const std::array<int16_t, HIDDEN_SIZE>& them) const {
        int output = output_bias;

        // Side-To-Move Accumulator -> Output.
        for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
            output += crelu(us[i]) * output_weights[i];
        }

        // Not-Side-To-Move Accumulator -> Output.
        for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
            output += crelu(them[i]) * output_weights[HIDDEN_SIZE + i];
        }

        // Apply eval scale.
        output *= SCALE;

        // Remove quantisation.
        output /= QA * QB;

        return output;
    }
};

// Accumulator structure
struct Accumulator {
    std::array<int16_t, HIDDEN_SIZE> vals;

    // Constructor
    Accumulator(const Network& net) : vals(net.feature_bias) {}

    // Add a feature to an accumulator.
    void add_feature(size_t feature_idx, const Network& net) {
        for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
            vals[i] += net.feature_weights[feature_idx][i];
        }
    }

    // Remove a feature from an accumulator.
    void remove_feature(size_t feature_idx, const Network& net) {
        for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
            vals[i] -= net.feature_weights[feature_idx][i];
        }
    }
};

int evaluate_position(uint64_t hash, const Network& network);
void load_network(Network& network, const std::string& filepath);