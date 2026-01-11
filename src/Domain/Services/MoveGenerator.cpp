#include "MoveGenerator.h"
#include "ChessBoard.h"
#include "Entities/ChessPiece.h"
#include "Color.h"
#include <cctype>
#include <iostream>

MoveGenerator::MoveGenerator(const ChessBoard* board) : m_board(board) {}

std::vector<Move> MoveGenerator::generateMovesForPiece(int row, int col) const {
    std::vector<Move> moves;
    if (!m_board || !m_board->isInsideBoard(row, col)) return moves;
    const ChessPiece* p = m_board->getPieceAt(row, col);
    if (!p || p->type.empty()) return moves;

    // Use the full type string, not just the first character
    const std::string& pieceType = p->type;
    std::cout << "[MoveGenerator] Generating moves for piece type: '" << pieceType << "'" << std::endl;
    
    if (pieceType == "pawn") {
        generatePawnMoves(row, col, moves);
    } else if (pieceType == "knight") {
        std::cout << "[MoveGenerator] Found knight! Generating knight moves..." << std::endl;
        generateKnightMoves(row, col, moves);
    } else if (pieceType == "bishop") {
        generateBishopMoves(row, col, moves);
    } else if (pieceType == "rook") {
        generateRookMoves(row, col, moves);
    } else if (pieceType == "queen") {
        generateQueenMoves(row, col, moves);
    } else if (pieceType == "king") {
        generateKingMoves(row, col, moves);
    } else {
        std::cout << "[MoveGenerator] Unknown piece type: '" << pieceType << "'" << std::endl;
    }
    
    std::cout << "[MoveGenerator] Generated " << moves.size() << " total moves for " << pieceType << std::endl;
    return moves;
}

std::vector<Move> MoveGenerator::generateMovesForColor(Color color) const {
    std::vector<Move> moves;
    if (!m_board) return moves;
    
    std::string colorStr = (color == Color::White) ? "white" : "black";
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && piece->color == colorStr && !piece->type.empty()) {
                std::vector<Move> pieceMoves = generateMovesForPiece(row, col);
                moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }
    return moves;
}

