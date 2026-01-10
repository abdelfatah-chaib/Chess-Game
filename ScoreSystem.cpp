#include "ScoreSystem.h"
#include "ChessBoard.h"
#include "ChessPiece.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstdint>

// Fonction de vérification des pointeurs corrompus
bool isBadScorePointer(const ChessPiece* p) {
    if (!p) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(p);
    return (addr == 0x2AAD3C1FF8 || addr == 0x00000024C1FF8);
}

// Valeurs des pièces en POINTS (vrais poids d'échecs)
int ScoreSystem::getPieceValue(const std::string& pieceType) {
    if (pieceType.empty()) return 0;

    // VALEURS STANDARD INTERNATIONALES D'ÉCHECS
    if (pieceType == "pawn" || pieceType == "P") return 1;
    if (pieceType == "knight" || pieceType == "N") return 3;
    if (pieceType == "bishop" || pieceType == "B") return 3;
    if (pieceType == "rook" || pieceType == "R") return 5;
    if (pieceType == "queen" || pieceType == "Q") return 9;
    if (pieceType == "king" || pieceType == "K") return 0;

    return 0;
}

// Constructeur
ScoreSystem::ScoreSystem() {
    loadAllStats();
}

// --- CALCUL DU SCORE ACTUEL ---
ScoreSystem::GameScore ScoreSystem::calculateCurrentScore(const ChessBoard& board) {
    GameScore score{};

    // Valeurs initiales de chaque camp (sans les rois)
    // 8 pions = 8, 2 cavaliers = 6, 2 fous = 6, 2 tours = 10, 1 dame = 9
    const int TOTAL_INITIAL_MATERIAL = 39;
    
    int currentWhiteMaterial = 0;
    int currentBlackMaterial = 0;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const ChessPiece* piece = board.getPieceAt(row, col);

            // VÉRIFICATION CRITIQUE DU POINTEUR
            if (isBadScorePointer(piece)) {
                std::cerr << "[ScoreSystem] WARNING: Bad pointer at (" << row << "," << col << ")" << std::endl;
                continue;
            }

            if (!piece) continue;

            // Check if piece exists and has a valid type
            if (piece->type.empty()) continue;

            int value = getPieceValue(piece->type);
            if (piece->color == "white") {
                currentWhiteMaterial += value;
            }
            else if (piece->color == "black") {
                currentBlackMaterial += value;
            }
        }
    }

    // Score = pièces CAPTURÉES chez l'adversaire
    score.whiteScore = TOTAL_INITIAL_MATERIAL - currentBlackMaterial;
    score.blackScore = TOTAL_INITIAL_MATERIAL - currentWhiteMaterial;

    // Garder le matériel actuel pour référence
    score.whiteMaterial = currentWhiteMaterial;
    score.blackMaterial = currentBlackMaterial;

    return score;
}

// --- ÉVALUATION DE POSITION ---
std::string ScoreSystem::evaluatePosition(const ChessBoard& board) {
    GameScore score = calculateCurrentScore(board);
    int difference = score.whiteScore - score.blackScore;

    std::stringstream evaluation;
    evaluation << "=== ÉVALUATION DE POSITION ===\n";
    evaluation << "Matériel Blanc restant: " << score.whiteMaterial << " points\n";
    evaluation << "Matériel Noir restant:  " << score.blackMaterial << " points\n";
    evaluation << "Score Blanc (capturé):  " << score.whiteScore << " points\n";
    evaluation << "Score Noir (capturé):   " << score.blackScore << " points\n";
    evaluation << "Différence:             " << difference << " points\n\n";

    // Seuils ajustés pour les valeurs standard d'échecs
    if (difference > 9) evaluation << "AVANTAGE DÉCISIF POUR LES BLANCS";
    else if (difference > 5) evaluation << "AVANTAGE IMPORTANT POUR LES BLANCS";
    else if (difference > 3) evaluation << "AVANTAGE MODÉRÉ POUR LES BLANCS";
    else if (difference > 1) evaluation << "LÉGER AVANTAGE POUR LES BLANCS";
    else if (difference > -1) evaluation << "POSITION ÉQUILIBRÉE";
    else if (difference > -3) evaluation << "LÉGER AVANTAGE POUR LES NOIRS";
    else if (difference > -5) evaluation << "AVANTAGE MODÉRÉ POUR LES NOIRS";
    else if (difference > -9) evaluation << "AVANTAGE IMPORTANT POUR LES NOIRS";
    else evaluation << "AVANTAGE DÉCISIF POUR LES NOIRS";

    return evaluation.str();
}

// --- AJOUTER UN RÉSULTAT DE PARTIE ---
void ScoreSystem::addGameResult(const std::string& playerName, bool won, int scoreBonus,
    const std::string& opponent, bool ratedGame) {
    if (!playerExists(playerName)) {
        playerStats[playerName] = PlayerStats();
        playerStats[playerName].username = playerName;
    }

    PlayerStats& stats = playerStats[playerName];
    stats.gamesPlayed++;

    if (won) {
        stats.gamesWon++;
        stats.totalPoints += 100 + scoreBonus;
    }
    else {
        stats.gamesLost++;
        stats.totalPoints += 20 + scoreBonus;
    }

    if (ratedGame) {
        int opponentELO = 1200;
        if (!opponent.empty() && playerExists(opponent)) opponentELO = playerStats[opponent].eloRating;

        updateELO(playerName, won, opponentELO);
    }

    if (stats.eloRating > stats.highestElo) stats.highestElo = stats.eloRating;

    saveAllStats();
}

