#ifndef PIECE_H
#define PIECE_H

#include "Enums.h"

class Piece {
public:
    Piece(PieceColor color, PieceType type = PieceType::MAN);

    PieceColor getColor() const;
    PieceType getType() const;
    void makeKing();
    bool isKing() const;

private:
    PieceColor color;
    PieceType type;
};

#endif