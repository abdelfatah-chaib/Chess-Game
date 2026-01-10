#pragma once

#include <string>
#include <map>

class ChessBoard;
class MoveValidator;

// Énumération pour les résultats de partie
enum class GameResult {
    ONGOING,        // Partie en cours
    WHITE_WIN,      // Victoire des blancs
    BLACK_WIN,      // Victoire des noirs
    DRAW            // Match nul
};

// Énumération pour les raisons de fin de partie
enum class GameEndReason {
    NONE,                   // Partie en cours
    CHECKMATE,              // Échec et mat
    STALEMATE,              // Pat
    INSUFFICIENT_MATERIAL,  // Matériel insuffisant
    FIFTY_MOVE_RULE,        // Règle des 50 coups
    THREEFOLD_REPETITION,   // Triple répétition
    RESIGNATION,            // Abandon (si implémenté)
    TIMEOUT                 // Dépassement de temps (si implémenté)
};

/**
 * Classe responsable de l'évaluation des fins de partie d'échecs
 * selon les règles officielles de la FIDE.
 */
class GameEndEvaluator {
public:
    GameEndEvaluator(const ChessBoard* board, const MoveValidator* validator);
    
    // Méthode principale d'évaluation
    GameResult evaluateGameState(const std::string& currentPlayerColor);
    
    // Getter pour la raison de fin
    GameEndReason getEndReason() const { return m_endReason; }
    
    // Vérifications spécifiques des cas de fin de partie
    bool isCheckmate(const std::string& playerColor) const;
    bool isStalemate(const std::string& playerColor) const;
    bool hasInsufficientMaterial() const;
    bool isFiftyMoveRule() const;
    bool isThreefoldRepetition() const;
    
    // Obtenir une description textuelle du résultat
    std::string getResultDescription() const;
    
    // Mise à jour des compteurs (appelée après chaque coup)
    void updateAfterMove(bool isPawnMove, bool isCapture);
    
    // Enregistrer la position actuelle pour la détection de répétition
    void recordCurrentPosition();
    
    // Réinitialiser l'état
    void reset();

private:
    const ChessBoard* m_board;
    const MoveValidator* m_validator;
    
    GameResult m_currentResult;
    GameEndReason m_endReason;
    
    // Compteur pour la règle des 50 coups
    int m_halfMoveClock;
    
    // Historique des positions pour la triple répétition
    // Clé: hash de la position, Valeur: nombre d'occurrences
    std::map<std::string, int> m_positionHistory;
    
    // Méthodes privées d'aide
    std::string generatePositionHash() const;
    bool hasOnlyKings() const;
    bool hasKingVsKingBishop() const;
    bool hasKingVsKingKnight() const;
    bool hasKingBishopVsKingBishop() const;
    
    // Compte le nombre de pièces d'un type et couleur donnés
    int countPieces(const std::string& type, const std::string& color) const;
    
    // Vérifie si tous les fous d'une couleur sont sur la même couleur de case
    bool allBishopsOnSameColorSquares(const std::string& color) const;
};
