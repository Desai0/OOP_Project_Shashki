#include "Game.h"
#include <iostream>

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
    bool flipped = isBoardFlipped(); //  Используем isBoardFlipped()

    for (int i = 0; i < board.getBoardSize(); ++i) {
        for (int j = 0; j < board.getBoardSize(); ++j) {
            int row = flipped ? board.getBoardSize() - 1 - i : i;
            int col = flipped ? board.getBoardSize() - 1 - j : j;

            std::optional<PieceColor> color = board.getPieceColor(row, col);
            if (color == PieceColor::WHITE) {
                if (board.getPiece(row, col)->isKing())
                    std::cout << "W ";
                else
                    std::cout << "w ";
            }
            else if (color == PieceColor::BLACK) {
                if (board.getPiece(row, col)->isKing())
                    std::cout << "B ";
                else
                    std::cout << "b ";
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
}
bool Game::isBoardFlipped() const {
    return !whiteStarts; //  Доска перевернута, если белые НЕ начинают.
}
// ... (остальные методы Game.cpp - без изменений) ...

void Game::start() {

    while (!isGameOver()) {
        // 1. Вывести доску (в консоли или через графический интерфейс)
        std::cout << "Current Board: " << std::endl;
        printBoard(); // Используем printBoard

        // 2. Получить ход от текущего игрока
        std::cout << "Current Player: " << players[currentPlayerIndex]->getName() << std::endl;

        // 3. Сделать ход (если ход некорректен, запросить заново)
        while (!makePlayerMove());

        // 4. Проверить, не закончилась ли игра
        checkGameEnd();

        // 5. Сменить игрока
        switchPlayer();
    }
    //Выводим результат
    if (gameState == GameState::WHITE_WON)
    {
        std::cout << "White won!" << std::endl;
    }
    else if (gameState == GameState::BLACK_WON)
    {
        std::cout << "Black won!" << std::endl;
    }
    else
    {
        std::cout << "Draw!" << std::endl;
    }

}

// ... (остальные методы Game) ...
bool Game::isGameOver() const {
    return gameState != GameState::PLAYING;
}

PieceColor Game::getCurrentPlayerColor() const {
    return players[currentPlayerIndex]->getColor();
}

void Game::switchPlayer() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
}

bool Game::makePlayerMove()
{
    std::pair<std::pair<int, int>, std::pair<int, int>> move = players[currentPlayerIndex]->getMove(board, isBoardFlipped()); // Передаем isBoardFlipped()
    int fromRow = move.first.first;
    int fromCol = move.first.second;
    int toRow = move.second.first;
    int toCol = move.second.second;

    return board.makeMove(fromRow, fromCol, toRow, toCol, getCurrentPlayerColor());
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