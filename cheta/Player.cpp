#include "Player.h"
#include <iostream>

Player::Player(const std::string& name, PieceColor color) : name(name), color(color) {}

const std::string& Player::getName() const {
	return name;
}

PieceColor Player::getColor() const {
	return color;
}


HumanPlayer::HumanPlayer(const std::string& name, PieceColor color) : Player(name, color) {}

std::pair<std::pair<int, int>, std::pair<int, int>> HumanPlayer::getMove(const Board& board, bool isFlipped) {
    int fromRow, fromCol, toRow, toCol;

    while (true) {
        std::cout << name << ", enter your move (fromRow fromCol toRow toCol): ";
        if (std::cin >> fromRow >> fromCol >> toRow >> toCol) {

            int boardSize = board.getBoardSize();
            if (isFlipped) { // Инвертируем ВСЕГДА, если доска перевернута
                fromRow = boardSize - 1 - fromRow;
                fromCol = boardSize - 1 - fromCol;
                toRow = boardSize - 1 - toRow;
                toCol = boardSize - 1 - toCol;
            }

            if (fromRow >= 0 && fromRow < board.getBoardSize() &&
                fromCol >= 0 && fromCol < board.getBoardSize() &&
                toRow >= 0 && toRow < board.getBoardSize() &&
                toCol >= 0 && toCol < board.getBoardSize())
            {
                if (board.getPieceColor(fromRow, fromCol) == getColor()) { // Используем getColor()
                    break;
                }
                else {
                    std::cout << "Invalid move: No piece of your color at the starting position." << std::endl;
                }
            }
            else {
                std::cout << "Invalid move: Coordinates out of range." << std::endl;
            }
        }
        else {
            std::cout << "Invalid input: Please enter four integers." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return { {fromRow, fromCol}, {toRow, toCol} };
}