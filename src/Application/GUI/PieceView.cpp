#include "PieceView.h"
#include "Entities/ChessPiece.h"

PieceView::PieceView(ChessPiece* piece) : m_piece(piece) {}

void PieceView::setPiece(ChessPiece* p) { m_piece = p; }

std::string PieceView::texturePathForPiece() const {
    if (!m_piece || m_piece->type.empty()) return "";
    const std::string& t = m_piece->type;
    const bool white = (m_piece->color == "white");
    std::string base = white ? "assets/white_" : "assets/black_";
    if (t == "pawn") return base + "pawn.png";
    if (t == "knight") return base + "knight.png";
    if (t == "bishop") return base + "bishop.png";
    if (t == "rook") return base + "rook.png";
    if (t == "queen") return base + "queen.png";
    if (t == "king") return base + "king.png";
    return "";
}

void PieceView::loadTextureFromPiece() {
    const std::string path = texturePathForPiece();
    if (path.empty()) return;
    if (m_texture.loadFromFile(path)) {
        m_sprite.setTexture(m_texture);
    }
}

void PieceView::updatePosition(int boardX, int boardY) {
    const float tileSize = 64.0f; // placeholder
    m_sprite.setPosition(boardX * tileSize, boardY * tileSize);
}

void PieceView::draw(sf::RenderWindow& window) {
    window.draw(m_sprite);
}
