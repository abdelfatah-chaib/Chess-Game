# ChessMasterUIT

ChessMasterUIT is a complete chess application developed in C++ using SFML.  
It allows players to compete against the computer with multiple difficulty levels, manage user accounts, save games, and track statistics.

---

## Key Features

### Chess Gameplay
- Full implementation of official chess rules (legal moves, check, checkmate, castling, en passant, promotion)
- Interactive 2D graphical interface built with SFML
- Visual highlighting of selected pieces and possible moves
- Move history tracking

### Artificial Intelligence
- Three difficulty levels:
  - Easy: random move generation
  - Medium: heuristic-based evaluation
  - Hard: Minimax algorithm with advanced evaluation
- Position evaluation based on material and positional values

### User Management
- User registration and authentication system
- Secure login with email and password
- Password recovery functionality
- User profiles with game statistics

### Data Persistence
- SQLite database for user accounts and game data
- Game saving and loading system
- Score and performance tracking

---

## Project Architecture

The project follows a layered architecture with clear separation of concerns:

- Domain Layer: chess logic, rules, move validation, AI engine
- Application Layer: graphical interface, screens, UI components
- Infrastructure Layer: database access, save/load system, utilities

---

## Technologies Used
- C++ (C++17 or higher)
- SFML
- SQLite
- vcpkg (dependency management)
- CMake (optional)

---

## Clone the Repository

You can clone this repository to your local machine using Git:

```bash
git clone https://github.com/abdelfatah-chaib/Chess-Game.git
``` 

---
## Build and Run

### Requirements
- C++17 compatible compiler
- SFML 2.6.x
- SQLite

### Build with CMake
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
### Run
```
# Windows
./x64/Release/ChessMasterUIT.exe

# Linux
./build/ChessMasterUIT
```

