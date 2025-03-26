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

std::pair<std::pair<int, int>, std::pair<int, int>> HumanPlayer::getMove(const Board& board) {
    int fromRow, fromCol, toRow, toCol;
    int boardSize = board.getBoardSize(); // Получаем размер доски один раз

    while (true) {
        // Используем Player::getColor() для получения цвета
        std::string colorStr = (getColor() == PieceColor::WHITE) ? "White" : "Black";
        std::cout << name << " (" << colorStr << "), enter your move (fromRow fromCol toRow toCol): ";

        if (std::cin >> fromRow >> fromCol >> toRow >> toCol) {
            // Проверяем только базовую корректность координат
            if (fromRow >= 0 && fromRow < boardSize &&
                fromCol >= 0 && fromCol < boardSize &&
                toRow >= 0 && toRow < boardSize &&
                toCol >= 0 && toCol < boardSize)
            {
                break; // Координаты в пределах доски, выходим из цикла
            }
            else {
                std::cout << "Invalid input: Coordinates must be between 0 and " << (boardSize - 1) << "." << std::endl;
            }
        }
        else {
            std::cout << "Invalid input format. Please enter four integers separated by spaces." << std::endl;
            std::cin.clear(); // Сбросить флаги ошибок ввода
            // Очистить буфер ввода до следующей новой строки
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    // Возвращаем введенные координаты
    return { {fromRow, fromCol}, {toRow, toCol} };
}