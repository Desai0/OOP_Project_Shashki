#include "Board.h"
#include <stdexcept>
#include <algorithm>

Board::Board() : board(boardSize, std::vector<Piece*>(boardSize, nullptr)) {
	initialize();
}

void Board::initialize() {
	// ������� �����
	for (int i = 0; i < boardSize; ++i) {
		for (int j = 0; j < boardSize; ++j) {
			clearPiece(i, j);
		}
	}

	// ����������� ����� �����
	for (int row = 0; row < 3; ++row) {
		for (int col = (row % 2 == 0) ? 1 : 0; col < boardSize; col += 2) {
			board[row][col] = new Piece(PieceColor::WHITE);
		}
	}

	// ����������� ������ �����
	for (int row = 5; row < 8; ++row) {
		for (int col = (row % 2 == 0) ? 1 : 0; col < boardSize; col += 2) {
			board[row][col] = new Piece(PieceColor::BLACK);
		}
	}
}

Piece* Board::getPiece(int row, int col) const {
	if (!isInsideBoard(row, col)) {
		return nullptr;
	}
	return board[row][col];
}

std::optional<PieceColor> Board::getPieceColor(int row, int col) const
{
	if (!isInsideBoard(row, col)) {
		return std::nullopt;
	}
	Piece* piece = getPiece(row, col);
	if (!piece) return PieceColor::NONE;
	return piece->getColor();
}

bool Board::isValidMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) const
{
	if (!isInsideBoard(fromRow, fromCol) || !isInsideBoard(toRow, toCol)) {
		return false;
	}

	Piece* piece = board[fromRow][fromCol];
	if (!piece || piece->getColor() != playerColor) {
		return false;  // ��� ����� ��� ����� �� ���� �����
	}

	if (board[toRow][toCol] != nullptr) {
		return false; // ������ ���������� ������
	}

	//�������� �� ������������ ������.
	if (hasRequiredJumps(playerColor) && !isJumpPossible(fromRow, fromCol, toRow, toCol, playerColor))
	{
		return false;
	}


	//������� ��� ��� ������
	return isJumpPossible(fromRow, fromCol, toRow, toCol, playerColor) || isRegularMovePossible(fromRow, fromCol, toRow, toCol, playerColor);

}


bool Board::makeMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) {

	if (!isValidMove(fromRow, fromCol, toRow, toCol, playerColor)) {
		return false;
	}

	Piece* piece = board[fromRow][fromCol];
	board[toRow][toCol] = piece;
	board[fromRow][fromCol] = nullptr;


	// ��������� �����
	if (std::abs(toRow - fromRow) == 2 || std::abs(toRow - fromRow) == 4) {
		int jumpedRow = (fromRow + toRow) / 2;
		int jumpedCol = (fromCol + toCol) / 2;
		delete board[jumpedRow][jumpedCol];
		board[jumpedRow][jumpedCol] = nullptr;

		//�������� ��� ������� ����� ��������� �����.
		if (std::abs(toRow - fromRow) == 4)
		{
			if (toRow > fromRow && toCol > fromCol)
			{
				delete board[fromRow + 1][fromCol + 1];
				board[fromRow + 1][fromCol + 1] = nullptr;
			}
			else if (toRow > fromRow && toCol < fromCol)
			{
				delete board[fromRow + 1][fromCol - 1];
				board[fromRow + 1][fromCol - 1] = nullptr;
			}
			else if (toRow < fromRow && toCol > fromCol)
			{
				delete board[fromRow - 1][fromCol + 1];
				board[fromRow - 1][fromCol + 1] = nullptr;
			}
			else
			{
				delete board[fromRow - 1][fromCol - 1];
				board[fromRow - 1][fromCol - 1] = nullptr;
			}
		}
	}


	// ����������� � �����
	if ((playerColor == PieceColor::WHITE && toRow == boardSize - 1) || (playerColor == PieceColor::BLACK && toRow == 0))
	{
		piece->makeKing();
	}
	return true;
}

bool Board::isInsideBoard(int row, int col) const {
	return row >= 0 && row < boardSize && col >= 0 && col < boardSize;
}


bool Board::isRegularMovePossible(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) const
{
	Piece* piece = board[fromRow][fromCol];
	if (!piece) return false;

	int rowDiff = toRow - fromRow;
	int colDiff = std::abs(toCol - fromCol);

	if (colDiff != 1)
	{
		return false; //������� ���� ������ �� ���� ������ �� ���������
	}


	if (piece->isKing())
	{
		//����� ����� ������ � ����� �����������.
		return std::abs(rowDiff) == 1;
	}
	else
	{
		//������� �����
		if (playerColor == PieceColor::WHITE)
		{
			return rowDiff == 1; //����� ����� �����
		}
		else
		{
			return rowDiff == -1; //������ ����� ����.
		}
	}

}

