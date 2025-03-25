#include "Game.h"
#include "Player.h"
#include <iostream>

int main() {

    // ������� �������
    Player* player1 = new HumanPlayer("Player 1", PieceColor::WHITE);
    Player* player2 = new HumanPlayer("Player 2", PieceColor::BLACK);


    // ������� ����
    Game game(player1, player2, true);

    // �������� ����
    game.start();


    //����� �� ��������� player1 � player2, ������ ��� � ����������� Game.
    return 0;
}