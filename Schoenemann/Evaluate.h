#pragma once
#include <iostream>
#include "movegen/position.h"
#include "movegen/tables.h"
#include "movegen/types.h"

int evaluate(Position& position);
int countPieces(const Color& color, Position& position);
int count_amount(Position& position, Piece type);