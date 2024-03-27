#include "Evaluate.h"
#include <iostream>
#include "movegen/chess.hpp"

const short pawnValue = 100;
const short knightValue = 300;
const short bishopValue = 325;
const short rookValue = 500;
const short queenValue = 900;

int evaluate(chess::Board& board) {
    int evaluation = 0;
    int white_evaluation = count_material(board, chess::Color::WHITE);
    int black_evaluation = count_material(board, chess::Color::BLACK);

    evaluation = white_evaluation - black_evaluation;

    int perspective = board.sideToMove() == chess::Color::WHITE ? 1 : -1;

    return evaluation * perspective;
}

int count_material(chess::Board& board, chess::Color color) {
    int material = 0;
    material += count_amount(board, chess::PieceType::PAWN, color) * pawnValue;
    material += count_amount(board, chess::PieceType::KNIGHT, color) * knightValue;
    material += count_amount(board, chess::PieceType::BISHOP, color) * bishopValue;
    material += count_amount(board, chess::PieceType::ROOK, color) * rookValue;
    material += count_amount(board, chess::PieceType::QUEEN, color) * queenValue;
    return material;
}

int count_amount(chess::Board& board, chess::PieceType type, chess::Color color) {
    chess::Bitboard bitboard = board.pieces(type, color);
    return bitboard.count();
}