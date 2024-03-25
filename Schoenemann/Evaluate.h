#pragma once
#include <iostream>
#include "movegen/chess.hpp"

int evaluate(chess::Board& board);
int countPieces(const chess::Color& color, chess::Board& board);
int count_amount(chess::Board& board, chess::PieceType type);