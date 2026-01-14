#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <stack>
#include <utility>
#include "ChessPiece.h"
#include <filesystem>
#include "Move.h"
#include "BoardTheme.h"
#include "PieceSetType.h"  // Add this include
#include <optional>  // Add for std::optional

// Last move tracking structure
struct LastMove {
    int fromRow, fromCol;
    int toRow, toCol;
    
    LastMove(int fr, int fc, int tr, int tc) 
        : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc) {}
};

class ChessBoard {
private:
    sf::RectangleShape boardSquares[8][8];
    ChessPiece* pieces[8][8];
    sf::Texture pieceTextures[12];
    bool texturesLoaded;
    float squareSize;
    float boardX, boardY;
    std::stack<Move> moveHistory;
    ThemeColors currentTheme;
    PieceSetType currentPieceSet;  // Add this member variable

    // Tracking pour les règles de fin de partie
    int halfMoveClock;  // Pour la règle des 50 coups
    std::vector<std::string> positionHistory;  // Pour la triple répétition
    
    // King danger tracking
    std::pair<int, int> whiteKingDangerPos;  // Position du roi blanc en danger (-1, -1 si pas en danger)
    std::pair<int, int> blackKingDangerPos;  // Position du roi noir en danger (-1, -1 si pas en danger)

    // Last move tracking
    std::optional<LastMove> lastMove;

    bool createBlackPieceFromWhite(sf::Texture& blackTexture, const std::string& whitePieceName);
    void setupPiece(int row, int col, const std::string& type, const std::string& color);
    
    // New method to load themed pieces
    bool loadThemedPiece(sf::Texture& texture, const std::string& pieceName, const std::string& color, const BoardTheme& theme);

    // Integrated move validation methods from extracted logic
    bool isValidPawnMove(int fromRow, int fromCol, int toRow, int toCol, const std::string& color);
    bool isValidRookMove(int fromRow, int fromCol, int toRow, int toCol);
    bool isValidKnightMove(int fromRow, int fromCol, int toRow, int toCol);
    bool isValidBishopMove(int fromRow, int fromCol, int toRow, int toCol);
    bool isValidQueenMove(int fromRow, int fromCol, int toRow, int toCol);
    bool isValidKingMove(int fromRow, int fromCol, int toRow, int toCol);

public:
    ChessBoard();
    ~ChessBoard();

    bool loadTextures();
    bool loadTextures(PieceSetType pieceSet, BoardTheme theme);  // New overload
    void initialize(float x, float y, float size);
    void initialize(float x, float y, float size, const ThemeColors& theme);
    void initialize(float x, float y, float size, const ThemeColors& theme, PieceSetType pieceSet);  // New overload
    void draw(sf::RenderWindow& window);
    void highlightSquare(int row, int col, sf::RenderWindow& window, 
                        sf::Color color = sf::Color(101, 67, 33, 255));
    void drawLegalMoveIndicator(int row, int col, sf::RenderWindow& window);
    void setTheme(const ThemeColors& theme);
    void setPieceSet(PieceSetType pieceSet, BoardTheme theme);  // New method
    
    // Non-const and const accessors
    ChessPiece* getPieceAt(int row, int col);
    const ChessPiece* getPieceAt(int row, int col) const;

    bool movePiece(int fromRow, int fromCol, int toRow, int toCol);
    bool movePiece(int fromRow, int fromCol, int toRow, int toCol, const std::string& promotionPiece);
    bool undoMove();
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol);
    
    // Pawn promotion
    void promotePawn(int row, int col, const std::string& promotionPiece);

    sf::FloatRect getBounds() const;
    sf::Vector2f getSquarePosition(int row, int col) const;
    std::pair<int, int> getSquareAtPosition(float x, float y) const;

    // Helpers used by SaveLoadManager
    void removePiece(int row, int col);
    void setPieceByNames(int row, int col, const std::string& type, const std::string& color);
    
    // Reset board to initial state
    void reset();

    bool isInsideBoard(int row, int col) const;

    // Méthodes pour la gestion des fins de partie
    int getHalfMoveClock() const { return halfMoveClock; }
    void resetHalfMoveClock() { halfMoveClock = 0; }
    const std::vector<std::string>& getPositionHistory() const { return positionHistory; }
    void recordCurrentPosition();
    std::string generatePositionHash() const;
    
    // King danger detection and visual alert
    std::pair<int, int> getKingPosition(const std::string& color) const;
    bool isKingInDanger(const std::string& color) const;
    std::pair<int, int> getWhiteKingDangerPos() const { return whiteKingDangerPos; }
    std::pair<int, int> getBlackKingDangerPos() const { return blackKingDangerPos; }
    void updateKingDangerStatus(const std::string& color, const class MoveValidator* validator);
    void clearKingDangerStatus();
    void highlightKingDanger(sf::RenderWindow& window);

    // Last move tracking
    void setLastMove(int fromRow, int fromCol, int toRow, int toCol);
    std::optional<LastMove> getLastMove() const { return lastMove; }
    void clearLastMove() { lastMove.reset(); }
    void drawLastMoveHighlight(sf::RenderWindow& window);
};
