#ifndef PLAYER_H
#define PLAYER_H

#include "Enums.h"
#include <string>
#include "Board.h"

class Player {
public:
    Player(const std::string& name, PieceColor color);

    const std::string& getName() const;
    PieceColor getColor() const;

    virtual std::pair<std::pair<int, int>, std::pair<int, int>> getMove(const Board& board) = 0; //Добавили параметр.
    virtual ~Player() = default;


protected:
    std::string name;
    PieceColor color;
};

//Пример реализации простого игрока-человека.
class HumanPlayer : public Player {
public:
    HumanPlayer(const std::string& name, PieceColor color);
    std::pair<std::pair<int, int>, std::pair<int, int>> getMove(const Board& board) override;
};

#endif