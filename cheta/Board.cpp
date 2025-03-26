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
	// 1. ������� ��������
	if (!isInsideBoard(toRow, toCol)) {
		return false; // �������� ������ ��� �����
	}
	Piece* piece = board[fromRow][fromCol];
	if (!piece || piece->getColor() != playerColor) {
		return false; // ��� ����� ������ � ��������� ������
	}
	if (board[toRow][toCol] != nullptr) {
		return false; // �������� ������ ������
	}

	// 2. ������ ��������
	int rowDiff = toRow - fromRow;
	int colDiff = toCol - fromCol; // ���������� �������� �������� ��� ����������� �����������

	// 3. �������� ��������������
	if (std::abs(rowDiff) != std::abs(colDiff)) {
		return false; // ��� �� �� ���������
	}

	// 4. �������� �������� ����
	if (rowDiff == 0) { // ��� colDiff == 0
		return false; // ������ ���������� �� �����
	}

	// 5. ������ ��� ����� (MAN)
	if (!piece->isKing()) {
		// ����� ����� ������ �� 1 ������
		if (std::abs(rowDiff) != 1) {
			return false;
		}
		// ����� ����� ������ ������
		// �������, ��� ����� ����� (������� 0..2), ������ ������ (5..7)
		// ����� ����� �� ������� ������ ������ (rowDiff == 1)
		// ������ ����� �� ������� ������ ������ (rowDiff == -1)
		if (playerColor == PieceColor::WHITE && rowDiff != 1) {
			return false; // �������� ����������� ��� ����� �����
		}
		if (playerColor == PieceColor::BLACK && rowDiff != -1) {
			return false; // �������� ����������� ��� ������ �����
		}
		// ���� ��� �������� ��� ����� ��������
		return true;
	}
	// 6. ������ ��� ����� (KING)
	else {
		// ����� ����� ������ �� ����� ���������� �� ���������
		// ����������� ���� �� ����������

		// ���������, ��� ���� ��������
		int rowDir = (rowDiff > 0) ? 1 : -1; // ����������� �� ������� (+1 ��� -1)
		int colDir = (colDiff > 0) ? 1 : -1; // ����������� �� �������� (+1 ��� -1)
		int steps = std::abs(rowDiff);       // ���������� �����

		// ��������� ��� ������ ����� ��������� � ��������
		for (int i = 1; i < steps; ++i) {
			int currentRow = fromRow + i * rowDir;
			int currentCol = fromCol + i * colDir;
			if (board[currentRow][currentCol] != nullptr) {
				return false; // ���� ������������
			}
		}
		// ���� ���� ����������, ���� ��������
		return true;
	}
}

