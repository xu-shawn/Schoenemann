#include "movegen/chess.hpp"
#include <vector>
#include <iostream>


const int HIDDEN_SIZE = 16;
const int SCALE = 400;
const int QA = 255;
const int QB = 64;

inline int32_t crelu(int16_t x);

struct Accumulator {
    std::array<int16_t, HIDDEN_SIZE> vals;

    Accumulator(const std::array<int16_t, HIDDEN_SIZE>& bias) : vals(bias) {}

    void add_feature(size_t feature_idx, const std::array<std::array<int16_t, HIDDEN_SIZE>, 768>& feature_weights) {
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            vals[i] += feature_weights[feature_idx][i];
        }
    }

    void remove_feature(size_t feature_idx, const std::array<std::array<int16_t, HIDDEN_SIZE>, 768>& feature_weights) {
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            vals[i] -= feature_weights[feature_idx][i];
        }
    }
};

struct Network {
    std::array<std::array<int16_t, HIDDEN_SIZE>, 768> feature_weights;
    std::array<int16_t, HIDDEN_SIZE> feature_bias;
    std::array<int16_t, 2 * HIDDEN_SIZE> output_weights;
    int16_t output_bias;

    int32_t evaluate(const Accumulator& us, const Accumulator& them) const {
        int32_t output = output_bias;

        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            output += crelu(us.vals[i]) * output_weights[i];
        }

        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            output += crelu(them.vals[i]) * output_weights[HIDDEN_SIZE + i];
        }

        output *= SCALE;
        output /= (QA * QB);

        return output;
    }
};

int evaluate_position(uint64_t hash, const Network& network);
void load_network(Network& network, const std::string& filepath);