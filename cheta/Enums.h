#ifndef ENUMS_H
#define ENUMS_H

enum class PieceColor {
    NONE,  // Пустая клетка
    WHITE,
    BLACK
};

enum class PieceType {
    MAN,     // Простая шашка
    KING    // Дамка
};

enum class GameState {
    PLAYING,
    WHITE_WON,
    BLACK_WON,
    DRAW
};

#endif