#include "nnue.h"
#include "movegen/chess.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

inline int32_t crelu(int16_t x) {
    return std::clamp(static_cast<int32_t>(x), 0, QA);
}

void load_network(Network& network, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    file.read(reinterpret_cast<char*>(network.feature_weights.data()), sizeof(network.feature_weights));
    file.read(reinterpret_cast<char*>(network.feature_bias.data()), sizeof(network.feature_bias));
    file.read(reinterpret_cast<char*>(network.output_weights.data()), sizeof(network.output_weights));
    file.read(reinterpret_cast<char*>(&network.output_bias), sizeof(network.output_bias));

    file.close();
}

int evaluate_position(uint64_t hash, const Network& network) {

    // Convert Zobrist hash to two side-to-move accumulators
    std::array<int16_t, HIDDEN_SIZE> us = {}, them = {};
    for (size_t i = 0; i < 64; ++i) {
        if ((hash >> i) & 1) {
            us[i % HIDDEN_SIZE]++;
        }
        else {
            them[i % HIDDEN_SIZE]++;
        }
    }

    // Evaluate using the network
    return network.evaluate(us, them);
}


