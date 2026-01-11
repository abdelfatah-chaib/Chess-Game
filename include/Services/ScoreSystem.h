#ifndef SCORESYSTEM_H
#define SCORESYSTEM_H

#include <string>
#include <vector>
#include <map>

class ChessBoard;

class ScoreSystem {
public:
    struct GameScore {
        int whiteScore = 0;        // Score blanc (pièces capturées)
        int blackScore = 0;        // Score noir (pièces capturées)
        int whiteMaterial = 0;     // Matériel blanc actuel (pièces restantes)
        int blackMaterial = 0;     // Matériel noir actuel (pièces restantes)

        int getDifference() const {
            return whiteScore - blackScore;
        }

        std::string toString() const {
            return "White: " + std::to_string(whiteScore) +
                ", Black: " + std::to_string(blackScore) +
                ", Diff: " + std::to_string(getDifference());
        }
    };

    struct PlayerStats {
        std::string username;
        int gamesPlayed = 0;
        int gamesWon = 0;
        int gamesLost = 0;
        int totalPoints = 0;
        int eloRating = 1200;
        int highestElo = 1200;

        float getWinRate() const {
            return gamesPlayed > 0 ? (gamesWon * 100.0f) / gamesPlayed : 0.0f;
        }
    };

    // Constructeur
    ScoreSystem();

    // Méthodes principales
    GameScore calculateCurrentScore(const ChessBoard& board);
    std::string evaluatePosition(const ChessBoard& board);

    // Gestion des joueurs
    void addGameResult(const std::string& playerName, bool won, int scoreBonus,
        const std::string& opponent = "", bool ratedGame = true);
    PlayerStats getPlayerStats(const std::string& playerName) const;

    // Classements
    std::vector<std::pair<std::string, int>> getLeaderboard(int maxPlayers = 0) const;
    std::vector<std::pair<std::string, float>> getWinRateLeaderboard(int maxPlayers = 0) const;

    // Sauvegarde/Chargement
    bool saveAllStats(const std::string& filename = "player_stats.bin");
    bool loadAllStats(const std::string& filename = "player_stats.bin");

    // Utilitaires
    int getPieceValue(const std::string& pieceType);

private:
    std::map<std::string, PlayerStats> playerStats;

    // Méthodes privées
    void updateELO(const std::string& playerName, bool won, int opponentELO);
    int calculateELOChange(int playerRating, int opponentRating, bool won);
    bool playerExists(const std::string& playerName) const;
};

#endif // SCORESYSTEM_H