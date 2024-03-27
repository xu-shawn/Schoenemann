#pragma once
#include <iostream>
#include "movegen/chess.hpp"

int evaluate(chess::Board& board);
int count_material(chess::Board& board, chess::Color color);
int count_amount(chess::Board& board, chess::PieceType type, chess::Color color);