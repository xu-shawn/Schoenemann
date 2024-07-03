#include <iostream>

#include "movegen/chess.hpp"
#include "consts.h"
#include "tt.h"

using namespace chess;

Movelist orderMoves(Movelist moveList, Hash* entry);
