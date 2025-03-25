#include "Piece.h"

Piece::Piece(PieceColor color, PieceType type) : color(color), type(type) {}

PieceColor Piece::getColor() const {
    return color;
}

PieceType Piece::getType() const {
    return type;
}

void Piece::makeKing() {
    type = PieceType::KING;
}

bool Piece::isKing() const {
    return type == PieceType::KING;
}