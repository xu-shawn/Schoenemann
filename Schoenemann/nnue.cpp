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
    return std::clamp(static_cast<int32_t>(x), 0, 64);
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

int getPieceType(chess::PieceType type, bool friendly)
{
    if (friendly)
    {
        switch (type.internal())
        {
        case chess::PieceType::PAWN: return usPawn;
        case chess::PieceType::KNIGHT: return usKnight;
        case chess::PieceType::BISHOP: return usBishop;
        case chess::PieceType::ROOK: return usRook;
        case chess::PieceType::QUEEN: return usQueen;
        case chess::PieceType::KING: return usKing;
        default: return -1; // Handle error or unknown piece type
        }
    }
    else
    {
        switch (type.internal())
        {
        case chess::PieceType::PAWN: return opponentPawn;
        case chess::PieceType::KNIGHT: return opponentKnight;
        case chess::PieceType::BISHOP: return opponentBishop;
        case chess::PieceType::ROOK: return opponentRook;
        case chess::PieceType::QUEEN: return opponentQueen;
        case chess::PieceType::KING: return opponentKing;
        default: return -1; // Handle error or unknown piece type
        }
    }
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
            bool isFriendly = (isWhiteToMove && isPieceWhite) || (!isWhiteToMove && !isPieceWhite);

            // Correct square transformation for black's perspective
            uint8_t rank = square / 8;
            uint8_t file = square % 8;
            uint8_t newSq = isWhiteToMove ? square : (7 - rank) * 8 + (7 - file);

            int featureIndex = piece.type() * 64 + newSq;

                if (isFriendly) {
                    us.addFeature(featureIndex, net);
                }
                else {
                    them.addFeature(featureIndex, net);
                }
        }
    }

    return net.evaluate(us, them);
}
