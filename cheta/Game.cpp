#include "Game.h"
#include <iostream>
#include <cstdlib>

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
    int size = board.getBoardSize();
    // �������� ������ ��������
    std::cout << "  "; // ������ ��� ������� �����
    for (int j = 0; j < size; ++j) {
        std::cout << j << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < size; ++i) {
        std::cout << i << " "; // �������� ����� ������
        for (int j = 0; j < size; ++j) {
            // ���������� i � j ��������, �.�. ��� ��� ������������� ������ �����
            Piece* piece = board.getPiece(i, j); // ���������� getPiece
            if (piece) { // ���� ������ �� �����
                PieceColor color = piece->getColor();
                bool isKing = piece->isKing();
                if (color == PieceColor::WHITE) {
                    std::cout << (isKing ? "W " : "w ");
                }
                else { // BLACK
                    std::cout << (isKing ? "B " : "b ");
                }
            }
            else { // ������ ������
                // ���������� '-' ��� '.', ����� ���������� ��� ������� ��������� �����
                std::cout << (((i + j) % 2 == 0) ? ". " : "- "); // ������ ��� ����������� ���������� ������
                // ��� ������: std::cout << ". ";
            }
        }
        std::cout << i; // ����� ������ ������
        std::cout << std::endl;
    }
    // �������� ������ �������� �����
    std::cout << "  ";
    for (int j = 0; j < size; ++j) {
        std::cout << j << " ";
    }
    std::cout << std::endl;
}


