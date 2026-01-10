#pragma once

#include <string>

// Forward declarations
class ChessBoard;

/**
 * @brief Évaluateur de positions d'échecs pour l'IA
 * 
 * Cette classe fournit des fonctions d'évaluation heuristique
 * pour déterminer la qualité d'une position d'échecs.
 */
class Evaluator {
public:
    /**
     * @brief Évalue la position actuelle du plateau
     * @param board Référence constante vers l'état du plateau
     * @param playerColor Couleur du joueur à évaluer ("white" ou "black")
     * @return Score de la position (positif = avantage, négatif = désavantage)
     */
    static double evaluatePosition(const ChessBoard& board, const std::string& playerColor);

    /**
     * @brief Calcule la valeur matérielle totale d'un joueur
     * @param board Référence constante vers l'état du plateau
     * @param playerColor Couleur du joueur ("white" ou "black")
     * @return Valeur matérielle totale
     */
    static double getMaterialValue(const ChessBoard& board, const std::string& playerColor);

    /**
     * @brief Évalue la mobilité (nombre de coups possibles) d'un joueur
     * @param board Référence constante vers l'état du plateau
     * @param playerColor Couleur du joueur ("white" ou "black")
     * @return Score de mobilité
     */
    static double getMobilityScore(const ChessBoard& board, const std::string& playerColor);

    /**
     * @brief Évalue la sécurité du roi
     * @param board Référence constante vers l'état du plateau
     * @param playerColor Couleur du joueur ("white" ou "black")
     * @return Score de sécurité du roi (positif = sécurisé, négatif = exposé)
     */
    static double getKingSafetyScore(const ChessBoard& board, const std::string& playerColor);

    /**
     * @brief Obtient la valeur d'une pièce selon son type
     * @param pieceType Type de la pièce ("pawn", "rook", "knight", "bishop", "queen", "king")
     * @return Valeur numérique de la pièce
     */
    static double getPieceValue(const std::string& pieceType);

private:
    // Méthodes privées d'aide si nécessaire
};
