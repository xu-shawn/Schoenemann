#pragma once
#include "movegen/chess.hpp"

using namespace chess;

class searcher {
public:

	const short infinity = 32767;
	int count_nodes = 0;
	int transpositions = 0;
	chess::Move bestMove = chess::Move::NULL_MOVE;
	bool shouldStop = false;
	bool isNormalSearch = true;
	int timeForMove = 0;

	int getNodes();
	void setNodes(int newNodes);
	int search(int depth, int alpha, int beta, int ply, Board& board);
	chess::Move& getBestMove();
	int quiescenceSearch(int alpha, int beta, int ply, Board& board);
	void iterativeDeepening(Board& board);
	int getTranspositions();
	int checkQuiescenceSearch(int alpha, int beta, int ply, Board& board);
private:
};
