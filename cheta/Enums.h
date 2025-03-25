#ifndef ENUMS_H
#define ENUMS_H

enum class PieceColor {
    NONE,  // ������ ������
    WHITE,
    BLACK
};

enum class PieceType {
    MAN,     // ������� �����
    KING    // �����
};

enum class GameState {
    PLAYING,
    WHITE_WON,
    BLACK_WON,
    DRAW
};

#endif