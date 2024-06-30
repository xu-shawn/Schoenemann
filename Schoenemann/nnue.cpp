#include "nnue.h"

int Popsquare(uint64_t& number) {
	const int place = std::countr_zero(number);
	number &= (number - 1);
	return place;
}

uint8_t Mirror(const uint8_t sq) {
	return sq ^ 0b111000;
}

int NNEvaluate(const Board& board) {

	// Initialize arrays
	std::array<int16_t, HiddenSize> hiddenWhite = std::array<int16_t, HiddenSize>();
	std::array<int16_t, HiddenSize> hiddenBlack = std::array<int16_t, HiddenSize>();
	for (int i = 0; i < HiddenSize; i++) hiddenWhite[i] = (Network->FeatureBias[i]);
	for (int i = 0; i < HiddenSize; i++) hiddenBlack[i] = (Network->FeatureBias[i]);

	// Iterate through inputs

	Bitboard occupancy = board.occ();
	for (uint8_t _ = 0; _ < 63; _++) {
		uint64_t bits = occupancy.getBits();
		const int sq = Popsquare(bits);
		Piece piece = board.at(sq);
		const int pieceType = piece.type();
		Color pieceColor = piece.color();
		const int colorOffset = 64 * 6;

		// Turn on the right inputs
		const int whiteActivationIndex = (pieceColor == Color::WHITE ? 0 : colorOffset) + (pieceType - 1) * 64 + sq;
		const int blackActivationIndex = (pieceColor == Color::BLACK ? 0 : colorOffset) + (pieceType - 1) * 64 + Mirror(sq);
		for (int i = 0; i < HiddenSize; i++)
		{
			hiddenWhite[i] += Network->FeatureWeights[whiteActivationIndex][i];
		}

		for (int i = 0; i < HiddenSize; i++)
		{
			hiddenBlack[i] += Network->FeatureWeights[blackActivationIndex][i];
		}
	}

	// Flip
	std::array<int16_t, HiddenSize>& hiddenFriendly = ((board.sideToMove() == Color::WHITE) ? hiddenWhite : hiddenBlack);
	std::array<int16_t, HiddenSize>& hiddenOpponent = ((board.sideToMove() == Color::WHITE) ? hiddenBlack : hiddenWhite);
	int32_t output = 0;

	// Calculate output
	for (int i = 0; i < HiddenSize; i++)
	{
		output += CReLU(hiddenFriendly[i]) * Network->OutputWeights[i];
	}

	for (int i = 0; i < HiddenSize; i++)
	{
		output += CReLU(hiddenOpponent[i]) * Network->OutputWeights[i + HiddenSize];
	}
	const int scale = 400;
	const int qab = 255 * 64;
	output = (output + Network->OutputBias) * scale / qab;

	return output;

}

void LoadNetwork() {
	Network = new NetworkRepresentation;
	std::ifstream file("C:\\GitHub\\Schoenemann\\Schoenemann\\net.bin", std::ios::binary);
	file.read((char*)Network, sizeof(NetworkRepresentation));
}