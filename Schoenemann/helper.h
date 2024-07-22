#pragma once

#include "consts.h"
#include "evaluate.h"
#include "movegen/chess.hpp"

#include <chrono>

void transpositionTableTest(Board& board);
void testCommand();
void runBenchmark();
void uciPrint();