void Game::start() {
    while (!isGameOver()) {

        // --- ������� ������� ---
#ifdef _WIN32 // ������ ��� Windows (������ ������������ ������������ MSVC)
        system("cls");
#endif
        // ------------------------

        // ������ ������� ����������� ���������
        std::cout << "\nCurrent Board:" << std::endl;
        printBoard(); // ����� ����� ����� �������
        std::cout << "Current Player: " << players[currentPlayerIndex]->getName()
            << " (" << (getCurrentPlayerColor() == PieceColor::WHITE ? "White" : "Black") << ")" << std::endl;

        bool moveResult = makePlayerMove(); // ��������� ��� (������� ����������� ���� � �.�.)

        // ���� makePlayerMove ������� false (����� �� ����), checkGameEnd ��� ���������
        if (!moveResult && !isGameOver()) { // ������� !isGameOver() �� ������ ������
            // ����� �������� ���������, ���� �����
            // std::cout << "No moves were made." << std::endl;
        }

        // ��������� ����� ���� ����� ������� ����
        if (checkGameEnd()) {
            break; // ����� �� ��������� ����� ����
        }

        // ����������� ������ ������ ���� ���� �� �����������
        switchPlayer();

    } // ����� while (!isGameOver())

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    std::cout << "\n--- Game Over ---" << std::endl;
    printBoard(); // �������� ��������� �����
    switch (gameState) {
    case GameState::WHITE_WON:
        std::cout << "White won!" << std::endl;
        break;
    case GameState::BLACK_WON:
        std::cout << "Black won!" << std::endl;
        break;
    case GameState::DRAW: // ����� ���� �� �����������, �� ����� �����
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
    bool turnFinished = false; // ����: �������� �� ����� ���� ��� ���������
    int currentPieceRow = -1, currentPieceCol = -1; // ������� ����� ��� ����� �������

    // �������� ���� ���� ������. �����������, ���� ��� �� �������� (turnFinished = true).
    while (!turnFinished) {

        int fromRow, fromCol, toRow, toCol;
        bool isContinuationJump = (currentPieceRow != -1);

        // 1. ��������� ����� �� ������
        if (isContinuationJump) {
            // ������ ������ �������� ��������� ��� ����������� ������
            fromRow = currentPieceRow;
            fromCol = currentPieceCol;
            std::cout << currentPlayer->getName() << ", continue jump from (" << fromRow << "," << fromCol << ") (enter toRow toCol): ";
            int nextToRow, nextToCol;
            while (!(std::cin >> nextToRow >> nextToCol)) { // ���� ��������� ��������� ������� �����
                std::cout << "Invalid input format. Please enter two integers." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Continue jump from (" << fromRow << "," << fromCol << ") (enter toRow toCol): ";
            }
            toRow = nextToRow;
            toCol = nextToCol;
        }
        else {
            // ������ ������� ���� (fromRow, fromCol, toRow, toCol)
            // getMove ������ HumanPlayer ��� ������������ �������� ������ � ����� �� �������
            std::pair<std::pair<int, int>, std::pair<int, int>> move = currentPlayer->getMove(board);
            fromRow = move.first.first;
            fromCol = move.first.second;
            toRow = move.second.first;
            toCol = move.second.second;
        }

        // 2. ��������� ����/������ �� ������ ������ ����
        bool isValidAction = false; // ���� ���������� ������� �������
        bool isAttemptedJump = std::abs(toRow - fromRow) >= 2;
        auto requiredJumpsInfo = board.getRequiredJumps(playerColor); // �����, ������� ������� ����
        bool mustJumpGenerally = !requiredJumpsInfo.empty();          // ���� �� ������ ������������ ������

        if (isContinuationJump) { // --- ��������� ����������� ����� ---
            // ������ ���� ������, � �� ������ ���� �������� � ������� ������
            if (isAttemptedJump && board.isJumpPossible(fromRow, fromCol, toRow, toCol, playerColor)) {
                isValidAction = true;
            }
            else {
                std::cout << "Invalid continuation jump. You must make a valid jump from (" << fromRow << "," << fromCol << ")." << std::endl;
                // isValidAction �������� false
                // currentPieceRow/Col �������� ��������, ���� while �������� ���� �����
            }
        }
        else if (mustJumpGenerally) { // --- ��������� ������� ���� ��� ������� ����. ������� ---
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
                isValidAction = true; // ��� ������� ��� ������������� ������ ���������
            }
            // ���� �� ���� �� ������� �� ���������, isValidAction �������� false
        }
        else { // --- ��������� �������� ���� ��� ��������������� ������ ---
            if (board.isValidMove(fromRow, fromCol, toRow, toCol, playerColor)) {
                // isValidMove ��������� ��: ����, ������� ������, �����������, ������������ ������ (�� ���)
                isValidAction = true;
            }
            else {
                // �������� ������� ������ ��� ����� ������� ���������
                if (!board.isInsideBoard(fromRow, fromCol) || !board.isInsideBoard(toRow, toCol))
                    std::cout << "Invalid move: Coordinates out of bounds." << std::endl;
                else if (board.getPieceColor(fromRow, fromCol) != playerColor)
                    std::cout << "Invalid move: No piece of your color at (" << fromRow << "," << fromCol << ")." << std::endl;
                else if (board.getPiece(toRow, toCol) != nullptr)
                    std::cout << "Invalid move: Destination square (" << toRow << "," << toCol << ") is occupied." << std::endl;
                else // ������ ������� (�� �� ���������, �������� ����������� ��� ����� � �.�.)
                    std::cout << "Invalid move logic. Please check rules." << std::endl;
                // isValidAction �������� false
            }
        }

        // 3. ���������� ��������, ���� ��� �������
        if (isValidAction) {
            Piece* movingPiece = board.getPiece(fromRow, fromCol); // �������� ��������� �� �����������
            board.setPiece(toRow, toCol, movingPiece);           // ���������� ��������� � ����� ������
            board.clearPiece(fromRow, fromCol);                  // ������� ������ ������ (������������� nullptr)
            bool piecePromoted = false;

            if (isAttemptedJump) { // --- ��������� ����������� ������ ---
                // ������� ������ �����
                int rowDir = (toRow - fromRow > 0) ? 1 : -1;
                int colDir = (toCol - fromCol > 0) ? 1 : -1;
                int jumpedRow = -1, jumpedCol = -1;
                for (int i = 1; i < std::abs(toRow - fromRow); ++i) {
                    int currentRow = fromRow + i * rowDir;
                    int currentCol = fromCol + i * colDir;
                    // ���� ����� ���������� �� ����
                    if (board.getPiece(currentRow, currentCol) != nullptr && board.getPieceColor(currentRow, currentCol) != playerColor) {
                        jumpedRow = currentRow;
                        jumpedCol = currentCol;
                        break;
                    }
                }
                if (jumpedRow != -1) {
                    board.removePiece(jumpedRow, jumpedCol); // ������� ������ Piece
                    // std::cout << "Piece captured at (" << jumpedRow << "," << jumpedCol << ")" << std::endl; // Debug message
                }

                // �������� �� ����������� � �����
                if (!movingPiece->isKing() && ((playerColor == PieceColor::WHITE && toRow == board.getBoardSize() - 1) || (playerColor == PieceColor::BLACK && toRow == 0))) {
                    movingPiece->makeKing();
                    piecePromoted = true;
                    std::cout << "Piece promoted to King!" << std::endl;
                }

                // �������� �� �����������/������������� ����������� �������
                if (!piecePromoted && board.canJumpFrom(toRow, toCol, playerColor)) {
                    currentPieceRow = toRow; // ���������� ������� ��� ����. ������
                    currentPieceCol = toCol;
                    std::cout << "Another jump possible/required from (" << toRow << "," << toCol << ")." << std::endl;
                    printBoard(); // ���������� ����� ����� �������������� ������
                    // turnFinished �������� false, ������� ���� while �����������
                }
                else {
                    turnFinished = true; // ������ ��������, ����� ���������
                }
            }
            else { // --- ��������� ����������� �������� ���� ---
                // �������� �� ����������� � �����
                if (!movingPiece->isKing() && ((playerColor == PieceColor::WHITE && toRow == board.getBoardSize() - 1) || (playerColor == PieceColor::BLACK && toRow == 0))) {
                    movingPiece->makeKing();
                    // piecePromoted = true; // �� ������������ ������
                    std::cout << "Piece promoted to King!" << std::endl;
                }
                turnFinished = true; // ������� ��� ������ ��������� �������
            }
        }
        // else { // ���� isValidAction == false
        //   ������ �� ������. turnFinished �������� false.
        //   ���� while �������� ������, ���������� ���� � ���� �� ������.
        // }

    } // ����� while (!turnFinished)

    // ���������� true, �.�. ������� ����������� ������ ����� ��������� ����/�����
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