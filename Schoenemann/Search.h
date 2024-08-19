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
	std::uint64_t repitions[256] = {0};

	bool contains(std::uint64_t key)
	{
		for (size_t i = 0; i < sizeof(repitions) / sizeof(std::uint64_t); i++)
		{
			if (key == repitions[i])
			{
				return true;
			}
		}
		return false;
	}

	void storeKey(std::uint64_t key)
	{
		repitions[index] = key;
		index++;
		if (index == 255)
		{
			index = 0;
		}
	}

	int nodes = 0;

	int pvs(int alpha, int beta, int depth, int ply, Board& board);
	int qs(int alpha, int beta, Board& board, int ply);
	bool see(Board& board, Color color, Move move, int cutoff);
	Bitboard getAttackes(Square square, Bitboard occ, Board& board, Color color);
	Bitboard getXRayPieceMap(Color color, Square square, Bitboard occ, Board& board);
	PieceType getLeastValuableAttacker(Board& board, Square square, Bitboard occ, Color color);
	void iterativeDeepening(Board& board);
private:
};
