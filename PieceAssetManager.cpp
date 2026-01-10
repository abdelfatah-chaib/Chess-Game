#include "PieceAssetManager.h"
#include <sstream>

std::string PieceAssetManager::getPiecePath(PieceSetType setType, BoardTheme theme,
                                            const std::string& color, const std::string& pieceType) {
    if (setType == PieceSetType::CLASSIC) {
        // Classic pieces from assets folder
        std::string prefix = (color == "White" || color == "white") ? "w_" : "b_";
        return prefix + pieceType + ".png";
    }
    else {
        // Themed pieces from pieces folder
        if (color == "White" || color == "white") {
            // White pieces are always the same regardless of theme
            return "pieces/player white/" + pieceType + ".png";
        }
        else {
            // Black pieces vary by theme
            std::string themeFolder = getThemedFolderName(theme);
            return "pieces/" + themeFolder + "/" + pieceType + ".png";
        }
    }
}

std::string PieceAssetManager::getPieceAssetName(PieceSetType setType, BoardTheme theme,
                                                 const std::string& color, const std::string& pieceType) {
    if (setType == PieceSetType::CLASSIC) {
        std::string prefix = (color == "White" || color == "white") ? "w_" : "b_";
        return prefix + pieceType;
    }
    else {
        // Create unique names for themed pieces
        std::string colorPrefix = (color == "White" || color == "white") ? "white" : "black";
        if (color == "White" || color == "white") {
            return "themed_white_" + pieceType;
        }
        else {
            std::string themeName = getThemedFolderName(theme);
            return "themed_" + themeName + "_" + pieceType;
        }
    }
}

std::string PieceAssetManager::getThemedFolderName(BoardTheme theme) {
    switch (theme) {
        case BoardTheme::Wooden:
            return "wooden";
        case BoardTheme::RedWine:
            return "redwine";
        case BoardTheme::BlueSky:
            return "bluesky";
        default:
            return "wooden";
    }
}