// --- CALCUL ELO ---
void ScoreSystem::updateELO(const std::string& playerName, bool won, int opponentELO) {
    if (!playerExists(playerName)) return;

    PlayerStats& stats = playerStats[playerName];
    int change = calculateELOChange(stats.eloRating, opponentELO, won);
    stats.eloRating += change;

    if (stats.eloRating < 100) stats.eloRating = 100;
    if (stats.eloRating > 3000) stats.eloRating = 3000;
}

int ScoreSystem::calculateELOChange(int playerRating, int opponentRating, bool won) {
    const int K = 32;
    double expected = 1.0 / (1.0 + pow(10.0, (opponentRating - playerRating) / 400.0));
    double actual = won ? 1.0 : 0.0;

    int change = static_cast<int>(K * (actual - expected));
    if (won && change < 10) change = 10;
    if (!won && change > -10) change = -10;

    return change;
}

// --- STATS JOUEUR ---
ScoreSystem::PlayerStats ScoreSystem::getPlayerStats(const std::string& playerName) const {
    auto it = playerStats.find(playerName);
    if (it != playerStats.end()) return it->second;
    PlayerStats emptyStats; emptyStats.username = playerName;
    return emptyStats;
}

// --- CLASSEMENTS ---
std::vector<std::pair<std::string, int>> ScoreSystem::getLeaderboard(int maxPlayers) const {
    std::vector<std::pair<std::string, int>> leaderboard;
    for (const auto& pair : playerStats) leaderboard.push_back({ pair.first, pair.second.eloRating });

    std::sort(leaderboard.begin(), leaderboard.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    if (maxPlayers > 0 && leaderboard.size() > static_cast<size_t>(maxPlayers))
        leaderboard.resize(maxPlayers);

    return leaderboard;
}

std::vector<std::pair<std::string, float>> ScoreSystem::getWinRateLeaderboard(int maxPlayers) const {
    std::vector<std::pair<std::string, float>> leaderboard;
    for (const auto& pair : playerStats) {
        if (pair.second.gamesPlayed >= 3)
            leaderboard.push_back({ pair.first, pair.second.getWinRate() });
    }

    std::sort(leaderboard.begin(), leaderboard.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    if (maxPlayers > 0 && leaderboard.size() > static_cast<size_t>(maxPlayers))
        leaderboard.resize(maxPlayers);

    return leaderboard;
}

// --- SAUVEGARDE / CHARGEMENT ---
bool ScoreSystem::saveAllStats(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    size_t count = playerStats.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& pair : playerStats) {
        // 1. Écrire le nom
        size_t nameSize = pair.first.size();
        file.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
        file.write(pair.first.c_str(), nameSize);

        // 2. Écrire CHAQUE CHAMP individuellement (sauf std::string)
        const PlayerStats& stats = pair.second;
        file.write(reinterpret_cast<const char*>(&stats.gamesPlayed), sizeof(stats.gamesPlayed));
        file.write(reinterpret_cast<const char*>(&stats.gamesWon), sizeof(stats.gamesWon));
        file.write(reinterpret_cast<const char*>(&stats.gamesLost), sizeof(stats.gamesLost));
        file.write(reinterpret_cast<const char*>(&stats.eloRating), sizeof(stats.eloRating));
        file.write(reinterpret_cast<const char*>(&stats.highestElo), sizeof(stats.highestElo));
        file.write(reinterpret_cast<const char*>(&stats.totalPoints), sizeof(stats.totalPoints));
    }

    file.close();
    return true;
}

bool ScoreSystem::loadAllStats(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    playerStats.clear();
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        // 1. Lire le nom d'utilisateur
        size_t nameSize;
        file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));

        std::string username(nameSize, '\0');
        file.read(&username[0], nameSize);

        // 2. Créer un PlayerStats vide
        PlayerStats stats;
        stats.username = username; // Assignment correct pour std::string

        // 3. Lire CHAQUE CHAMP individuellement (sauf std::string)
        file.read(reinterpret_cast<char*>(&stats.gamesPlayed), sizeof(stats.gamesPlayed));
        file.read(reinterpret_cast<char*>(&stats.gamesWon), sizeof(stats.gamesWon));
        file.read(reinterpret_cast<char*>(&stats.gamesLost), sizeof(stats.gamesLost));
        file.read(reinterpret_cast<char*>(&stats.eloRating), sizeof(stats.eloRating));
        file.read(reinterpret_cast<char*>(&stats.highestElo), sizeof(stats.highestElo));
        file.read(reinterpret_cast<char*>(&stats.totalPoints), sizeof(stats.totalPoints));

        playerStats[username] = stats;
    }

    file.close();
    return true;
}

// --- MÉTHODES PRIVÉES ---
bool ScoreSystem::playerExists(const std::string& playerName) const {
    return playerStats.find(playerName) != playerStats.end();
}