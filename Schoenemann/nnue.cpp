#include "nnue.h"
#include "incbin.h"

// Including the binary network
INCBIN(nnue, "simple-10.bin");

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
    bool isWhiteToMove = (board.sideToMove() == Color::WHITE);

    //Initialize accumulators for the side to move and the opponent
    Accumulator us(nnue_params);
    Accumulator them(nnue_params);

    //Add features to the accumulators based on the current board state
    for (size_t square = 0; square < 64; ++square) 
    {
        Piece piece = board.at(square);

        if (piece != Piece::NONE) 
        {
            size_t featureIdx = 0;
            bool isPieceWhite = piece.color() == Color::WHITE;

            //If we are white and it is a white piece it is a our piece
            if (isPieceWhite && isWhiteToMove) 
            {
                //Friendly piece
                featureIdx = static_cast<size_t>(piece.type()) * 64 + square;
                us.addFeature(featureIdx, nnue_params);
            }

            //If we are white but the piece is black it is a opponent piece
            if (!isPieceWhite && isWhiteToMove)
            {
                //Opponent piece
                featureIdx = static_cast<size_t>(piece.type() + 6) * 64 + square;
                them.addFeature(featureIdx, nnue_params);
            }

            //If we are black and it is a black piece it is a our piece
            if (!isPieceWhite && !isWhiteToMove)
            {
                //Friendly piece
                featureIdx = (static_cast<size_t>(piece.type())) * 64 + square;
                us.addFeature(featureIdx, nnue_params);
            }

            //If we are black but the piece is white it is a opponent piece
            if (isPieceWhite && !isWhiteToMove)
            {
                //Opponent piece
                featureIdx = (static_cast<size_t>(piece.type()) + 6) * 64 + square;
                them.addFeature(featureIdx, nnue_params);
            }
        }
    }

    return nnue_params.evaluate(us, them);
}
