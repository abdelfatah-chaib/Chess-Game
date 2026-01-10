# ♟️ ChessMaster-UIT  
**Projet académique — Master 1 IAOC • Université Ibn Tofail**

ChessMaster-UIT est une application complète d'échecs développée en C++ avec SFML, permettant de jouer contre l'ordinateur avec trois niveaux de difficulté, de gérer des comptes utilisateurs, de sauvegarder des parties et de consulter des statistiques.

---

## 📚 Table des matières
- [🎯 Fonctionnalités principales](#-fonctionnalités-principales)
- [🏗 Architecture du projet](#-architecture-du-projet)
- [📦 Structure du projet](#-structure-du-projet)
- [🧠 Intelligence Artificielle](#-intelligence-artificielle)
- [🛠 Prérequis et compilation](#-prérequis-et-compilation)
- [🚀 Lancement de l'application](#-lancement-de-lapplication)
- [📸 Captures d'écran](#-captures-décran)
- [👥 Contributeurs](#-contributeurs)
- [📄 Licence](#-licence)

---

## 🎯 Fonctionnalités principales

### ✔️ Jeu d'échecs complet
- **Règles officielles** : déplacements légaux, échec et mat, pat, roque, prise en passant, promotion
- **Interface graphique moderne** : plateau 2D interactif avec SFML
- **Sélection visuelle** : highlighting des cases et mouvements possibles
- **Historique des coups** : consultation des mouvements effectués

### ✔️ Système d'Intelligence Artificielle
- **3 niveaux de difficulté** :
  - **Facile (RandomAI)** : coups aléatoires pour débutants
  - **Moyen (HeuristicAI)** : évaluation heuristique basique
  - **Difficile (MinimaxAI)** : algorithme Minimax avec évaluation avancée
- **Évaluation de position** : basée sur la valeur matérielle et positionnelle

### ✔️ Gestion des utilisateurs
- **Création de compte** : système d'inscription complet
- **Authentification** : connexion sécurisée avec email/mot de passe
- **Récupération de mot de passe** : option "Mot de passe oublié"
- **Profils personnalisés** : username unique et statistiques

### ✔️ Persistance des données
- **Base de données SQLite** : stockage des comptes utilisateurs
- **Système de score** : calcul et sauvegarde des performances
- **Statistiques** : consultation de l'historique de parties
- **Sauvegarde/Chargement** : reprise de parties en cours

### ✔️ Interface utilisateur avancée
- **Écrans multiples** :
  - Écran de titre
  - Menu principal
  - Sélection du niveau de difficulté
  - Plateau de jeu
  - Écran de statistiques
- **Composants UI réutilisables** : boutons, champs de texte, cartes, checkboxes
- **Thèmes personnalisables** : différents styles de plateau et de pièces
- **Polices élégantes** : Inter, Cinzel, JetBrains Mono

---

## 🏗 Architecture du projet

Le projet suit une architecture en **couches** (Layered Architecture) avec séparation des responsabilités :

### 🧩 **1. Domain Layer (Logique métier)**
Contient toute la logique du jeu d'échecs, indépendante de l'interface.

- **Entities** : `ChessBoard`, `ChessPiece`, `Move`, `Position`, `Color`
- **Rules** : `MoveValidator`, `CastlingRules`, `CheckDetector`, `GameStateEvaluator`, `GameEndEvaluator`
- **Services** : `MoveGenerator`, `AIEngine`, `Evaluator`, `GameController`, `ScoreSystem`

### 🖥 **2. Application Layer (Interface utilisateur)**
Gère les interactions utilisateur et coordonne avec le domaine.

- **GUI (SFML)** : 
  - Écrans : `TitleScreen`, `MainMenuScreen`, `LoginScreen`, `GameBoardScreen`, `StatisticsScreen`, etc.
  - Composants : `Button`, `InputField`, `Card`, `Checkbox`, `Sidebar`
  - Vues : `PieceView`, `BoardThemeManager`, `PieceAssetManager`
- **Managers** : `ScreenManager`, `FontManager`, `TextureManager`, `Application`

### 💾 **3. Infrastructure Layer (Persistance et système)**
Accès aux ressources externes et système.

- **Persistence** : `SQLiteManager`, `SaveLoadManager`, `GameRepository`
- **System** : `Logger`, `UIHelpers`

---

## 📦 Structure du projet

```
ChessMasterUIT/
├── Source Files/
│   ├── Application/
│   │   ├── GUI/              # Écrans et composants UI
│   │   └── CLI/              # Interface ligne de commande (legacy)
│   ├── Domain/
│   │   ├── Entities/         # Modèles de données
│   │   ├── Rules/            # Validation et détection
│   │   └── Services/         # IA, évaluation, contrôleurs
│   └── Infrastructure/
│       ├── Persistence/      # Base de données, sauvegardes
│       └── System/           # Logging, utilitaires
│
├── Header Files/
│   ├── Entities/             # Headers des entités
│   ├── Rules/                # Headers des règles
│   ├── Services/             # Headers des services
│   └── GUI/                  # Headers de l'interface
│
├── Resource Files/
│   ├── Assets/               # Images (backgrounds, icônes, pièces)
│   └── fonts/                # Polices (Inter, Cinzel, JetBrains Mono)
│
├── External/
│   └── sqlite/               # Bibliothèque SQLite3
│
├── .gitignore                # Exclusions Git
├── README.md                 # Documentation du projet
├── vcpkg.json                # Dépendances (SFML, ImGui)
├── CMakeLists.txt            # Configuration CMake
└── ChessMasterUIT.vcxproj    # Projet Visual Studio
```

---

## 🧠 Intelligence Artificielle

### Architecture IA
L'IA est composée de plusieurs modules :

```cpp
AIEngine (interface)
├── RandomAI      // Niveau Facile : coups aléatoires
├── HeuristicAI   // Niveau Moyen : évaluation simple
└── MinimaxAI     // Niveau Difficile : recherche approfondie
```

### Algorithmes et heuristiques

#### **RandomAI** (Facile)
- Sélection aléatoire parmi tous les coups légaux
- Idéal pour débutants et tests

#### **HeuristicAI** (Moyen)
- Évaluation basée sur :
  - Valeur matérielle des captures
  - Contrôle du centre (bonus de position)
  - Évaluation globale du plateau
- Choisit le meilleur coup selon le score

#### **MinimaxAI** (Difficile)
- Évaluation avancée incluant :
  - Captures avec bonus élevé (×15)
  - Contrôle stratégique du centre
  - Développement des pièces (cavaliers, fous)
  - Pénalité pour exposition du roi
- Profondeur de recherche configurable (par défaut : 3 coups)

### Système d'évaluation (Evaluator)

**Valeurs des pièces** :
- Pion : 1.0
- Cavalier / Fou : 3.0
- Tour : 5.0
- Dame : 9.0
- Roi : 1000.0 (valeur symbolique)

---

## 🛠 Prérequis et compilation

### 🚧 **Prérequis**
- **Compilateur** : C++17 ou supérieur (MSVC, GCC, Clang)
- **SFML** : 2.6.x
- **vcpkg** : gestionnaire de paquets recommandé
- **CMake** : ≥ 3.20 (optionnel)

### 📥 **Installation des dépendances avec vcpkg**

```bash
# Installer vcpkg (si pas déjà fait)
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat   # Windows
./bootstrap-vcpkg.sh    # Linux/Mac

# Installer SFML
./vcpkg install sfml:x64-windows
```

### 🔧 **Compilation avec Visual Studio**

1. Ouvrir `ChessMasterUIT.sln`
2. Sélectionner la configuration **Release** ou **Debug**
3. Compiler le projet (`Ctrl+Shift+B`)
4. Lancer l'application (`F5`)

### 🔧 **Compilation avec CMake**

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[chemin_vers_vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

---

## 🚀 Lancement de l'application

### ▶️ **Exécution**

```bash
# Windows
./x64/Release/ChessMasterUIT.exe

# Linux
./build/ChessMasterUIT
```

### 🎮 **Utilisation**

1. **Écran de titre** : cliquer sur "Get Started"
2. **Authentification** : se connecter ou créer un compte
3. **Menu principal** : sélectionner "Start Game"
4. **Choix de difficulté** : Easy / Medium / Hard
5. **Jeu** :
   - Cliquer sur une pièce pour la sélectionner
   - Cliquer sur une case valide pour la déplacer
   - Le tour de l'IA s'exécute automatiquement
6. **Statistiques** : consulter vos performances depuis le menu

---

## 📸 Captures d'écran

_(Ajoutez ici des screenshots de votre application)_

---

## 👥 Contributeurs

**Master 1 IAOC - Université Ibn Tofail**

- Développement : [Votre nom]
- Architecture : [Nom]
- IA et Algorithmes : [Nom]
- Interface Graphique : [Nom]

---

## 📄 Licence

Ce projet est développé dans un cadre académique à l'**Université Ibn Tofail** pour le Master 1 IAOC.

**Ressources utilisées** :
- **SFML** : bibliothèque graphique (zlib/png license)
- **SQLite** : base de données (Public Domain)
- **Polices** : Inter (SIL OFL), Cinzel (SIL OFL), JetBrains Mono (Apache 2.0)

---

## 🔗 Liens utiles

- **GitHub Repository** : https://github.com/abdelfatah-chaib/Chess-Game
- **SFML Documentation** : https://www.sfml-dev.org/documentation/
- **SQLite Documentation** : https://www.sqlite.org/docs.html

---

**Développé avec ❤️ par l'équipe ChessMaster-UIT**