bool Board::isJumpPossible(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) const {

	Piece* piece = board[fromRow][fromCol];
	if (!piece) return false;

	int rowDiff = toRow - fromRow;
	int colDiff = toCol - fromCol;

	//�������� ���� �����.
	if (piece->isKing()) {
		if (std::abs(rowDiff) != std::abs(colDiff)) {
			return false; //������ �� �� ���������.
		}
		int rowDir = (rowDiff > 0) ? 1 : -1;
		int colDir = (colDiff > 0) ? 1 : -1;

		int enemyCount = 0; //������� ��������� �����.
		int emptyCount = 0;

		//�������� ������ �� ���� �����
		for (int i = 1; i < std::abs(rowDiff); i++)
		{
			int currentRow = fromRow + i * rowDir;
			int currentCol = fromCol + i * colDir;

			Piece* intermediatePiece = board[currentRow][currentCol];
			if (intermediatePiece)
			{
				if (intermediatePiece->getColor() == playerColor)
				{
					return false;  //������ ������� ����� ����
				}
				else
				{
					enemyCount++;
				}
			}
			else
			{
				emptyCount++;
			}
		}
		return enemyCount > 0 && emptyCount >= 0; // ������ ��������, ���� �� ���� ���� ���� �� ���� ��������� �����
	}
	else
	{
		// ��� ������� �����
		if (std::abs(rowDiff) != 2 || std::abs(colDiff) != 2) {
			return false; // ������� ����� ����� ������� ������ ����� ���� ������
		}

		int jumpedRow = (fromRow + toRow) / 2;
		int jumpedCol = (fromCol + toCol) / 2;
		Piece* jumpedPiece = board[jumpedRow][jumpedCol];

		// ��������, ��� ������ ���������
		return jumpedPiece != nullptr && jumpedPiece->getColor() != playerColor;
	}
}


std::vector<std::pair<int, int>> Board::getPossibleMoves(int row, int col, PieceColor playerColor) const
{
	std::vector<std::pair<int, int>> moves;
	if (!isInsideBoard(row, col) || getPieceColor(row, col) != playerColor) {
		return moves;
	}

	bool requiredJumps = hasRequiredJumps(playerColor);

	// �������� ��������� �������
	if (!requiredJumps || canJumpFrom(row, col, playerColor)) //���� ���� ������������ ������, ��������� ������ ��
	{
		for (int dRow = -2; dRow <= 2; dRow += 4) {
			for (int dCol = -2; dCol <= 2; dCol += 4) {
				int newRow = row + dRow;
				int newCol = col + dCol;
				if (isJumpPossible(row, col, newRow, newCol, playerColor)) {
					moves.emplace_back(newRow, newCol);
				}
			}
		}

		// ��� ����� ��������� ��� ��������� ������
		Piece* piece = board[row][col];
		if (piece && piece->isKing())
		{
			for (int dRow = -boardSize; dRow <= boardSize; ++dRow)
			{
				for (int dCol = -boardSize; dCol <= boardSize; ++dCol)
				{
					int newRow = row + dRow;
					int newCol = col + dCol;
					if (isJumpPossible(row, col, newRow, newCol, playerColor))
					{
						moves.emplace_back(newRow, newCol);
					}
				}
			}
		}
	}

	// ���� ��� ������������ �������, ��������� ������� ����
	if (moves.empty() && !requiredJumps) {
		for (int dRow = -1; dRow <= 1; dRow += 2) {
			for (int dCol = -1; dCol <= 1; dCol += 2) {
				int newRow = row + dRow;
				int newCol = col + dCol;
				if (isRegularMovePossible(row, col, newRow, newCol, playerColor)) {
					moves.emplace_back(newRow, newCol);
				}
			}
		}
	}

	return moves;
}

bool Board::canJumpFrom(int row, int col, PieceColor playerColor) const
{
	Piece* piece = board[row][col];
	if (!piece) return false;

	if (piece->isKing())
	{
		//��������� ��� ������������ ����������� ��� �����.
		for (int dr = -1; dr <= 1; dr += 2)
		{
			for (int dc = -1; dc <= 1; dc += 2)
			{
				if (isJumpPossible(row, col, row + 2 * dr, col + 2 * dc, playerColor)) return true;
				if (isJumpPossible(row, col, row + 3 * dr, col + 3 * dc, playerColor)) return true;
				if (isJumpPossible(row, col, row + 4 * dr, col + 4 * dc, playerColor)) return true;
			}
		}

	}
	else
	{
		// ��������� ������ 4 ��������� ����������� ������ ��� ������� �����
		for (int dRow = -2; dRow <= 2; dRow += 4) {
			for (int dCol = -2; dCol <= 2; dCol += 4) {
				if (isJumpPossible(row, col, row + dRow, col + dCol, playerColor)) {
					return true;
				}
			}
		}
	}
	return false;

}


std::vector<std::pair<int, int>> Board::getRequiredJumps(PieceColor playerColor) const {
	std::vector<std::pair<int, int>> jumpPositions;
	for (int row = 0; row < boardSize; ++row)
	{
		for (int col = 0; col < boardSize; ++col)
		{
			if (getPieceColor(row, col) == playerColor && canJumpFrom(row, col, playerColor))
			{
				jumpPositions.emplace_back(row, col);
			}
		}
	}
	return jumpPositions;
}

bool Board::hasRequiredJumps(PieceColor playerColor) const
{
	return !getRequiredJumps(playerColor).empty();
}

void Board::removePiece(int row, int col)
{
	if (isInsideBoard(row, col))
	{
		delete board[row][col];
		board[row][col] = nullptr;
	}
}

void Board::setPiece(int row, int col, Piece* piece)
{
	if (isInsideBoard(row, col))
	{
		delete board[row][col]; // ������� ������ �����, ���� ��� ����
		board[row][col] = piece;
	}
}

void Board::clearPiece(int row, int col)
{
	if (isInsideBoard(row, col))
	{
		delete board[row][col];
		board[row][col] = nullptr;
	}
}

Board Board::clone() const
{
	Board newBoard;
	for (int row = 0; row < boardSize; ++row)
	{
		for (int col = 0; col < boardSize; ++col)
		{
			Piece* piece = board[row][col];
			if (piece)
			{
				newBoard.board[row][col] = new Piece(piece->getColor(), piece->getType());
			}
			else
			{
				newBoard.board[row][col] = nullptr;
			}
		}
	}
	return newBoard;
}