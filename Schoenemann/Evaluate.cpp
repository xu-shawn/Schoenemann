#include "evaluate.h"

int evaluate(Board& board) {
    int evaluation = 0;

    evaluation = count_material(board, Color::WHITE) - count_material(board, Color::BLACK);

    evaluation += getMobility(board, Color::WHITE) - getMobility(board, Color::BLACK);

    int perspective;

    if (board.sideToMove() == Color::WHITE)
    {
        perspective = 1;
    }
    else
    {
        perspective = -1;
    }

    return evaluation * perspective;
}

int getMobility(Board& borad, Color color)
{
    int mobility = 0;
    for (size_t i = 0; i < 63; i++)
    {
        if (borad.isAttacked(i, color))
        {
            mobility += 5;
        }
    }
    return mobility;
}

int count_material(Board& board, Color color) {
    int material = 0;
    psqt bouns;
    //Pawn
    short amountPawn = count_amount(board, PieceType::PAWN, color);
    int valuePawn = pawnValue + bouns.getPieceBounus(board, PieceType::PAWN, color);
    material += amountPawn * valuePawn;
    //std::cout << "color " << color << " pawnvalue " << count_amount(board, PieceType::PAWN, color) * (pawnValue + bouns.getPieceBounus(board, PieceType::PAWN, color)) << " amount " << count_amount(board, PieceType::PAWN, color) << " bounus " << bouns.getPieceBounus(board, PieceType::PAWN, color) << std::endl;
    
    short amountKnight = count_amount(board, PieceType::KNIGHT, color);
    int valueKnight = knightValue + bouns.getPieceBounus(board, PieceType::KNIGHT, color);
    material += amountKnight * valueKnight;


    short bishopAmount = count_amount(board, PieceType::BISHOP, color);

    if (bishopAmount >= 2)
    {
        material += 50;
    }

    int valueBishop = bishopValue + bouns.getPieceBounus(board, PieceType::BISHOP, color);

    material += bishopAmount * valueBishop;

    short amountRook = count_amount(board, PieceType::ROOK, color);
    int valueRook = rookValue + bouns.getPieceBounus(board, PieceType::ROOK, color);

    material += amountRook * valueRook;

    short amountQueen = count_amount(board, PieceType::QUEEN, color);
    int valueQueen = queenValue + bouns.getPieceBounus(board, PieceType::QUEEN, color);

    material += amountQueen * valueQueen;
    
    //material += bouns.getPieceBounus(board, PieceType::KING, color);
    return material;
}

int count_amount(Board& board, PieceType type, Color color) {
    return board.pieces(type, color).count();
}

int nnueEvaluation(Board& board)
{
    return NNEvaluate(board);
}