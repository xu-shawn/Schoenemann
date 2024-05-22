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

    Accumulator us(network);
    Accumulator them(network);

    zobrist_to_accumulators(hash, us, them, network);

    return network.evaluate(us.vals, them.vals);
}

void zobrist_to_accumulators(uint64_t zobrist, Accumulator& us, Accumulator& them, const Network& net) {
    // Here we assume that each bit in the Zobrist hash corresponds to a feature
    // This is a simple example. In a real case, you would map this more meaningfully.
    for (std::size_t i = 0; i < 64; ++i) {
        if (zobrist & (1ULL << i)) {
            us.add_feature(i % 768, net);  // Modulo 768 to wrap around features if necessary
            them.add_feature(i % 768, net);
        }
    }
}



