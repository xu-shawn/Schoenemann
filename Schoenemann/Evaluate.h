#pragma once

#include <iostream>

#include "nnue.h"
#include "psqt.h"
#include "consts.h"

#include "movegen/chess.hpp"

using namespace chess;

int evaluate(Board& board);
int count_material(Board& board, Color color);
int count_amount(Board& board, PieceType type, Color color);
int nnueEvaluation(Board& board);
