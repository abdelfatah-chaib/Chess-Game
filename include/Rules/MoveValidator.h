#pragma once

#include <vector>
#include <string>
#include <utility>
#include "Move.h"

class ChessBoard;
struct ChessPiece;

class MoveValidator {
public:
    explicit MoveValidator(const ChessBoard* board);

    // Main validation method
    bool isMoveLegal(const Move& move) const;

    // Check game states
    bool isInCheck(const std::string& playerColor) const;
    bool isCheckmate(const std::string& playerColor) const;
    bool isStalemate(const std::string& playerColor) const;

    // Get valid moves
    std::vector<Move> getValidMovesForPiece(int row, int col) const;
    std::vector<Move> getAllValidMovesForColor(const std::string& color) const;

private:
    const ChessBoard* m_board;

    // Helper methods
    bool isPieceMoveLegal(const Move& move, const ChessPiece& piece) const;
    bool wouldLeaveKingInCheck(const Move& move, const std::string& playerColor) const;
    std::pair<int, int> findKing(const std::string& color) const;
};
