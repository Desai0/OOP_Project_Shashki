#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Player.h"
#include "Enums.h"
#include <vector>

class Game {
public:
	Game(Player* player1, Player* player2, bool whiteStarts = true);
	~Game();

	void start();          // ������ ����
	void reset();
	bool isGameOver() const;  // ���������, ����������� �� ����
	GameState getGameState() const;
	PieceColor getCurrentPlayerColor() const;

	const Board& getBoard() const { return board; } //��� ���������.

private:
	Board board;
	std::vector<Player*> players; //���������� ������, ����� ���� �����, ���� ������� ������ ����
	int currentPlayerIndex;
	GameState gameState;
	bool whiteStarts;
	bool isBoardFlipped() const; //�������� ����� ��� ��������, ����� �� ����������� �����

	void switchPlayer();
	bool makePlayerMove();
	bool checkGameEnd();
	void printBoard() const; // �������� ��������� ����� ��� ������ �����
};

#endif