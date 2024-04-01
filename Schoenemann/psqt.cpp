#include <algorithm>

#include "psqt.h"


int psqt::getPieceBounus(Board& board, PieceType piece, Color& color)
{
	int bounus = 0;
	Bitboard pieces = board.pieces(piece, color);
	if (color == Color::WHITE)
	{
		pieces = reverse(pieces);
	}
	std::vector<int> indexs = getSetBitsEfficient(pieces);

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
	
	return bounus;
}

Bitboard psqt::reverse(Bitboard& bitboard)
{
	std::uint64_t r = bitboard.getBits();
	r = ((r >> 1) & 0x5555555555555555) | ((r & 0x5555555555555555) << 1);
	r = ((r >> 2) & 0x3333333333333333) | ((r & 0x3333333333333333) << 2);
	r = ((r >> 4) & 0x0F0F0F0F0F0F0F0F) | ((r & 0x0F0F0F0F0F0F0F0F) << 4);
	r = ((r >> 8) & 0x00FF00FF00FF00FF) | ((r & 0x00FF00FF00FF00FF) << 8);
	r = ((r >> 16) & 0x0000FFFF0000FFFF) | ((r & 0x0000FFFF0000FFFF) << 16);
	r = (r >> 32) | (r << 32);
	return Bitboard(r);
}

std::vector<int> psqt::getSetBitsEfficient(Bitboard& bitboard) {
	std::vector<int> setBits;
	for (int i = 0; i < 64; ++i) {
		if ((bitboard.getBits() >> i) & 1ULL) {
			setBits.push_back(i);
		}
	}
	return setBits;
}


