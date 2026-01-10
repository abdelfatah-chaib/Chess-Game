#ifndef PIECEVIEW_H
#define PIECEVIEW_H

#include <SFML/Graphics.hpp>
#include <string>

struct ChessPiece;

class PieceView {
public:
    explicit PieceView(ChessPiece* piece = nullptr);

    void setPiece(ChessPiece* p);
    void loadTextureFromPiece();
    void updatePosition(int boardX, int boardY);
    void draw(sf::RenderWindow& window);

private:
    ChessPiece* m_piece{nullptr};
    sf::Sprite m_sprite;
    sf::Texture m_texture;

    std::string texturePathForPiece() const; // helper to map piece to texture path
};

#endif // PIECEVIEW_H
