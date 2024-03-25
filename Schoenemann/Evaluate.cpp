#include "Evaluate.h"
#include <iostream>
#include "movegen/chess.hpp"

const short pawnValue = 100;
const short knightValue = 300;
const short bishopValue = 325;
const short rookValue = 500;
const short queenValue = 900;

int evaluate(chess::Board& board) {
    int material = 0;
    material += count_amount(board, chess::PieceType::PAWN) * pawnValue;
    material += count_amount(board, chess::PieceType::KNIGHT) * knightValue;
    material += count_amount(board, chess::PieceType::BISHOP) * bishopValue;
    material += count_amount(board, chess::PieceType::ROOK) * rookValue;
    material += count_amount(board, chess::PieceType::QUEEN) * queenValue;

    if (board.sideToMove() == chess::Color::BLACK)
    {
        material = -material;
    }

    return material;
}

int count_amount(chess::Board& board, chess::PieceType type) {
    chess::Bitboard bitboard = board.pieces(type, board.sideToMove());
    return bitboard.count();
}