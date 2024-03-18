#include "Evaluate.h"
#include <iostream>
#include "movegen/position.h"
#include "movegen/tables.h"
#include "movegen/types.h"


const short pawnValue = 100;
const short knightValue = 300;
const short bishopValue = 325;
const short rookValue = 500;
const short queenValue = 900;

int evaluate(Position& position) {
    return countPieces(position.turn(), position);
}

int countPieces(const Color& color, Position& position) {
    int material = 0;
    if (color == WHITE)
    {
        material += count_amount(position, Piece::WHITE_PAWN) * pawnValue;
        material += count_amount(position, Piece::WHITE_KNIGHT) * knightValue;
        material += count_amount(position, Piece::WHITE_BISHOP) * bishopValue;
        material += count_amount(position, Piece::WHITE_ROOK) * rookValue;
        material += count_amount(position, Piece::WHITE_QUEEN) * queenValue;
    }
    else
    {
        material -= count_amount(position, Piece::BLACK_PAWN) * pawnValue;
        material -= count_amount(position, Piece::BLACK_KNIGHT) * knightValue;
        material -= count_amount(position, Piece::BLACK_BISHOP) * bishopValue;
        material -= count_amount(position, Piece::BLACK_ROOK) * rookValue;
        material -= count_amount(position, Piece::BLACK_QUEEN) * queenValue;
    }


    return material;
}

int count_amount(Position& position, Piece type) {
    return pop_count(position.bitboard_of(type));
}