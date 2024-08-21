#pragma once
#include "chess.hpp"

using namespace chess;

class Search {
public:
	const short infinity = 32767;
	Move bestMove = Move::NULL_MOVE;
	bool shouldStop = false;
	bool isNormalSearch = true;
	int timeForMove = 0;
	int index = 0;
	std::uint64_t repetitions[256] = {0};
	const int SEE_PIECE_VALUES[7] = {100, 300, 300, 500, 900, 0, 0};

	bool contains(std::uint64_t key)
	{
		for (size_t i = 0; i < sizeof(repetitions) / sizeof(std::uint64_t); i++)
		{
			if (key == repetitions[i])
			{
				return true;
			}
		}
		return false;
	}

	void storeKey(std::uint64_t key)
	{
		repetitions[index] = key;
		index++;
		if (index == 255)
		{
			index = 0;
		}
	}

	int nodes = 0;

	int pvs(int alpha, int beta, int depth, int ply, Board& board);
	int qs(int alpha, int beta, Board& board, int ply);
	bool see(const Board &board, Move &move, int threshold = 0);
	PieceType getLeastValuableAttacker(const Board &board, Bitboard &occ, Bitboard attackers, Color color);
	int getPieceValue(const Board &board, Move &move);
	void iterativeDeepening(Board& board);
};
