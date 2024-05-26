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
            bool isPieceWhite = (piece.color() == Color::WHITE);
            if (isPieceWhite && isWhiteToMove) 
            {
                //Friendly pieces
                PieceType type = piece.type();
                uint8_t tempValue;

                featureIdx = static_cast<size_t>(piece.type()) * 64 + square;
                us.addFeature(featureIdx, nnue_params);
            }

            if (!isPieceWhite && isWhiteToMove)
            {
                //Enemy pieces
                featureIdx = (static_cast<size_t>(piece.type()) + 6) * 64 + square;
                them.addFeature(featureIdx, nnue_params);
            }

            if (!isPieceWhite && !isWhiteToMove)
            {
                //Friendly pieces
                featureIdx = (static_cast<size_t>(piece.type()) + 6) * 64 + square;
                them.addFeature(featureIdx, nnue_params);
            }

            if (isPieceWhite && !isWhiteToMove)
            {
                //Enemy pieces
                featureIdx = (static_cast<size_t>(piece.type()) + 6) * 64 + square;
                them.addFeature(featureIdx, nnue_params);
            }
        }
    }

    return nnue_params.evaluate(us, them);
}

uint8_t getFriendlyPiece(PieceType type)
{
    if (type == PieceType::PAWN)
    {
        return usPawn;
    }
    else if (type == PieceType::KNIGHT)
    {
        return usKnight;
    }
    else if (type == PieceType::BISHOP)
    {
        return usBishop;
    }
    else if (type == PieceType::ROOK)
    {
        return usRook;
    }
    else if (type == PieceType::QUEEN)
    {
        return usQueen;
    }
    else if (type == PieceType::KING)
    {
        return usKing;
    }
}

uint8_t getOpponentPiece(PieceType type)
{
    if (type == PieceType::PAWN)
    {
        return opponentPawn;
    }
    else if (type == PieceType::KNIGHT)
    {
        return opponentKnight;
    }
    else if (type == PieceType::BISHOP)
    {
        return opponentBishop;
    }
    else if (type == PieceType::ROOK)
    {
        return opponentRook;
    }
    else if (type == PieceType::QUEEN)
    {
        return opponentQueen;
    }
    else if (type == PieceType::KING)
    {
        return opponentKing;
    }
}
