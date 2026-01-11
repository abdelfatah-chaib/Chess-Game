#include "Evaluator.h"
#include "ChessBoard.h"
#include "Entities/ChessPiece.h"
#include "Rules/MoveValidator.h"
#include <cmath>

double Evaluator::evaluatePosition(const ChessBoard& board, const std::string& playerColor) {
    // Calcul silencieux du score total basé sur plusieurs facteurs
    double materialScore = getMaterialValue(board, playerColor) - getMaterialValue(board, (playerColor == "white") ? "black" : "white");
    double mobilityScore = getMobilityScore(board, playerColor) - getMobilityScore(board, (playerColor == "white") ? "black" : "white");
    double kingSafetyScore = getKingSafetyScore(board, playerColor) - getKingSafetyScore(board, (playerColor == "white") ? "black" : "white");
    
    // Pondération des différents facteurs (calcul silencieux)
    return materialScore * 1.0 +           // Le matériel est le plus important
           mobilityScore * 0.1 +            // La mobilité est secondaire
           kingSafetyScore * 0.5;           // La sécurité du roi est importante
}

double Evaluator::getMaterialValue(const ChessBoard& board, const std::string& playerColor) {
    double totalValue = 0.0;
    
    // Parcourir tout le plateau (calcul silencieux)
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const ChessPiece* piece = board.getPieceAt(row, col);
            if (piece && !piece->type.empty() && piece->color == playerColor) {
                totalValue += getPieceValue(piece->type);
            }
        }
    }
    
    return totalValue;
}

double Evaluator::getMobilityScore(const ChessBoard& board, const std::string& playerColor) {
    // Calcul silencieux de la mobilité (pas de logs pour éviter la verbosité)
    MoveValidator validator(&board);
    std::vector<Move> validMoves = validator.getAllValidMovesForColor(playerColor);
    
    // Score basé sur le nombre de coups possibles
    return static_cast<double>(validMoves.size());
}

double Evaluator::getKingSafetyScore(const ChessBoard& board, const std::string& playerColor) {
    MoveValidator validator(&board);
    
    // Pénalité si le roi est en échec (calcul silencieux)
    if (validator.isInCheck(playerColor)) {
        return -50.0;  // Pénalité importante pour être en échec
    }
    
    // Bonus pour la sécurité
    return 0.0;
}

double Evaluator::getPieceValue(const std::string& pieceType) {
    // Valeurs standard des pièces d'échecs (calcul silencieux)
    if (pieceType == "pawn") return 1.0;
    if (pieceType == "knight") return 3.0;
    if (pieceType == "bishop") return 3.0;
    if (pieceType == "rook") return 5.0;
    if (pieceType == "queen") return 9.0;
    if (pieceType == "king") return 1000.0;  // Valeur très élevée pour le roi
    
    return 0.0;  // Pièce inconnue
}