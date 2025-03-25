#include "Game.h"
#include <iostream>

Game::Game(Player* player1, Player* player2, bool whiteStarts) :
    currentPlayerIndex(0), gameState(GameState::PLAYING), whiteStarts(whiteStarts)
{
    players.push_back(player1);
    players.push_back(player2);

    if (!whiteStarts) {
        currentPlayerIndex = 1; // ������ ��������, ���� whiteStarts == false
    }
}

Game::~Game()
{
    for (Player* player : players) {
        delete player;
    }
}

void Game::printBoard() const {
    bool flipped = isBoardFlipped(); //  ���������� isBoardFlipped()

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
    return !whiteStarts; //  ����� �����������, ���� ����� �� ��������.
}
// ... (��������� ������ Game.cpp - ��� ���������) ...

void Game::start() {

    while (!isGameOver()) {
        // 1. ������� ����� (� ������� ��� ����� ����������� ���������)
        std::cout << "Current Board: " << std::endl;
        printBoard(); // ���������� printBoard

        // 2. �������� ��� �� �������� ������
        std::cout << "Current Player: " << players[currentPlayerIndex]->getName() << std::endl;

        // 3. ������� ��� (���� ��� �����������, ��������� ������)
        while (!makePlayerMove());

        // 4. ���������, �� ����������� �� ����
        checkGameEnd();

        // 5. ������� ������
        switchPlayer();
    }
    //������� ���������
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

// ... (��������� ������ Game) ...
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
    std::pair<std::pair<int, int>, std::pair<int, int>> move = players[currentPlayerIndex]->getMove(board, isBoardFlipped()); // �������� isBoardFlipped()
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

    //����� ����, ���� � ������ �� ������� �� �������� �����.
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

    //���������, ����� �� ������� ����� ������� ���.  ���� ���, �� ���� �������������.
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

    //���� ������� ����� �� ����� ������� ���, �� ������� ���������.
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

    return false; //���� ������������
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