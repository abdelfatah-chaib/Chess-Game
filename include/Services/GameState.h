#pragma once

/**
 * @brief États possibles du jeu d'échecs
 * 
 * Cet enum permet de gérer les différents états du jeu,
 * notamment pour bloquer les interactions pendant la promotion du pion.
 */
enum class GameState {
    PLAYING,                    // Jeu en cours normal
    PAWN_PROMOTION_PENDING,     // En attente de sélection pour promotion
    GAME_ENDED,                 // Partie terminée
    PAUSED                      // Jeu en pause
};
