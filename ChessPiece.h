#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Move.h"

struct ChessPiece {
    sf::Sprite sprite;
    std::string type;
    std::string color;
    bool hasMoved;
    int row, col;

    ChessPiece();

    char getType() const {
        return !type.empty() ? type[0] : '\0';
    }
};
