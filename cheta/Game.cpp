#include "Game.h"
#include <iostream>
#include <cstdlib>

Game::Game(Player* player1, Player* player2, bool whiteStarts) :
    currentPlayerIndex(0), gameState(GameState::PLAYING), whiteStarts(whiteStarts)
{
    players.push_back(player1);
    players.push_back(player2);

    if (!whiteStarts) {
        currentPlayerIndex = 1; // Черные начинают, если whiteStarts == false
    }
}

Game::~Game()
{
    for (Player* player : players) {
        delete player;
    }
}

void Game::printBoard() const {
    int size = board.getBoardSize();
    // Печатаем номера столбцов
    std::cout << "  "; // Отступ для номеров строк
    for (int j = 0; j < size; ++j) {
        std::cout << j << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < size; ++i) {
        std::cout << i << " "; // Печатаем номер строки
        for (int j = 0; j < size; ++j) {
            // Используем i и j напрямую, т.к. они уже соответствуют логике доски
            Piece* piece = board.getPiece(i, j); // Используем getPiece
            if (piece) { // Если клетка не пуста
                PieceColor color = piece->getColor();
                bool isKing = piece->isKing();
                if (color == PieceColor::WHITE) {
                    std::cout << (isKing ? "W " : "w ");
                }
                else { // BLACK
                    std::cout << (isKing ? "B " : "b ");
                }
            }
            else { // Пустая клетка
                // Используем '-' или '.', можно чередовать для эффекта шахматной доски
                std::cout << (((i + j) % 2 == 0) ? ". " : "- "); // Пример для визуального разделения клеток
                // Или просто: std::cout << ". ";
            }
        }
        std::cout << i; // Номер строки справа
        std::cout << std::endl;
    }
    // Печатаем номера столбцов снизу
    std::cout << "  ";
    for (int j = 0; j < size; ++j) {
        std::cout << j << " ";
    }
    std::cout << std::endl;
}


void Game::start() {
    while (!isGameOver()) {

        // --- Очистка консоли ---
#ifdef _WIN32 // Макрос для Windows (обычно определяется компилятором MSVC)
        system("cls");
#endif
        // ------------------------

        // Теперь выводим обновленное состояние
        std::cout << "\nCurrent Board:" << std::endl;
        printBoard(); // Вывод доски после очистки
        std::cout << "Current Player: " << players[currentPlayerIndex]->getName()
            << " (" << (getCurrentPlayerColor() == PieceColor::WHITE ? "White" : "Black") << ")" << std::endl;

        bool moveResult = makePlayerMove(); // Выполняем ход (который запрашивает ввод и т.д.)

        // Если makePlayerMove вернула false (ходов не было), checkGameEnd это обнаружит
        if (!moveResult && !isGameOver()) { // Добавил !isGameOver() на всякий случай
            // Можно добавить сообщение, если нужно
            // std::cout << "No moves were made." << std::endl;
        }

        // Проверяем конец игры ПОСЛЕ попытки хода
        if (checkGameEnd()) {
            break; // Выход из основного цикла игры
        }

        // Переключаем игрока ТОЛЬКО если игра не закончилась
        switchPlayer();

    } // Конец while (!isGameOver())

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    std::cout << "\n--- Game Over ---" << std::endl;
    printBoard(); // Показать финальную доску
    switch (gameState) {
    case GameState::WHITE_WON:
        std::cout << "White won!" << std::endl;
        break;
    case GameState::BLACK_WON:
        std::cout << "Black won!" << std::endl;
        break;
    case GameState::DRAW: // Ничья пока не реализована, но пусть будет
        std::cout << "Draw!" << std::endl;
        break;
    default:
        std::cout << "Game finished unexpectedly." << std::endl;
        break;
    }
}


bool Game::isGameOver() const {
    return gameState != GameState::PLAYING;
}

PieceColor Game::getCurrentPlayerColor() const {
    return players[currentPlayerIndex]->getColor();
}

void Game::switchPlayer() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
}


bool isPositionInList(int r, int c, const std::vector<std::pair<int, int>>& list) {
    return std::find_if(list.begin(), list.end(),
        [r, c](const std::pair<int, int>& pos) { return pos.first == r && pos.second == c; }) != list.end();
}

