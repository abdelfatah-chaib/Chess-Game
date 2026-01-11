#pragma once

#include "Move.h"
#include <memory>

// Forward declarations
class ChessBoard;

/**
 * @brief Interface commune pour tous les moteurs d'IA d'échecs
 * 
 * Cette classe abstraite définit l'interface que toutes les implémentations
 * d'IA doivent respecter pour s'intégrer au système de jeu.
 */
class AIEngine {
public:
    /**
     * @brief Choisit le meilleur coup pour l'IA dans la position donnée
     * @param board Référence constante vers l'état actuel du plateau
     * @param playerColor Couleur du joueur IA ("white" ou "black")
     * @return Le coup choisi par l'IA
     */
    virtual Move chooseMove(const ChessBoard& board, const std::string& playerColor) = 0;

    /**
     * @brief Destructeur virtuel pour assurer une destruction correcte
     */
    virtual ~AIEngine() = default;

protected:
    /**
     * @brief Constructeur protégé - cette classe est abstraite
     */
    AIEngine() = default;
};

/**
 * @brief Factory pour créer des instances d'IA selon le niveau
 * @param difficulty Niveau de difficulté (1=Facile, 2=Moyen, 3=Difficile)
 * @return Pointeur unique vers l'instance d'IA appropriée
 */
std::unique_ptr<AIEngine> createAIEngine(int difficulty);
