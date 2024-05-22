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
    Accumulator us(network.feature_bias);
    Accumulator them(network.feature_bias);

    // Füge den Hashwert als Feature hinzu. Dies ist ein vereinfachtes Beispiel,
    // wie man den Hashwert in das Netzwerk einspeisen könnte.
    // In der Praxis müsstest du eine Methode haben, um den Hashwert in die Feature-Matrix zu integrieren.

    // Hier könnte man den Hashwert in Teile zerlegen und als Features hinzufügen.
    // us.add_feature(hash % 768, network.feature_weights);  // Beispiel für Modulo-Operation
    // them.add_feature((hash / 768) % 768, network.feature_weights);  // Beispiel für Division und Modulo

    // Beispielhafte einfache Nutzung:
    us.add_feature(hash % 768, network.feature_weights);
    them.add_feature((hash / 768) % 768, network.feature_weights);

    return network.evaluate(us, them);
}