bool Game::makePlayerMove()
{
    Player* currentPlayer = players[currentPlayerIndex];
    PieceColor playerColor = currentPlayer->getColor();
    bool turnFinished = false; // Флаг: завершил ли игрок свой ход полностью
    int currentPieceRow = -1, currentPieceCol = -1; // Позиция шашки для серии прыжков

    // Основной цикл хода игрока. Повторяется, пока ход не завершен (turnFinished = true).
    while (!turnFinished) {

        int fromRow, fromCol, toRow, toCol;
        bool isContinuationJump = (currentPieceRow != -1);

        // 1. Получение ввода от игрока
        if (isContinuationJump) {
            // Запрос только конечных координат для продолжения прыжка
            fromRow = currentPieceRow;
            fromCol = currentPieceCol;
            std::cout << currentPlayer->getName() << ", continue jump from (" << fromRow << "," << fromCol << ") (enter toRow toCol): ";
            int nextToRow, nextToCol;
            while (!(std::cin >> nextToRow >> nextToCol)) { // Цикл обработки неверного ФОРМАТА ввода
                std::cout << "Invalid input format. Please enter two integers." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Continue jump from (" << fromRow << "," << fromCol << ") (enter toRow toCol): ";
            }
            toRow = nextToRow;
            toCol = nextToCol;
        }
        else {
            // Запрос полного хода (fromRow, fromCol, toRow, toCol)
            // getMove внутри HumanPlayer уже обрабатывает неверный формат и выход за границы
            std::pair<std::pair<int, int>, std::pair<int, int>> move = currentPlayer->getMove(board);
            fromRow = move.first.first;
            fromCol = move.first.second;
            toRow = move.second.first;
            toCol = move.second.second;
        }

        // 2. Валидация хода/прыжка на основе правил игры
        bool isValidAction = false; // Флаг валидности ТЕКУЩЕЙ попытки
        bool isAttemptedJump = std::abs(toRow - fromRow) >= 2;
        auto requiredJumpsInfo = board.getRequiredJumps(playerColor); // Шашки, которые ОБЯЗАНЫ бить
        bool mustJumpGenerally = !requiredJumpsInfo.empty();          // Есть ли вообще обязательные прыжки

        if (isContinuationJump) { // --- Валидация продолжения серии ---
            // Должен быть прыжок, и он должен быть возможен с текущей клетки
            if (isAttemptedJump && board.isJumpPossible(fromRow, fromCol, toRow, toCol, playerColor)) {
                isValidAction = true;
            }
            else {
                std::cout << "Invalid continuation jump. You must make a valid jump from (" << fromRow << "," << fromCol << ")." << std::endl;
                // isValidAction остается false
                // currentPieceRow/Col остаются прежними, цикл while запросит ввод снова
            }
        }
        else if (mustJumpGenerally) { // --- Валидация первого хода при наличии обяз. прыжков ---
            if (!isAttemptedJump) {
                std::cout << "Invalid move: A jump is required." << std::endl;
            }
            else if (!isPositionInList(fromRow, fromCol, requiredJumpsInfo)) {
                std::cout << "Invalid move: You must jump with a piece from specific positions. Required from: ";
                for (const auto& pos : requiredJumpsInfo) std::cout << "(" << pos.first << "," << pos.second << ") ";
                std::cout << std::endl;
            }
            else if (!board.isJumpPossible(fromRow, fromCol, toRow, toCol, playerColor)) {
                std::cout << "Invalid jump destination or path. Please try again." << std::endl;
            }
            else {
                isValidAction = true; // Все условия для обязательного прыжка выполнены
            }
            // Если ни одно из условий не выполнено, isValidAction остается false
        }
        else { // --- Валидация обычного хода или необязательного прыжка ---
            if (board.isValidMove(fromRow, fromCol, toRow, toCol, playerColor)) {
                // isValidMove проверяет всё: цвет, пустоту клетки, направление, обязательные прыжки (их нет)
                isValidAction = true;
            }
            else {
                // Выясняем причину ошибки для более точного сообщения
                if (!board.isInsideBoard(fromRow, fromCol) || !board.isInsideBoard(toRow, toCol))
                    std::cout << "Invalid move: Coordinates out of bounds." << std::endl;
                else if (board.getPieceColor(fromRow, fromCol) != playerColor)
                    std::cout << "Invalid move: No piece of your color at (" << fromRow << "," << fromCol << ")." << std::endl;
                else if (board.getPiece(toRow, toCol) != nullptr)
                    std::cout << "Invalid move: Destination square (" << toRow << "," << toCol << ") is occupied." << std::endl;
                else // Другие причины (не по диагонали, неверное направление для пешки и т.д.)
                    std::cout << "Invalid move logic. Please check rules." << std::endl;
                // isValidAction остается false
            }
        }

        // 3. Выполнение действия, если оно валидно
        if (isValidAction) {
            Piece* movingPiece = board.getPiece(fromRow, fromCol); // Получаем указатель ДО перемещения
            board.setPiece(toRow, toCol, movingPiece);           // Перемещаем указатель в новую клетку
            board.clearPiece(fromRow, fromCol);                  // Очищаем старую клетку (устанавливаем nullptr)
            bool piecePromoted = false;

            if (isAttemptedJump) { // --- Обработка последствий прыжка ---
                // Удаляем сбитую шашку
                int rowDir = (toRow - fromRow > 0) ? 1 : -1;
                int colDir = (toCol - fromCol > 0) ? 1 : -1;
                int jumpedRow = -1, jumpedCol = -1;
                for (int i = 1; i < std::abs(toRow - fromRow); ++i) {
                    int currentRow = fromRow + i * rowDir;
                    int currentCol = fromCol + i * colDir;
                    // Ищем шашку противника на пути
                    if (board.getPiece(currentRow, currentCol) != nullptr && board.getPieceColor(currentRow, currentCol) != playerColor) {
                        jumpedRow = currentRow;
                        jumpedCol = currentCol;
                        break;
                    }
                }
                if (jumpedRow != -1) {
                    board.removePiece(jumpedRow, jumpedCol); // Удаляем объект Piece
                    // std::cout << "Piece captured at (" << jumpedRow << "," << jumpedCol << ")" << std::endl; // Debug message
                }

                // Проверка на превращение в дамку
                if (!movingPiece->isKing() && ((playerColor == PieceColor::WHITE && toRow == board.getBoardSize() - 1) || (playerColor == PieceColor::BLACK && toRow == 0))) {
                    movingPiece->makeKing();
                    piecePromoted = true;
                    std::cout << "Piece promoted to King!" << std::endl;
                }

                // Проверка на возможность/необходимость продолжения прыжков
                if (!piecePromoted && board.canJumpFrom(toRow, toCol, playerColor)) {
                    currentPieceRow = toRow; // Запоминаем позицию для след. прыжка
                    currentPieceCol = toCol;
                    std::cout << "Another jump possible/required from (" << toRow << "," << toCol << ")." << std::endl;
                    printBoard(); // Показываем доску после промежуточного прыжка
                    // turnFinished остается false, внешний цикл while продолжится
                }
                else {
                    turnFinished = true; // Прыжок завершен, серия закончена
                }
            }
            else { // --- Обработка последствий обычного хода ---
                // Проверка на превращение в дамку
                if (!movingPiece->isKing() && ((playerColor == PieceColor::WHITE && toRow == board.getBoardSize() - 1) || (playerColor == PieceColor::BLACK && toRow == 0))) {
                    movingPiece->makeKing();
                    // piecePromoted = true; // Не используется дальше
                    std::cout << "Piece promoted to King!" << std::endl;
                }
                turnFinished = true; // Обычный ход всегда завершает очередь
            }
        }
        // else { // Если isValidAction == false
        //   Ничего не делаем. turnFinished остается false.
        //   Цикл while начнется заново, запрашивая ввод у того же игрока.
        // }

    } // Конец while (!turnFinished)

    // Возвращаем true, т.к. функция завершается только после успешного хода/серии
    return true;
}

