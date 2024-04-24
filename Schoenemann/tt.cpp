#include <iostream>

#include "tt.h"

void tt::storeEvaluation(int depth, int ply, int score, int nodeType, Move move, Board& board)
{
	entry ent;

	//Get the zobrist key for the position
	std::uint64_t zobristKey = board.zobrist();

	//Set the values of the entry
	ent.key = zobristKey;
	ent.score = score;
	ent.nodeType = nodeType;
	ent.depth = depth;
	ent.move = move;

	//Store the entry in the lookup Table
	entries[zobristKey % ttSize] = ent;
}

int tt::lookUpEvaluation(int depth, int ply, int alpha, int beta, Board& board)
{
	//Get the zobrist key for the position
	std::uint64_t zobristKey = board.zobrist();
	
	//Get the entry
	entry ent = entries[zobristKey % ttSize];

	if (ent.key == zobristKey)
	{
		if (ent.depth >= depth)
		{
			int score = ent.score;
			if (ent.nodeType == exact)
			{
				return score;
			}
			if (ent.nodeType == uppperBound && score <= alpha)
			{
				return score;
			}
			if (ent.nodeType == lowerBound && score >= beta)
			{
				return score;
			}
		}
	}

	//No transposition was found
	return lookupFaild;
}

tt::entry tt::getEntry(Board& board)
{
	//Returns an entry based on the board
	return entries[board.zobrist() % ttSize];
}

void tt::clear()
{
	//Clears every element in the array
	for (int i = 0; i < ttSize; i++)
	{
		entries[i] = entry{ };
	}
}


void tt::init(int size)
{
	//Maybe the tranposition table is already initialized so we clear it
	clear();

	//The entry size
	int entrySize = sizeof(entry);

	//Calculate the size in bytes
	long long sizeInBytes = static_cast<long long>(size) * 1024 * 1024;

	//Calculate the transposition table size
	ttSize = sizeInBytes / entrySize;

	//Initialize the tranpsosition table
	entries = new entry[ttSize];
}
