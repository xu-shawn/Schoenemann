#include <iostream>

#include "evaluate.h"
#include "movegen/chess.hpp"
#include "psqt.h"

using namespace chess;

const short pawnValue = 126;
const short knightValue = 781;
const short bishopValue = 825;
const short rookValue = 1276;
const short queenValue = 2538;

int evaluate(Board& board) {
    int evaluation = 0;
    int whiteEvaluation = count_material(board, Color::WHITE);
    int blackEvaluation = count_material(board, Color::BLACK);

    evaluation = whiteEvaluation - blackEvaluation;

    int perspective;

    if (board.sideToMove() == Color::WHITE)
    {
        perspective = 1;
    }
    else
    {
        perspective = -1;
    }

    return evaluation * perspective;
}

int count_material(Board& board, Color color) {
    int material = 0;
    psqt bouns;
    //Pawn
    material += count_amount(board, PieceType::PAWN, color) * (pawnValue + bouns.getPieceBounus(board, PieceType::PAWN, color));
    //std::cout << "color " << color << " pawnvalue " << count_amount(board, PieceType::PAWN, color) * (pawnValue + bouns.getPieceBounus(board, PieceType::PAWN, color)) << " amount " << count_amount(board, PieceType::PAWN, color) << " bounus " << bouns.getPieceBounus(board, PieceType::PAWN, color) << std::endl;
    material += count_amount(board, PieceType::KNIGHT, color) * (knightValue + bouns.getPieceBounus(board, PieceType::KNIGHT, color));

    short bishopAmount = count_amount(board, PieceType::BISHOP, color);

    if (bishopAmount >= 2)
    {
        material += 15;
    }

    material += bishopAmount * (bishopValue + bouns.getPieceBounus(board, PieceType::BISHOP, color));

    material += count_amount(board, PieceType::ROOK, color) * (rookValue + bouns.getPieceBounus(board, PieceType::ROOK, color));
    material += count_amount(board, PieceType::QUEEN, color) * (queenValue + bouns.getPieceBounus(board, PieceType::QUEEN, color));
    //material += bouns.getPieceBounus(board, PieceType::KING, color);
    return material;
}

int count_amount(Board& board, PieceType type, Color color) {
    Bitboard bitboard = board.pieces(type, color);
    return bitboard.count();
}