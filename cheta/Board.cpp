#include "Board.h"
#include <stdexcept>
#include <algorithm>

Board::Board() : board(boardSize, std::vector<Piece*>(boardSize, nullptr)) {
	initialize();
}

void Board::initialize() {
	// Очищаем доску
	for (int i = 0; i < boardSize; ++i) {
		for (int j = 0; j < boardSize; ++j) {
			clearPiece(i, j);
		}
	}

	// Расставляем белые шашки
	for (int row = 0; row < 3; ++row) {
		for (int col = (row % 2 == 0) ? 1 : 0; col < boardSize; col += 2) {
			board[row][col] = new Piece(PieceColor::WHITE);
		}
	}

	// Расставляем черные шашки
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
		return false;  // Нет шашки или шашка не того цвета
	}

	if (board[toRow][toCol] != nullptr) {
		return false; // Клетка назначения занята
	}

	//Проверка на обязательные прыжки.
	if (hasRequiredJumps(playerColor) && !isJumpPossible(fromRow, fromCol, toRow, toCol, playerColor))
	{
		return false;
	}


	//Обычный ход или прыжок
	return isJumpPossible(fromRow, fromCol, toRow, toCol, playerColor) || isRegularMovePossible(fromRow, fromCol, toRow, toCol, playerColor);

}


