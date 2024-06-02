#include "nnue.h"
#include "incbin.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>

Network net;

void initNNUE(const char* file)
{
    FILE* nn = fopen(file, "rb");
    if (!nn) 
    {
        std::cerr << "Error opening the file " << file << "\n";
        exit(1);
    }

    size_t read = 0;
    size_t expectedRead = 0;

    //featureWeights
    for (size_t i = 0; i < net.featureWeights.size(); ++i) 
    {
        read += fread(net.featureWeights[i].vals.data(), sizeof(int16_t), HIDDEN_SIZE, nn);
        expectedRead += HIDDEN_SIZE;
    }

    //featureBias
    read += fread(net.featureBias.data(), sizeof(int16_t), HIDDEN_SIZE, nn);
    expectedRead += HIDDEN_SIZE;

    //outputWeights
    read += fread(net.outputWeights.data(), sizeof(int16_t), HIDDEN_SIZE * 2, nn);
    expectedRead += HIDDEN_SIZE * 2;

    //outputBias
    read += fread(&net.outputBias, sizeof(int16_t), 1, nn);
    expectedRead += 1;

    fclose(nn);

    if (read != expectedRead) {
        std::cerr << "Error loading the net, aborting ";
        std::cerr << "Expected " << expectedRead << " shorts, got " << read << "\n";
        exit(1);
    }
}


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

            int pieceTypeIndex = getPieceType(piece.type(), isFriendly) % 6;

            int featureIndex = pieceTypeIndex * 64 + newSq;

            if (pieceTypeIndex >= 0) // Ensure valid feature index
            {
                if (isFriendly) {
                    us.addFeature(featureIndex, net);
                }
                else {
                    them.addFeature(featureIndex, net);
                }
            }
        }
    }

    return net.evaluate(us, them);
}
