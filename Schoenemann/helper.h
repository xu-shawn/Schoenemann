#pragma once

#include "consts.h"
#include "movegen/chess.hpp"

#include <chrono>

void transpositionTableTest(Board& board);
void testCommand();
void runBenchmark();
void uciPrint();