#ifndef SAVELOADMANAGER_H
#define SAVELOADMANAGER_H

#include <string>

class ChessBoard;
    
class SaveLoadManager {
public:
    // Save the current board state to a file in a simple custom format.
    bool saveToFile(const std::string& filePath, const ChessBoard& board);

    // Load board state from a file; returns true on success.
    bool loadFromFile(const std::string& filePath, ChessBoard& board);
};

#endif // SAVELOADMANAGER_H
