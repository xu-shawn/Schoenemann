#pragma once
#include "movegen/position.h"
#include "movegen/tables.h"
#include "movegen/types.h"

int search(int depth, int alpha, int beta, Position& position);
Move& getBestMove();
