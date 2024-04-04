#include <iostream>
#include "movegen/chess.hpp"

using namespace chess;

class tt{
public:
	int ttSize;
	tt(int size){
		entries = new entry[size];
		ttSize = size;
		this->size = size;
	}

	~tt() {
		delete[] entries;
	}


	struct entry {
		std::uint64_t key;
		int eval;
		Move move;
		short depth;
		short nodeType;
	};

	const short exact = 0;
	const short lowerBound = 1;
	const short uppperBound = 2;
	
	void storeEvaluation(int depth, int play, int eval, int nodeType, Move move, Board& board);
	int lookUpEvaluation(int depth, int ply, int alpha, int beta, Board& board);
	Move getStoredMove(Board& board);

private:
	entry* entries;
	int size;
};