bool Board::isJumpPossible(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) const {

	// ������� �������� �������� �����, ���� ���� ��� ���� � ���������� ��������
	if (!isInsideBoard(toRow, toCol)) {
		return false;
	}
	// ��������, ��� ������ ���������� ����� (����� ��� �������)
	if (board[toRow][toCol] != nullptr) {
		return false;
	}


	Piece* piece = board[fromRow][fromCol]; // 'piece' ��� �������� �� nullptr � ���� � isValidMove, �� �� � canJumpFrom
	// ������� ������� �������� � ����� ��� ����������, ���� ������� ����� ���������� ��������
	if (!piece || piece->getColor() != playerColor) {
		return false;
	}


	int rowDiff = toRow - fromRow;
	int colDiff = toCol - fromCol;

	//�������� ���� �����.
	if (piece->isKing()) {
		if (std::abs(rowDiff) != std::abs(colDiff) || std::abs(rowDiff) < 2) { // ������ ������ ���� ���� �� ����� 1 ������
			return false; //������ �� �� ��������� ��� ������� ��������
		}
		int rowDir = (rowDiff > 0) ? 1 : -1;
		int colDir = (colDiff > 0) ? 1 : -1;

		int enemyCount = 0; //������� ��������� �����.
		int jumpedPieceRow = -1, jumpedPieceCol = -1; // ���������� ������������ ��������� �����

		//�������� ������ �� ���� �����
		for (int i = 1; i < std::abs(rowDiff); i++)
		{
			int currentRow = fromRow + i * rowDir;
			int currentCol = fromCol + i * colDir;

			// ��� �������� �� �����, �.�. currentRow/currentCol ������ ����� ����� from � to,
			// � from � to ��� ��������� (��� ����� ��������� � canJumpFrom)
			// if (!isInsideBoard(currentRow, currentCol)) return false; // ������ ��������

			Piece* intermediatePiece = board[currentRow][currentCol];
			if (intermediatePiece)
			{
				if (intermediatePiece->getColor() == playerColor)
				{
					return false;  //������ ������� ����� ����
				}
				else // ��������� �����
				{
					if (enemyCount > 0) {
						return false; // ������ ���� ������ ����� ����� �� ���� ������ (�� ������� ������)
					}
					enemyCount++;
					jumpedPieceRow = currentRow; // ���������� ���������� ������ �����
					jumpedPieceCol = currentCol;
				}
			}
		}
		// ������ ��������, ���� �� ���� ���� ����� ���� ��������� ����� � ��� ��������� ������ �����
		return enemyCount == 1;
	}
	else // ��� ������� �����
	{
		if (std::abs(rowDiff) != 2 || std::abs(colDiff) != 2) {
			return false; // ������� ����� ����� ������� ������ ����� ���� ������ �� ���������
		}

		int jumpedRow = fromRow + rowDiff / 2; // ����� �������� ������ �������
		int jumpedCol = fromCol + colDiff / 2; // ����� �������� ������ �������

		// *** �������� �������� ***
		// ��������, ��� ���������� ��������� ����� � �������� �����
		if (!isInsideBoard(jumpedRow, jumpedCol)) {
			return false; // ���������� ����� ����� �� ��������� �����
		}

		Piece* jumpedPiece = board[jumpedRow][jumpedCol];

		// ��������, ��� ������ ��������� (����� ��������� �����)
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
	// ������� �������� �� ������������� ����� �����, ����� �������� ������������� nullptr
	if (!piece || piece->getColor() != playerColor) {
		return false;
	}


	if (piece->isKing())
	{
		//��������� ��� ������������ ����������� ��� �����.
		for (int dr = -1; dr <= 1; dr += 2)
		{
			for (int dc = -1; dc <= 1; dc += 2)
			{
				// ���� �� ���������, ���� �� ������ �� �����
				for (int dist = 1; ; ++dist) {
					int checkRow = row + dist * dr;
					int checkCol = col + dist * dc;

					// ���� ����� �� ������� ����� � ���� �����������, ����������
					if (!isInsideBoard(checkRow, checkCol)) break;

					Piece* checkPiece = board[checkRow][checkCol];

					if (checkPiece) { // ���� ���������� �� �����
						if (checkPiece->getColor() != playerColor) { // ���� ��� ����
							// ��������� ��������� ������ �� ������
							int jumpToRow = checkRow + dr;
							int jumpToCol = checkCol + dc;
							// ���� ��������� ������ ������ ����� � �����, �� ������ ��������
							if (isInsideBoard(jumpToRow, jumpToCol) && board[jumpToRow][jumpToCol] == nullptr) {
								// ��������, ��� �� ��� ������������ ���� �� ���� �� ����� �������
								// (isJumpPossible ������ ��� ���������)
								if (isJumpPossible(row, col, jumpToRow, jumpToCol, playerColor)) {
									return true;
								}
								// ���� ������ ����� ����� ����� ���������� (��������, ������ ����� ���� �����),
								// �� � ������ �� ���� ��������� ������ ��� ������
								break;
							}
							else {
								// ���� ������ �� ������ ������ ��� ��� �����, ������ �� ������
								break;
							}
						}
						else {
							// ���� ���������� �� ���� �����, ������ �� ���� ��������� �� ������
							break;
						}
					}
					// ���� ������ �����, ���������� ���� �� ��������� (��� �����)
				}
			}
		}
	}
	else // ������� �����
	{
		// ��������� ������ 4 ��������� ����������� ������ ��� ������� �����
		for (int dRow = -2; dRow <= 2; dRow += 4) { // dRow = -2, 2
			for (int dCol = -2; dCol <= 2; dCol += 4) { // dCol = -2, 2
				int newRow = row + dRow;
				int newCol = col + dCol;

				// *** ��������� �������� ***
				if (!isInsideBoard(newRow, newCol)) {
					continue; // ����������, ���� �������� ����� ������ ��� �����
				}

				// ������ �������� isJumpPossible ������ � ��������� newRow, newCol
				if (isJumpPossible(row, col, newRow, newCol, playerColor)) {
					return true;
				}
			}
		}
	}
	return false; // ������� � ���� ������ ���
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

// ��� ������� ������ ������� �����, ��� ��� ��� ���������� ��� ������.
void Board::removePiece(int row, int col) {
	if (isInsideBoard(row, col)) {
		delete board[row][col]; // ��������� - ������� ������ �����
		board[row][col] = nullptr;
	}
}

void Board::setPiece(int row, int col, Piece* piece) {
	if (isInsideBoard(row, col)) {
		// delete board[row][col]; // �����������! ������!
		board[row][col] = piece;
	}
}

// ������ ������������� ��������� � ������ � nullptr.
// �� ������� ������ �����, ������� ��� ���.
void Board::clearPiece(int row, int col) {
	if (isInsideBoard(row, col)) {
		// delete board[row][col]; // �����������! ������!
		board[row][col] = nullptr;
	}
}

// ��� ����� ����������� ���������� ��� Board, ����� �������� ������ �� ���������� ����� � ����� ����
Board::~Board() {
	for (int i = 0; i < boardSize; ++i) {
		for (int j = 0; j < boardSize; ++j) {
			delete board[i][j]; // ������� ��� ���������� Piece*
			// �� ����� ����������� nullptr �����, �.�. ������ board ������������
		}
	}
}