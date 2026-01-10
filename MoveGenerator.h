#pragma once

#include <vector>
#include <string>
#include "Color.h"

class ChessBoard;
struct Move;
enum class SpecialMoveType;

class MoveGenerator {
public:
    explicit MoveGenerator(const ChessBoard* board);

    std::vector<Move> generateMovesForPiece(int row, int col) const;
    std::vector<Move> generateMovesForColor(Color color) const;
    bool isSquareAttacked(int row, int col, Color attacker) const;

    // Public method for attack detection (used by MoveValidator)
    bool canPieceAttackSquare(int fromRow, int fromCol, int toRow, int toCol) const;

private:
    const ChessBoard* m_board;

    void generatePawnMoves(int row, int col, std::vector<Move>& out) const;
    void generateKnightMoves(int row, int col, std::vector<Move>& out) const;
    void generateKingMoves(int row, int col, std::vector<Move>& out) const;
    void generateRookMoves(int row, int col, std::vector<Move>& out) const;
    void generateBishopMoves(int row, int col, std::vector<Move>& out) const;
    void generateQueenMoves(int row, int col, std::vector<Move>& out) const;

    // Special move generation
    void generateCastlingMoves(int row, int col, std::vector<Move>& out) const;
    bool canCastle(int kingRow, int kingCol, bool kingside) const;

    // Attack detection helpers
    bool canPawnAttack(int fromRow, int fromCol, int toRow, int toCol, const std::string& color) const;
    bool canKnightAttack(int fromRow, int fromCol, int toRow, int toCol) const;
    bool canBishopAttack(int fromRow, int fromCol, int toRow, int toCol) const;
    bool canRookAttack(int fromRow, int fromCol, int toRow, int toCol) const;
    bool canQueenAttack(int fromRow, int fromCol, int toRow, int toCol) const;
    bool canKingAttack(int fromRow, int fromCol, int toRow, int toCol) const;

    // Path validation helpers
    bool isDiagonalPath(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isStraightPath(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol) const;
};
