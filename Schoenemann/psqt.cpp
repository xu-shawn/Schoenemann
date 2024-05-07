#include <algorithm>

#include "psqt.h"


int psqt::getPieceBounus(Board& board, PieceType piece, Color& color)
{
	int bounus = 0;
	Bitboard pieces = board.pieces(piece, color);
	bool isBlacksTurn = board.sideToMove() == Color::BLACK ? false : true;
	std::vector<int> indexs = getSetBitsEfficient(pieces, isBlacksTurn);

	if (piece == PieceType::PAWN)
	{
		for (int index : indexs)
		{
			bounus += pawnTable[index];
		}
	}
	else if (piece == PieceType::KNIGHT)
	{
		for (int index : indexs)
		{
			bounus += knightTable[index];
		}
	}
	else if (piece == PieceType::BISHOP)
	{
		for (int index : indexs)
		{
			bounus += bishopTable[index];
		}
	}
	else if (piece == PieceType::ROOK)
	{
		for (int index : indexs)
		{
			bounus += rookTable[index];
		}
	}
	else if (piece == PieceType::QUEEN)
	{
		for (int index : indexs)
		{
			bounus += queenTable[index];
		}
	}
	else if (piece == PieceType::KING)
	{
		int middleGameBounus = 0;
		int endGameBounus = 0;
		for (int index : indexs)
		{
			middleGameBounus = middlegameKingTable[index];
			endGameBounus = endgameKingTable[index];
		}
		short factor = pieces.count() / 32;
		bounus += factor * middleGameBounus + (1 - factor) * endGameBounus;
	}

	return bounus;
}

std::vector<int> psqt::getSetBitsEfficient(Bitboard& bitboard, bool isBlackTurn) {
	std::vector<int> setBits;
	for (int i = 0; i < 64; ++i) {
		if ((bitboard.getBits() >> i) & 1ULL) {
			isBlackTurn ? setBits.push_back(63 - i) : setBits.push_back(i);
		}
	}
	return setBits;
}