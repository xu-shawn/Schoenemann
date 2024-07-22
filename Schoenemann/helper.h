#include "consts.h"
#include "evaluate.h"
#include "movegen/chess.hpp"

#include <chrono>

void transpositionTableTest(Board& board);
void testCommand();
void run_benchmark();
void nnTest(Board& board);
void uciPrint();