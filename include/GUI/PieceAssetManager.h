#pragma once
#include <string>
#include "PieceSetType.h"
#include "BoardTheme.h"

class PieceAssetManager {
public:
    // Get the path for a piece texture based on set type, theme, and piece info
    static std::string getPiecePath(PieceSetType setType, BoardTheme theme, 
                                    const std::string& color, const std::string& pieceType);
    
    // Get the asset name for texture manager
    static std::string getPieceAssetName(PieceSetType setType, BoardTheme theme,
                                         const std::string& color, const std::string& pieceType);
    
    // Get folder name for themed pieces based on board theme
    static std::string getThemedFolderName(BoardTheme theme);
};
