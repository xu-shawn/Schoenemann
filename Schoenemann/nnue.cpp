#include "nnue.h"
#include "incbin.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>

INCBIN(network, "simple-98.bin");
const Network& net = *reinterpret_cast<const Network*>(gnetworkData);

int32_t crelu(int16_t x)
{
    return std::clamp(static_cast<int32_t>(x), 0, QA);
}

Accumulator::Accumulator(const Network& network)
{
    vals = network.featureBias;
}

void Accumulator::addFeature(size_t featureIdx, const Network& network)
{
    for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
        vals[i] += network.featureWeights[featureIdx].vals[i];
    }
}

void Accumulator::removeFeature(size_t featureIdx, const Network& network)
{
    for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
        vals[i] -= network.featureWeights[featureIdx].vals[i];
    }
}

int32_t Network::evaluate(const Accumulator& us, const Accumulator& them) const
{
    int32_t output = outputBias;

    for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
        output += crelu(us.vals[i]) * static_cast<int32_t>(outputWeights[i]);
    }

    for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
        output += crelu(them.vals[i]) * static_cast<int32_t>(outputWeights[HIDDEN_SIZE + i]);
    }

    output *= SCALE;
    output /= QA * QB;

    return output;
}
int evaluatePosition(Board& board)
{
    bool isWhiteToMove = board.sideToMove() == Color::WHITE;

    Accumulator us(net);
    Accumulator them(net);

    for (uint8_t square = 0; square < 64; ++square)
    {
        chess::Piece piece = board.at(square);
        if (piece != chess::Piece::NONE)
        {
            bool isPieceWhite = piece.color() == Color::WHITE;

            int whiteIndex = 0;
            int blackIndex = 0;

            constexpr size_t color_offset = 64 * 6;
            constexpr size_t piece_offset = 64;

            if (isPieceWhite)
            {
                whiteIndex += piece.color() * color_offset + piece.type() * piece_offset + square;
            }
            else
            {
                blackIndex += ~piece.color() * color_offset + piece.type() * piece_offset + square ^ 56;
            }


            if (isPieceWhite) {
                us.addFeature(whiteIndex, net);
            }
            else {
                them.addFeature(blackIndex, net);
            }
        }
    }
    return net.evaluate(us, them);
}
