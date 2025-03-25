#include "Game.h"
#include "Player.h"
#include <iostream>

int main() {

    // Создаем игроков
    Player* player1 = new HumanPlayer("Player 1", PieceColor::WHITE);
    Player* player2 = new HumanPlayer("Player 2", PieceColor::BLACK);


    // Создаем игру
    Game game(player1, player2, true);

    // Начинаем игру
    game.start();


    //Чтобы не удалялись player1 и player2, делаем это в деструкторе Game.
    return 0;
}