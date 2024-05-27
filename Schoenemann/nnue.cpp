#include "nnue.h"
#include "incbin.h"

// Including the binary network
INCBIN(nnue, "simple-98.bin");

const Network& nnue_params = *reinterpret_cast<const Network*>(gnnueData);

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
        output += crelu(them.vals[i]) * static_cast<int32_t>(outputWeights[i]);
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
    Accumulator us(nnue_params);
    Accumulator them(nnue_params);

    //Add features to the accumulators based on the current board state
    for (uint8_t square = 0; square < 64; ++square)
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
                us.addFeature(featureIndex, nnue_params);
            }
            else
            {
                //Opponent piece
                int featureIndex = (piece.type() + 6) * 64 + newSq;
                them.addFeature(featureIndex, nnue_params);
            }
        }
    }

    return nnue_params.evaluate(us, them);
}
