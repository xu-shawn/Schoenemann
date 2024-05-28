#include "nnue.h"
#include "incbin.h"

#ifdef _MSC_VER
#define SP_MSVC
#pragma push_macro("_MSC_VER")
#undef _MSC_VER
#endif

#define INCBIN_PREFIX g_
#include "incbin.h"

#ifdef SP_MSVC
#pragma pop_macro("_MSC_VER")
#undef SP_MSVC
#endif

INCBIN(network, "simple-98.bin");
const Network& networktpa = *reinterpret_cast<const Network*>(g_networkData);

int32_t crelu(int16_t x)
{
	return std::clamp(static_cast<int32_t>(x), 0, QA);
}

Accumulator::Accumulator(Network network)
{
    vals = network.featureBias;
}

void Accumulator::addFeature(size_t featureIdx, Network network)
{
    for (size_t i = 0; i < HIDDEN_SIZE; i++) {
        vals[i] += network.featureWeights[featureIdx].vals[i];
    }
}

void Accumulator::removeFeature(size_t featureIdx, Network network)
{
    for (size_t i = 0; i < HIDDEN_SIZE; i++) {
        vals[i] -= network.featureWeights[featureIdx].vals[i];
    }
}

int32_t Network::evaluate(const Accumulator& us, const Accumulator& them) const
{
    int output = static_cast<int32_t>(outputBias);

    //Side-To-Move Accumulator
    for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
        output += crelu(us.vals[i]) * static_cast<int32_t>(outputWeights[i]);
    }

    //Non-Side-To-Move Accumulator
    for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
        output += crelu(them.vals[i]) * static_cast<int32_t>(outputWeights[HIDDEN_SIZE + i]);
    }

    //Apply the eval scale
    output *= SCALE;

    //Remove quantization
    output /= QA * QB;

    return output;
}

int evaluatePosition(Board& board)
{
    //Get the side to move
    bool isWhiteToMove = board.sideToMove() == Color::WHITE;

    //Initialize accumulators for the side to move and the opponent
    Accumulator us(networktpa);
    Accumulator them(networktpa);

    //Add features to the accumulators based on the current board state
    for (uint8_t square = 0; square < 64; square++)
    {
        size_t newSq = isWhiteToMove ? square : square ^ 56;
        Piece piece = board.at(square);

        if (piece != Piece::NONE)
        {
            bool isPieceWhite = piece.color() == Color::WHITE;
            bool isFriendly = (isWhiteToMove && isPieceWhite) || (!isWhiteToMove && !isPieceWhite);

            //Determine the feature index based on the perspective of the side to move
            if (isFriendly)
            {
                //Friendly piece
                int featureIndex = piece.type() * 64 + newSq;
                us.addFeature(featureIndex, networktpa);
            }
            else
            {
                //Opponent piece
                int featureIndex = (piece.type() + 6) * 64 + newSq;
                them.addFeature(featureIndex, networktpa);
            }
        }
    }

    return networktpa.evaluate(us, them);
}