bool MoveGenerator::isSquareAttacked(int row, int col, Color attacker) const {
    if (!m_board || !m_board->isInsideBoard(row, col)) return false;
    
    std::string attackerColor = (attacker == Color::White) ? "white" : "black";
    
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            const ChessPiece* piece = m_board->getPieceAt(r, c);
            if (piece && piece->color == attackerColor && !piece->type.empty()) {
                if (canPieceAttackSquare(r, c, row, col)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool MoveGenerator::canPieceAttackSquare(int fromRow, int fromCol, int toRow, int toCol) const {
    const ChessPiece* piece = m_board->getPieceAt(fromRow, fromCol);
    if (!piece || piece->type.empty()) return false;

    // Use full piece type string
    const std::string& pieceType = piece->type;
    
    if (pieceType == "pawn") {
        return canPawnAttack(fromRow, fromCol, toRow, toCol, piece->color);
    } else if (pieceType == "knight") {
        return canKnightAttack(fromRow, fromCol, toRow, toCol);
    } else if (pieceType == "bishop") {
        return canBishopAttack(fromRow, fromCol, toRow, toCol);
    } else if (pieceType == "rook") {
        return canRookAttack(fromRow, fromCol, toRow, toCol);
    } else if (pieceType == "queen") {
        return canQueenAttack(fromRow, fromCol, toRow, toCol);
    } else if (pieceType == "king") {
        return canKingAttack(fromRow, fromCol, toRow, toCol);
    }
    
    return false;
}

void MoveGenerator::generatePawnMoves(int row, int col, std::vector<Move>& out) const {
    const ChessPiece* pawn = m_board->getPieceAt(row, col);
    if (!pawn || pawn->type.empty()) return;
    
    bool isWhite = (pawn->color == "white");
    int direction = isWhite ? -1 : 1; // White moves up (negative), black moves down (positive)
    int startRow = isWhite ? 6 : 1;   // Starting position for pawns
    int promotionRow = isWhite ? 0 : 7; // Promotion row
    
    std::cout << "[MoveGenerator] Generating pawn moves for " << pawn->color 
              << " pawn at (" << row << "," << col << ")" << std::endl;
    
    // Forward move (one square)
    int newRow = row + direction;
    if (m_board->isInsideBoard(newRow, col)) {
        const ChessPiece* target = m_board->getPieceAt(newRow, col);
        if (!target || target->type.empty()) {
            // Check for promotion
            if (newRow == promotionRow) {
                // Add promotion moves for all piece types
                std::string promotionPieces[] = {"queen", "rook", "bishop", "knight"};
                for (const std::string& promotePiece : promotionPieces) {
                    out.emplace_back(row, col, newRow, col, "", "", false, 
                                   SpecialMoveType::PawnPromotion, promotePiece);
                }
            } else {
                out.emplace_back(row, col, newRow, col);
            }
            
            // Forward move (two squares from starting position)
            if (row == startRow) {
                int newRow2 = row + 2 * direction;
                if (m_board->isInsideBoard(newRow2, col)) {
                    const ChessPiece* target2 = m_board->getPieceAt(newRow2, col);
                    if (!target2 || target2->type.empty()) {
                        out.emplace_back(row, col, newRow2, col);
                    }
                }
            }
        }
    }
    
    // Diagonal captures
    for (int colOffset = -1; colOffset <= 1; colOffset += 2) {
        int newCol = col + colOffset;
        if (m_board->isInsideBoard(newRow, newCol)) {
            const ChessPiece* target = m_board->getPieceAt(newRow, newCol);
            if (target && !target->type.empty() && target->color != pawn->color) {
                // Check for promotion on capture
                if (newRow == promotionRow) {
                    std::string promotionPieces[] = {"queen", "rook", "bishop", "knight"};
                    for (const std::string& promotePiece : promotionPieces) {
                        out.emplace_back(row, col, newRow, newCol, target->type, target->color, 
                                       false, SpecialMoveType::PawnPromotion, promotePiece);
                    }
                } else {
                    out.emplace_back(row, col, newRow, newCol, target->type, target->color);
                }
            }
        }
    }
    
    // TODO: En passant would need game state tracking
    std::cout << "[MoveGenerator] Generated " << out.size() << " pawn moves" << std::endl;
}

void MoveGenerator::generateKnightMoves(int row, int col, std::vector<Move>& out) const {
    const ChessPiece* knight = m_board->getPieceAt(row, col);
    if (!knight || knight->type.empty()) return;
    
    std::cout << "[MoveGenerator] Generating knight moves for " << knight->color 
              << " knight at (" << row << "," << col << ")" << std::endl;
    
    // Knight L-shaped moves: 8 possible moves
    int knightMoves[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };
    
    int validMoves = 0;
    for (int i = 0; i < 8; i++) {
        int newRow = row + knightMoves[i][0];
        int newCol = col + knightMoves[i][1];
        
        if (m_board->isInsideBoard(newRow, newCol)) {
            const ChessPiece* target = m_board->getPieceAt(newRow, newCol);
            if (!target || target->type.empty()) {
                // Empty square
                out.emplace_back(row, col, newRow, newCol);
                validMoves++;
                std::cout << "[MoveGenerator] Knight can move to empty square (" << newRow << "," << newCol << ")" << std::endl;
            } else if (target->color != knight->color) {
                // Enemy piece
                out.emplace_back(row, col, newRow, newCol, target->type, target->color);
                validMoves++;
                std::cout << "[MoveGenerator] Knight can capture " << target->type << " at (" << newRow << "," << newCol << ")" << std::endl;
            } else {
                std::cout << "[MoveGenerator] Knight blocked by own " << target->type << " at (" << newRow << "," << newCol << ")" << std::endl;
            }
        } else {
            std::cout << "[MoveGenerator] Knight move (" << newRow << "," << newCol << ") is out of bounds" << std::endl;
        }
    }
    
    std::cout << "[MoveGenerator] Generated " << validMoves << " knight moves" << std::endl;
}

void MoveGenerator::generateKingMoves(int row, int col, std::vector<Move>& out) const {
    const ChessPiece* king = m_board->getPieceAt(row, col);
    if (!king || king->type.empty()) return;
    
    // King moves: 8 possible directions (1 square each)
    int kingMoves[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };
    
    for (int i = 0; i < 8; i++) {
        int newRow = row + kingMoves[i][0];
        int newCol = col + kingMoves[i][1];
        
        if (m_board->isInsideBoard(newRow, newCol)) {
            const ChessPiece* target = m_board->getPieceAt(newRow, newCol);
            if (!target || target->type.empty()) {
                // Empty square - check if it's not under attack
                Color enemyColor = (king->color == "white") ? Color::Black : Color::White;
                if (!isSquareAttacked(newRow, newCol, enemyColor)) {
                    out.emplace_back(row, col, newRow, newCol);
                }
            } else if (target->color != king->color) {
                // Enemy piece - check if square is not under attack after capture
                Color enemyColor = (king->color == "white") ? Color::Black : Color::White;
                if (!isSquareAttacked(newRow, newCol, enemyColor)) {
                    out.emplace_back(row, col, newRow, newCol, target->type, target->color);
                }
            }
        }
    }
    
    // TODO: Add castling logic
    generateCastlingMoves(row, col, out);
}

void MoveGenerator::generateCastlingMoves(int row, int col, std::vector<Move>& out) const {
    const ChessPiece* king = m_board->getPieceAt(row, col);
    if (!king || king->type.empty() || king->type != "king") return;
    
    // Can't castle if king has moved
    if (king->hasMoved) return;
    
    bool isWhite = (king->color == "white");
    Color enemyColor = isWhite ? Color::Black : Color::White;
    
    // Can't castle if in check
    if (isSquareAttacked(row, col, enemyColor)) return;
    
    // Check kingside castling
    if (canCastle(row, col, true)) {
        out.emplace_back(row, col, row, col + 2, "", "", false, SpecialMoveType::Castling);
    }
    
    // Check queenside castling
    if (canCastle(row, col, false)) {
        out.emplace_back(row, col, row, col - 2, "", "", false, SpecialMoveType::Castling);
    }
}

bool MoveGenerator::canCastle(int kingRow, int kingCol, bool kingside) const {
    const ChessPiece* king = m_board->getPieceAt(kingRow, kingCol);
    if (!king || king->hasMoved) return false;
    
    bool isWhite = (king->color == "white");
    Color enemyColor = isWhite ? Color::Black : Color::White;
    
    int rookCol = kingside ? 7 : 0;
    const ChessPiece* rook = m_board->getPieceAt(kingRow, rookCol);
    
    // Check if rook exists and hasn't moved
    if (!rook || rook->type != "rook" || rook->color != king->color || rook->hasMoved) {
        return false;
    }
    
    // Check if path is clear
    int direction = kingside ? 1 : -1;
    int endCol = kingside ? rookCol - 1 : rookCol + 1;
    
    for (int col = kingCol + direction; col != endCol + direction; col += direction) {
        const ChessPiece* piece = m_board->getPieceAt(kingRow, col);
        if (piece && !piece->type.empty()) {
            return false; // Path blocked
        }
        
        // Check if king would pass through or end up in check
        if (abs(col - kingCol) <= 2) { // Only check squares king passes through
            if (isSquareAttacked(kingRow, col, enemyColor)) {
                return false; // King would be in check
            }
        }
    }
    
    return true;
}

void MoveGenerator::generateRookMoves(int row, int col, std::vector<Move>& out) const {
    const ChessPiece* rook = m_board->getPieceAt(row, col);
    if (!rook || rook->type.empty()) return;
    
    // Rook moves: 4 directions (horizontal and vertical)
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    for (int dir = 0; dir < 4; dir++) {
        int rowStep = directions[dir][0];
        int colStep = directions[dir][1];
        
        for (int step = 1; step < 8; step++) {
            int newRow = row + step * rowStep;
            int newCol = col + step * colStep;
            
            if (!m_board->isInsideBoard(newRow, newCol)) break;
            
            const ChessPiece* target = m_board->getPieceAt(newRow, newCol);
            if (!target || target->type.empty()) {
                // Empty square
                out.emplace_back(row, col, newRow, newCol);
            } else {
                if (target->color != rook->color) {
                    // Enemy piece - can capture
                    out.emplace_back(row, col, newRow, newCol, target->type, target->color);
                }
                // Blocked by any piece, stop in this direction
                break;
            }
        }
    }
}

void MoveGenerator::generateBishopMoves(int row, int col, std::vector<Move>& out) const {
    const ChessPiece* bishop = m_board->getPieceAt(row, col);
    if (!bishop || bishop->type.empty()) return;
    
    // Bishop moves: 4 diagonal directions
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    
    for (int dir = 0; dir < 4; dir++) {
        int rowStep = directions[dir][0];
        int colStep = directions[dir][1];
        
        for (int step = 1; step < 8; step++) {
            int newRow = row + step * rowStep;
            int newCol = col + step * colStep;
            
            if (!m_board->isInsideBoard(newRow, newCol)) break;
            
            const ChessPiece* target = m_board->getPieceAt(newRow, newCol);
            if (!target || target->type.empty()) {
                // Empty square
                out.emplace_back(row, col, newRow, newCol);
            } else {
                if (target->color != bishop->color) {
                    // Enemy piece - can capture
                    out.emplace_back(row, col, newRow, newCol, target->type, target->color);
                }
                // Blocked by any piece, stop in this direction
                break;
            }
        }
    }
}

void MoveGenerator::generateQueenMoves(int row, int col, std::vector<Move>& out) const {
    // Queen moves like both rook and bishop
    generateRookMoves(row, col, out);
    generateBishopMoves(row, col, out);
}

// Helper methods for attack detection
bool MoveGenerator::canPawnAttack(int fromRow, int fromCol, int toRow, int toCol, const std::string& color) const {
    int rowDiff = toRow - fromRow;
    int colDiff = abs(toCol - fromCol);
    
    if (colDiff != 1) return false;
    
    if (color == "white") {
        return rowDiff == -1;
    } else {
        return rowDiff == 1;
    }
}

bool MoveGenerator::canKnightAttack(int fromRow, int fromCol, int toRow, int toCol) const {
    int rowDiff = abs(toRow - fromRow);
    int colDiff = abs(toCol - fromCol);
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool MoveGenerator::canBishopAttack(int fromRow, int fromCol, int toRow, int toCol) const {
    return isDiagonalPath(fromRow, fromCol, toRow, toCol) && 
           isPathClear(fromRow, fromCol, toRow, toCol);
}

bool MoveGenerator::canRookAttack(int fromRow, int fromCol, int toRow, int toCol) const {
    return isStraightPath(fromRow, fromCol, toRow, toCol) && 
           isPathClear(fromRow, fromCol, toRow, toCol);
}

bool MoveGenerator::canQueenAttack(int fromRow, int fromCol, int toRow, int toCol) const {
    return (isDiagonalPath(fromRow, fromCol, toRow, toCol) || 
            isStraightPath(fromRow, fromCol, toRow, toCol)) &&
           isPathClear(fromRow, fromCol, toRow, toCol);
}

bool MoveGenerator::canKingAttack(int fromRow, int fromCol, int toRow, int toCol) const {
    int rowDiff = abs(toRow - fromRow);
    int colDiff = abs(toCol - fromCol);
    return rowDiff <= 1 && colDiff <= 1 && (rowDiff != 0 || colDiff != 0);
}

bool MoveGenerator::isDiagonalPath(int fromRow, int fromCol, int toRow, int toCol) const {
    int rowDiff = abs(toRow - fromRow);
    int colDiff = abs(toCol - fromCol);
    return rowDiff == colDiff && rowDiff > 0;
}

bool MoveGenerator::isStraightPath(int fromRow, int fromCol, int toRow, int toCol) const {
    return (fromRow == toRow && fromCol != toCol) || (fromCol == toCol && fromRow != toRow);
}

bool MoveGenerator::isPathClear(int fromRow, int fromCol, int toRow, int toCol) const {
    if (!m_board) return false;
    
    int rowStep = 0, colStep = 0;
    
    if (toRow != fromRow) {
        rowStep = (toRow > fromRow) ? 1 : -1;
    }
    if (toCol != fromCol) {
        colStep = (toCol > fromCol) ? 1 : -1;
    }
    
    int currentRow = fromRow + rowStep;
    int currentCol = fromCol + colStep;
    
    while (currentRow != toRow || currentCol != toCol) {
        const ChessPiece* piece = m_board->getPieceAt(currentRow, currentCol);
        if (piece && !piece->type.empty()) {
            return false; // Path is blocked
        }
        currentRow += rowStep;
        currentCol += colStep;
    }
    
    return true; // Path is clear
}
