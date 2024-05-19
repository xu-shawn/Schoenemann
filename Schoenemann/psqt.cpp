#include <algorithm>

#include "psqt.h"


int psqt::getPieceBounus(Board& board, PieceType piece, Color& color)
{
	int bounus = 0;
	Bitboard pieces = board.pieces(piece, color);

	std::vector<int> indexs = getSetBitsEfficient(pieces, color);

	if (piece == PieceType::PAWN)
	{
		for (int index : indexs)
		{
			//std::cout << " index " << index << " bounus " << bounus << std::endl;
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
		short factor = pieces.count() / 16;
		bounus += factor * middleGameBounus + (1 - factor) * endGameBounus;
	}

	return bounus;
}

std::vector<int> psqt::getSetBitsEfficient(Bitboard& bitboard, Color color) {
	std::vector<int> setBits;
	if (color == Color::WHITE)
	{
		for (int i = 0; i < 64; ++i) {
			if ((bitboard.getBits() >> i) & 1ULL) {
				setBits.push_back(63 - i);
			}
		}
	}
	else
	{
		for (int i = 63; i >= 0; --i) {
			if ((bitboard.getBits() >> i) & 1ULL) {
				setBits.push_back(i);
			}
		}
	}
	return setBits;
}
