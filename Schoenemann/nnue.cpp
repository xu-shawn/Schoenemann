#include "nnue.h"
#include "incbin.h"

INCBIN(network, "simple-98.bin");
const Network& networktpa = *reinterpret_cast<const Network*>(gnetworkData);

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
    int32_t output = static_cast<int32_t>(outputBias);

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

    Accumulator us(networktpa);
    Accumulator them(networktpa);

    for (uint8_t square = 0; square < 64; ++square)
    {
        short newSq = isWhiteToMove ? square : square ^ 56;
        Piece piece = board.at(square);

        if (piece != Piece::NONE)
        {
            bool isPieceWhite = piece.color() == Color::WHITE;
            bool isFriendly = (isWhiteToMove && isPieceWhite) || (!isWhiteToMove && !isPieceWhite);

            int featureIndex = piece.type() * 64 + newSq;
            if (isFriendly) {
                us.addFeature(featureIndex, networktpa);
            }
            else {
                them.addFeature(featureIndex, networktpa);
            }
        }
    }

    return networktpa.evaluate(us, them);
}