bool Board::makeMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) {

	if (!isValidMove(fromRow, fromCol, toRow, toCol, playerColor)) {
		return false;
	}

	Piece* piece = board[fromRow][fromCol];
	board[toRow][toCol] = piece;
	board[fromRow][fromCol] = nullptr;


	// Обработка битья
	if (std::abs(toRow - fromRow) == 2 || std::abs(toRow - fromRow) == 4) {
		int jumpedRow = (fromRow + toRow) / 2;
		int jumpedCol = (fromCol + toCol) / 2;
		delete board[jumpedRow][jumpedCol];
		board[jumpedRow][jumpedCol] = nullptr;

		//Удаление для прыжков через несколько шашек.
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


	// Превращение в дамку
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
	// 1. Базовые проверки
	if (!isInsideBoard(toRow, toCol)) {
		return false; // Конечная клетка вне доски
	}
	Piece* piece = board[fromRow][fromCol];
	if (!piece || piece->getColor() != playerColor) {
		return false; // Нет шашки игрока в начальной клетке
	}
	if (board[toRow][toCol] != nullptr) {
		return false; // Конечная клетка занята
	}

	// 2. Расчет смещения
	int rowDiff = toRow - fromRow;
	int colDiff = toCol - fromCol; // Используем знаковое смещение для определения направления

	// 3. Проверка диагональности
	if (std::abs(rowDiff) != std::abs(colDiff)) {
		return false; // Ход не по диагонали
	}

	// 4. Проверка нулевого хода
	if (rowDiff == 0) { // Или colDiff == 0
		return false; // Нельзя оставаться на месте
	}

	// 5. Логика для пешки (MAN)
	if (!piece->isKing()) {
		// Пешка ходит только на 1 клетку
		if (std::abs(rowDiff) != 1) {
			return false;
		}
		// Пешка ходит только вперед
		// Считаем, что белые внизу (индексы 0..2), черные вверху (5..7)
		// Белые ходят на бОльший индекс строки (rowDiff == 1)
		// Черные ходят на мЕньший индекс строки (rowDiff == -1)
		if (playerColor == PieceColor::WHITE && rowDiff != 1) {
			return false; // Неверное направление для белой пешки
		}
		if (playerColor == PieceColor::BLACK && rowDiff != -1) {
			return false; // Неверное направление для черной пешки
		}
		// Если все проверки для пешки пройдены
		return true;
	}
	// 6. Логика для дамки (KING)
	else {
		// Дамка может ходить на любое расстояние по диагонали
		// Направление хода не ограничено

		// Проверяем, что путь свободен
		int rowDir = (rowDiff > 0) ? 1 : -1; // Направление по строкам (+1 или -1)
		int colDir = (colDiff > 0) ? 1 : -1; // Направление по столбцам (+1 или -1)
		int steps = std::abs(rowDiff);       // Количество шагов

		// Проверяем все клетки МЕЖДУ начальной и конечной
		for (int i = 1; i < steps; ++i) {
			int currentRow = fromRow + i * rowDir;
			int currentCol = fromCol + i * colDir;
			if (board[currentRow][currentCol] != nullptr) {
				return false; // Путь заблокирован
			}
		}
		// Если цикл завершился, путь свободен
		return true;
	}
}

bool Board::isJumpPossible(int fromRow, int fromCol, int toRow, int toCol, PieceColor playerColor) const {

	// Добавим проверку конечной точки, даже если она есть в вызывающих функциях
	if (!isInsideBoard(toRow, toCol)) {
		return false;
	}
	// Проверка, что клетка назначения пуста (важно для прыжков)
	if (board[toRow][toCol] != nullptr) {
		return false;
	}


	Piece* piece = board[fromRow][fromCol]; // 'piece' уже проверен на nullptr и цвет в isValidMove, но не в canJumpFrom
	// Добавим базовые проверки и здесь для надежности, если функция будет вызываться напрямую
	if (!piece || piece->getColor() != playerColor) {
		return false;
	}


	int rowDiff = toRow - fromRow;
	int colDiff = toCol - fromCol;

	//Проверка хода дамки.
	if (piece->isKing()) {
		if (std::abs(rowDiff) != std::abs(colDiff) || std::abs(rowDiff) < 2) { // Прыжок должен быть хотя бы через 1 клетку
			return false; //Прыжок не по диагонали или слишком короткий
		}
		int rowDir = (rowDiff > 0) ? 1 : -1;
		int colDir = (colDiff > 0) ? 1 : -1;

		int enemyCount = 0; //Счетчик вражеских шашек.
		int jumpedPieceRow = -1, jumpedPieceCol = -1; // Координаты ЕДИНСТВЕННОЙ сбиваемой шашки

		//Проверка клеток на пути дамки
		for (int i = 1; i < std::abs(rowDiff); i++)
		{
			int currentRow = fromRow + i * rowDir;
			int currentCol = fromCol + i * colDir;

			// Эта проверка не нужна, т.к. currentRow/currentCol всегда будут между from и to,
			// а from и to уже проверены (или будут проверены в canJumpFrom)
			// if (!isInsideBoard(currentRow, currentCol)) return false; // Лишняя проверка

			Piece* intermediatePiece = board[currentRow][currentCol];
			if (intermediatePiece)
			{
				if (intermediatePiece->getColor() == playerColor)
				{
					return false;  //Нельзя прыгать через свои
				}
				else // Вражеская шашка
				{
					if (enemyCount > 0) {
						return false; // Нельзя бить больше одной шашки за один прыжок (по русским шашкам)
					}
					enemyCount++;
					jumpedPieceRow = currentRow; // Запоминаем координаты сбитой шашки
					jumpedPieceCol = currentCol;
				}
			}
		}
		// Прыжок возможен, если на пути была ровно одна вражеская шашка и все остальные клетки пусты
		return enemyCount == 1;
	}
	else // Ход обычной шашки
	{
		if (std::abs(rowDiff) != 2 || std::abs(colDiff) != 2) {
			return false; // Обычные шашки могут прыгать только через одну клетку по диагонали
		}

		int jumpedRow = fromRow + rowDiff / 2; // Более надежный способ расчета
		int jumpedCol = fromCol + colDiff / 2; // Более надежный способ расчета

		// *** КЛЮЧЕВАЯ ПРОВЕРКА ***
		// Убедимся, что координаты сбиваемой шашки в пределах доски
		if (!isInsideBoard(jumpedRow, jumpedCol)) {
			return false; // Невозможно сбить шашку за пределами доски
		}

		Piece* jumpedPiece = board[jumpedRow][jumpedCol];

		// Проверка, что прыжок корректен (через вражескую шашку)
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

	// Проверка возможных прыжков
	if (!requiredJumps || canJumpFrom(row, col, playerColor)) //Если есть обязательные прыжки, проверяем только их
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

		// Для дамки добавляем все возможные прыжки
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

	// Если нет обязательных прыжков, проверяем обычные ходы
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
	// Добавим проверку на существование шашки здесь, чтобы избежать разыменования nullptr
	if (!piece || piece->getColor() != playerColor) {
		return false;
	}


	if (piece->isKing())
	{
		//Проверяем все диагональные направления для дамок.
		for (int dr = -1; dr <= 1; dr += 2)
		{
			for (int dc = -1; dc <= 1; dc += 2)
			{
				// Идем по диагонали, пока не выйдем за доску
				for (int dist = 1; ; ++dist) {
					int checkRow = row + dist * dr;
					int checkCol = col + dist * dc;

					// Если вышли за пределы доски в этом направлении, прекращаем
					if (!isInsideBoard(checkRow, checkCol)) break;

					Piece* checkPiece = board[checkRow][checkCol];

					if (checkPiece) { // Если наткнулись на шашку
						if (checkPiece->getColor() != playerColor) { // Если это враг
							// Проверяем следующую клетку за врагом
							int jumpToRow = checkRow + dr;
							int jumpToCol = checkCol + dc;
							// Если следующая клетка внутри доски и пуста, то прыжок возможен
							if (isInsideBoard(jumpToRow, jumpToCol) && board[jumpToRow][jumpToCol] == nullptr) {
								// Проверим, был ли это единственный враг на пути до этого момента
								// (isJumpPossible теперь это проверяет)
								if (isJumpPossible(row, col, jumpToRow, jumpToCol, playerColor)) {
									return true;
								}
								// Если прыжок через этого врага невозможен (например, дальше стоит своя шашка),
								// то и дальше по этой диагонали искать нет смысла
								break;
							}
							else {
								// Если клетка за врагом занята или вне доски, дальше не пройти
								break;
							}
						}
						else {
							// Если наткнулись на свою шашку, дальше по этой диагонали не пройти
							break;
						}
					}
					// Если клетка пуста, продолжаем идти по диагонали (для дамки)
				}
			}
		}
	}
	else // Обычная шашка
	{
		// Проверяем только 4 возможных направления прыжка для обычной шашки
		for (int dRow = -2; dRow <= 2; dRow += 4) { // dRow = -2, 2
			for (int dCol = -2; dCol <= 2; dCol += 4) { // dCol = -2, 2
				int newRow = row + dRow;
				int newCol = col + dCol;

				// *** ДОБАВЛЕНА ПРОВЕРКА ***
				if (!isInsideBoard(newRow, newCol)) {
					continue; // Пропускаем, если конечная точка прыжка вне доски
				}

				// Теперь вызываем isJumpPossible только с валидными newRow, newCol
				if (isJumpPossible(row, col, newRow, newCol, playerColor)) {
					return true;
				}
			}
		}
	}
	return false; // Прыжков с этой клетки нет
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

// Эта функция ДОЛЖНА удалять шашку, так как она вызывается при взятии.
void Board::removePiece(int row, int col) {
	if (isInsideBoard(row, col)) {
		delete board[row][col]; // Правильно - удаляем сбитую шашку
		board[row][col] = nullptr;
	}
}

void Board::setPiece(int row, int col, Piece* piece) {
	if (isInsideBoard(row, col)) {
		// delete board[row][col]; // НЕПРАВИЛЬНО! УБРАТЬ!
		board[row][col] = piece;
	}
}

// Просто устанавливает указатель в клетке в nullptr.
// НЕ удаляет объект шашки, который там был.
void Board::clearPiece(int row, int col) {
	if (isInsideBoard(row, col)) {
		// delete board[row][col]; // НЕПРАВИЛЬНО! УБРАТЬ!
		board[row][col] = nullptr;
	}
}

// Вам также понадобится деструктор для Board, чтобы очистить память от оставшихся шашек в конце игры
Board::~Board() {
	for (int i = 0; i < boardSize; ++i) {
		for (int j = 0; j < boardSize; ++j) {
			delete board[i][j]; // Удаляем все оставшиеся Piece*
			// Не нужно присваивать nullptr здесь, т.к. объект board уничтожается
		}
	}
}