bool Game::checkGameEnd()
{
    int whiteCount = 0;
    int blackCount = 0;

    for (int i = 0; i < board.getBoardSize(); ++i)
    {
        for (int j = 0; j < board.getBoardSize(); ++j)
        {
            std::optional<PieceColor> color = board.getPieceColor(i, j);
            if (color == PieceColor::WHITE) whiteCount++;
            else if (color == PieceColor::BLACK) blackCount++;
        }
    }

    //Конец игры, если у одного из игроков не осталось шашек.
    if (whiteCount == 0)
    {
        gameState = GameState::BLACK_WON;
        return true;
    }
    if (blackCount == 0)
    {
        gameState = GameState::WHITE_WON;
        return true;
    }

    //Проверяем, может ли текущий игрок сделать ход.  Если нет, то игра заканчивается.
    bool canMove = false;
    for (int row = 0; row < board.getBoardSize(); ++row)
    {
        for (int col = 0; col < board.getBoardSize(); ++col)
        {
            if (board.getPieceColor(row, col) == getCurrentPlayerColor())
            {
                if (!board.getPossibleMoves(row, col, getCurrentPlayerColor()).empty())
                {
                    canMove = true;
                    break;
                }
            }
        }
        if (canMove) break;
    }

    //Если текущий игрок не может сделать ход, то победил противник.
    if (!canMove)
    {
        if (getCurrentPlayerColor() == PieceColor::WHITE)
        {
            gameState = GameState::BLACK_WON;
        }
        else
        {
            gameState = GameState::WHITE_WON;
        }
        return true;
    }

    return false; //Игра продолжается
}

void Game::reset()
{
    board.initialize();
    currentPlayerIndex = 0;
    gameState = GameState::PLAYING;
}

GameState Game::getGameState() const
{
    return gameState;
}