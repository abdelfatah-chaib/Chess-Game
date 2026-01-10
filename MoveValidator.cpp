#include "MoveValidator.h"
#include "ChessBoard.h"
#include "ChessPiece.h"
#include "MoveGenerator.h"
#include "Color.h"
#include <iostream>

MoveValidator::MoveValidator(const ChessBoard* board) : m_board(board) {}

bool MoveValidator::isMoveLegal(const Move& move) const {
    if (!m_board) return false;
    
    // Basic validation
    if (!m_board->isInsideBoard(move.fromRow, move.fromCol) ||
        !m_board->isInsideBoard(move.toRow, move.toCol)) {
        return false;
    }
    
    const ChessPiece* piece = m_board->getPieceAt(move.fromRow, move.fromCol);
    if (!piece || piece->type.empty()) return false;
    
    const ChessPiece* target = m_board->getPieceAt(move.toRow, move.toCol);
    
    // Cannot capture own pieces
    if (target && !target->type.empty() && target->color == piece->color) {
        return false;
    }
    
    // CRITICAL: Cannot capture the king - the game should end in checkmate before this
    if (target && !target->type.empty() && target->type == "king") {
        std::cout << "[MoveValidator] ILLEGAL: Cannot capture the king! The game should have ended in checkmate." << std::endl;
        return false;
    }
    
    // Check if the piece can make this move according to its rules
    if (!isPieceMoveLegal(move, *piece)) {
        return false;
    }
    
    // Check if move would leave own king in check
    if (wouldLeaveKingInCheck(move, piece->color)) {
        return false;
    }
    
    return true;
}

bool MoveValidator::isPieceMoveLegal(const Move& move, const ChessPiece& piece) const {
    MoveGenerator generator(m_board);
    std::vector<Move> legalMoves = generator.generateMovesForPiece(move.fromRow, move.fromCol);
    
    // Check if this move is in the list of generated legal moves
    for (const Move& legalMove : legalMoves) {
        if (legalMove.fromRow == move.fromRow && legalMove.fromCol == move.fromCol &&
            legalMove.toRow == move.toRow && legalMove.toCol == move.toCol) {
            return true;
        }
    }
    
    return false;
}

bool MoveValidator::wouldLeaveKingInCheck(const Move& move, const std::string& playerColor) const {
    // Create a temporary copy of the board state to simulate the move
    // Since we can't modify the const board, we'll use the existing board methods
    
    // Find the king position before and after the move
    std::pair<int, int> kingPos = findKing(playerColor);
    if (kingPos.first == -1) return true; // King not found - illegal
    
    // If the king is the piece being moved, update its position
    const ChessPiece* movingPiece = m_board->getPieceAt(move.fromRow, move.fromCol);
    if (movingPiece && movingPiece->type == "king" && movingPiece->color == playerColor) {
        kingPos = {move.toRow, move.toCol};
    }
    
    // For now, we'll use a simplified check - this can be enhanced later
    // Check if any opponent piece can attack the king's position after the move
    Color enemyColor = (playerColor == "white") ? Color::Black : Color::White;
    
    // Temporarily "simulate" the move by checking if the king would be attacked
    // This is a simplified version - a full implementation would require board state copying
    MoveGenerator generator(m_board);
    
    // Check if any enemy piece can attack the king square
    std::string enemyColorStr = (enemyColor == Color::White) ? "white" : "black";
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && !piece->type.empty() && piece->color == enemyColorStr) {
                // Skip the piece that might be captured
                if (row == move.toRow && col == move.toCol) continue;
                
                if (generator.canPieceAttackSquare(row, col, kingPos.first, kingPos.second)) {
                    return true; // King would be in check
                }
            }
        }
    }
    
    return false; // King would not be in check
}

std::pair<int, int> MoveValidator::findKing(const std::string& color) const {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && piece->type == "king" && piece->color == color) {
                return {row, col};
            }
        }
    }
    return {-1, -1}; // King not found (should never happen in valid game)
}

bool MoveValidator::isInCheck(const std::string& playerColor) const {
    std::pair<int, int> kingPos = findKing(playerColor);
    if (kingPos.first == -1) return false; // King not found
    
    Color enemyColor = (playerColor == "white") ? Color::Black : Color::White;
    MoveGenerator generator(m_board);
    return generator.isSquareAttacked(kingPos.first, kingPos.second, enemyColor);
}

bool MoveValidator::isCheckmate(const std::string& playerColor) const {
    if (!isInCheck(playerColor)) return false;
    
    // Generate all possible moves for the player
    Color color = (playerColor == "white") ? Color::White : Color::Black;
    std::vector<Move> allValidMoves = getAllValidMovesForColor(playerColor);
    
    // If no valid moves available while in check, it's checkmate
    return allValidMoves.empty();
}

bool MoveValidator::isStalemate(const std::string& playerColor) const {
    if (isInCheck(playerColor)) return false; // In check, so not stalemate
    
    // Generate all valid moves for the player
    std::vector<Move> allValidMoves = getAllValidMovesForColor(playerColor);
    
    // If no valid moves available while not in check, it's stalemate
    return allValidMoves.empty();
}

std::vector<Move> MoveValidator::getValidMovesForPiece(int row, int col) const {
    std::vector<Move> validMoves;
    if (!m_board || !m_board->isInsideBoard(row, col)) {
        std::cout << "[MoveValidator] ERROR: Invalid position or no board" << std::endl;
        return validMoves;
    }
    
    const ChessPiece* piece = m_board->getPieceAt(row, col);
    if (!piece || piece->type.empty()) {
        std::cout << "[MoveValidator] ERROR: No piece at position" << std::endl;
        return validMoves;
    }
    
    std::cout << "[MoveValidator] ===== GENERATING MOVES FOR " << piece->type << " =====" << std::endl;
    
    MoveGenerator generator(m_board);
    std::vector<Move> candidateMoves = generator.generateMovesForPiece(row, col);
    
    std::cout << "[MoveValidator] MoveGenerator returned " << candidateMoves.size() << " candidate moves" << std::endl;
    
    if (candidateMoves.empty() && piece->type == "knight") {
        std::cout << "[MoveValidator] WARNING: Knight generated 0 moves - this is suspicious!" << std::endl;
    }
    
    for (size_t i = 0; i < candidateMoves.size(); i++) {
        const Move& move = candidateMoves[i];
        std::cout << "[MoveValidator] Checking candidate move " << i << ": (" 
                  << move.fromRow << "," << move.fromCol << ") -> (" 
                  << move.toRow << "," << move.toCol << ")" << std::endl;
                  
        if (isMoveLegal(move)) {
            validMoves.push_back(move);
            std::cout << "[MoveValidator] --> VALID" << std::endl;
        } else {
            std::cout << "[MoveValidator] --> INVALID (fails FIDE rules or king protection)" << std::endl;
        }
    }
    
    std::cout << "[MoveValidator] ===== FINAL VALID MOVES: " << validMoves.size() << " =====" << std::endl;
    return validMoves;
}

std::vector<Move> MoveValidator::getAllValidMovesForColor(const std::string& color) const {
    std::vector<Move> validMoves;
    if (!m_board) return validMoves;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && !piece->type.empty() && piece->color == color) {
                std::vector<Move> pieceMoves = getValidMovesForPiece(row, col);
                validMoves.insert(validMoves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }
    
    return validMoves;
}