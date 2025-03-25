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

	void start();          // Начать игру
	void reset();
	bool isGameOver() const;  // Проверить, закончилась ли игра
	GameState getGameState() const;
	PieceColor getCurrentPlayerColor() const;

	const Board& getBoard() const { return board; } //Для отрисовки.

private:
	Board board;
	std::vector<Player*> players; //Используем вектор, чтобы было проще, если игроков больше двух
	int currentPlayerIndex;
	GameState gameState;
	bool whiteStarts;
	bool isBoardFlipped() const; //Добавили метод для проверки, нужно ли перевернуть доску

	void switchPlayer();
	bool makePlayerMove();
	bool checkGameEnd();
	void printBoard() const; // Добавили отдельный метод для печати доски
};

#endif