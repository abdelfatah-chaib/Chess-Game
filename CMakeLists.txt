cmake_minimum_required(VERSION 3.15)
project(ChessMasterUIT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Source files
set(SOURCES
    main.cpp
    Application.cpp
    UIHelpers.cpp
    Button.cpp
    InputField.cpp
    TextureManager.cpp
    FontManager.cpp
    ScreenManager.cpp
    GameController.cpp
    ChessBoard.cpp
    ChessPiece.cpp
    TitleScreen.cpp
    GetStartedScreen.cpp
    CreateAccountScreen.cpp
    EmailPasswordScreen.cpp
    UsernameScreen.cpp
    MainMenuScreen.cpp
    GameBoardScreen.cpp
    ConfirmationScreen.cpp
    SaveLoadManager.cpp
    Logger.cpp
)

# Header files
set(HEADERS
    AppState.h
    Application.h
    UIHelpers.h
    Button.h
    InputField.h
    TextureManager.h
    FontManager.h
    UserData.h
    Screen.h
    ScreenManager.h
    GameController.h
    ChessBoard.h
    ChessPiece.h
    TitleScreen.h
    GetStartedScreen.h
    CreateAccountScreen.h
    EmailPasswordScreen.h
    UsernameScreen.h
    MainMenuScreen.h
    GameBoardScreen.h
    ConfirmationScreen.h
    SaveLoadManager.h
    Logger.h
)

# Executable
add_executable(ChessMasterUIT ${SOURCES} ${HEADERS})

# Link SFML
target_link_libraries(ChessMasterUIT sfml-graphics sfml-window sfml-system)

# Include directories
target_include_directories(ChessMasterUIT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Copy assets directory into the target output folder so runtime loads from "assets/<file>"
add_custom_command(TARGET ChessMasterUIT POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    $<TARGET_FILE_DIR:ChessMasterUIT>/assets
)

# For Windows: copy SFML DLLs to output directory
if(WIN32)
    foreach(LIB sfml-graphics sfml-window sfml-system)
        add_custom_command(TARGET ChessMasterUIT POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${LIB}>
            $<TARGET_FILE_DIR:ChessMasterUIT>)
    endforeach()
endif()

# Print success message
message(STATUS "ChessMasterUIT - All 8 screens configured!")
message(STATUS "  - TitleScreen")
message(STATUS "  - GetStartedScreen")
message(STATUS "  - CreateAccountScreen")
message(STATUS "  - EmailPasswordScreen")
message(STATUS "  - UsernameScreen")
message(STATUS "  - MainMenuScreen")
message(STATUS "  - GameBoardScreen")
message(STATUS "  - ConfirmationScreen")
