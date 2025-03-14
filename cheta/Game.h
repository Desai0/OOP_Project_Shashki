#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Player.h"

class Game {
public:
    Game();
    void start();
private:
    Board board;
    Player player1;
    Player player2;
    void processTurn(Player& currentPlayer);
    bool isGameOver() const;
};

#endif // GAME_H 