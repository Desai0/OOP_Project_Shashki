#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include <vector>
#include <optional>
#include "Enums.h"

class Board {
public:
	Board();
	~Board();

	void initialize(); // Расстановка шашек в начальное положение
	Piece* getPiece(int row, int col) const; // Получить шашку по координатам.  Возвращает nullptr, если клетка пуста.
	std::optional<PieceColor> getPieceColor(int row, int col) const;

	bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) const;
	bool makeMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor);  // Вернуть true, если ход успешен, false в противном случае

	std::vector<std::pair<int, int>> getPossibleMoves(int row, int col, PieceColor playerColor) const;
	std::vector<std::pair<int, int>> getRequiredJumps(PieceColor playerColor) const;
	bool hasRequiredJumps(PieceColor playerColor) const;

	void removePiece(int row, int col);
	void setPiece(int row, int col, Piece* piece);
	void clearPiece(int row, int col);

	int getBoardSize() const { return boardSize; }
	bool isJumpPossible(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) const;
	bool isInsideBoard(int row, int col) const;
	bool canJumpFrom(int row, int col, PieceColor playerColor) const;

private:
	std::vector<std::vector<Piece*>> board;
	static const int boardSize = 8;  // Размер доски
	
	
	
	bool isRegularMovePossible(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) const;

	
};

#endif