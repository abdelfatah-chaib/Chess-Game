# 🚀 Guide de préparation Git et Push vers GitHub

## 📋 Étapes de préparation du projet

### 1️⃣ Nettoyage des fichiers inutiles (OPTIONNEL)

Les fichiers suivants peuvent être supprimés avant le commit :

```bash
# Fichiers de test
del AI_INTEGRATION_TEST.cpp

# Fichiers de documentation en doublon
del ARCHITECTURE.md

# Fichiers générés (seront ignorés par .gitignore)
del chessgame.db
del player_stats.bin
```

**Note** : Ces fichiers sont déjà exclus par le `.gitignore`, donc ils ne seront pas commités même si vous les laissez.

---

## 2️⃣ Initialisation du dépôt Git

```bash
# Se placer dans le dossier du projet
cd "C:\Users\abdel\OneDrive - uit.ac.ma\Bureau\M1-IAOC\Conception and Programing CPP\ChessMasterUIT-Project\ChessMasterUIT"

# Initialiser Git
git init

# Ajouter le remote (votre dépôt GitHub)
git remote add origin https://github.com/abdelfatah-chaib/Chess-Game.git
```

---

## 3️⃣ Premier commit

```bash
# Vérifier les fichiers à commiter
git status

# Ajouter tous les fichiers (sauf ceux dans .gitignore)
git add .

# Créer le commit initial
git commit -m "Initial commit: ChessMaster-UIT - Complete Chess Game with AI"
```

---

## 4️⃣ Push vers GitHub

```bash
# Pousser vers la branche main
git branch -M main
git push -u origin main
```

Si le dépôt existe déjà et contient des fichiers :

```bash
# Forcer le push (ATTENTION: écrase l'historique distant)
git push -u origin main --force
```

Ou fusionner avec l'existant :

```bash
# Récupérer les changements distants
git pull origin main --allow-unrelated-histories

# Résoudre les conflits si nécessaire
# Puis pousser
git push -u origin main
```

---

## 5️⃣ Organisation du projet sur GitHub

### Structure visible sur GitHub

Grâce au fichier `.vcxproj.filters`, Visual Studio organise les fichiers en dossiers virtuels :

- **📁 Source Files**
  - 📂 Application (GUI, CLI)
  - 📂 Domain (Entities, Services)
  - 📂 Infrastructure (Persistence, System)

- **📁 Header Files**
  - 📂 Entities
  - 📂 Rules
  - 📂 Services
  - 📂 GUI

- **📁 Resource Files**
  - 📂 Assets
  - 📂 fonts

### Fichiers physiques

Sur le système de fichiers (et GitHub), tous les fichiers `.cpp` et `.h` sont au même niveau dans le dossier racine, mais organisés logiquement par le `.vcxproj.filters`.

---

## 6️⃣ Vérifications importantes

### ✅ Fichiers inclus dans le commit

- ✔️ Tous les fichiers `.cpp` et `.h` (51 de chaque)
- ✔️ `ChessMasterUIT.vcxproj` et `.vcxproj.filters`
- ✔️ `.gitignore`
- ✔️ `README.md` (mise à jour)
- ✔️ `vcpkg.json`
- ✔️ `CMakeLists.txt`
- ✔️ Dossiers `assets/`, `pieces/`, `arial/`, `External/`

### ❌ Fichiers exclus (via .gitignore)

- ❌ Dossiers de build : `x64/`, `Debug/`, `Release/`
- ❌ Fichiers Visual Studio : `.vs/`, `*.user`, `*.suo`
- ❌ Fichiers générés : `*.db`, `*.bin`
- ❌ Fichiers de test : `AI_INTEGRATION_TEST.cpp`

---

## 7️⃣ Commandes Git utiles après le push

```bash
# Vérifier l'état du dépôt
git status

# Voir l'historique des commits
git log --oneline

# Ajouter des changements
git add fichier.cpp
git commit -m "Description du changement"
git push

# Créer une branche de fonctionnalité
git checkout -b feature/nouvelle-fonctionnalite

# Fusionner une branche
git checkout main
git merge feature/nouvelle-fonctionnalite
git push
```

---

## 🎯 Résultat attendu sur GitHub

Après le push, votre dépôt GitHub affichera :

```
Chess-Game/
├── .gitignore
├── README.md
├── vcpkg.json
├── CMakeLists.txt
├── ChessMasterUIT.vcxproj
├── ChessMasterUIT.vcxproj.filters
├── ChessMasterUIT.manifest
├── 51 fichiers .cpp
├── 51 fichiers .h
├── assets/
├── pieces/
├── arial/
└── External/sqlite/
```

---

## 📝 Notes importantes

1. **Taille du dépôt** : Les polices et assets représentent plusieurs Mo. GitHub accepte les fichiers jusqu'à 100 Mo.

2. **Fichiers sensibles** : Aucune information sensible (mots de passe, tokens) n'est présente dans le projet.

3. **Dépendances** : Les utilisateurs devront installer SFML via vcpkg avant de compiler.

4. **Organisation** : La structure en folders Virtual Studio est préservée via le `.vcxproj.filters`.

---

## ⚠️ Problèmes potentiels et solutions

### Problème : "Authentication failed"
**Solution** : Utiliser un Personal Access Token (PAT) au lieu du mot de passe
```bash
# Configurer le token
git remote set-url origin https://[YOUR-TOKEN]@github.com/abdelfatah-chaib/Chess-Game.git
```

### Problème : "Large files detected"
**Solution** : Vérifier la taille des assets, utiliser Git LFS si nécessaire
```bash
git lfs install
git lfs track "*.png"
git add .gitattributes
```

### Problème : "Merge conflicts"
**Solution** : Résoudre les conflits manuellement
```bash
git status  # Voir les fichiers en conflit
# Éditer les fichiers
git add fichier-resolu.cpp
git commit
```

---

## 🎉 Fin de la préparation !

Votre projet est maintenant prêt à être partagé sur GitHub avec :
- ✅ Code source propre et organisé
- ✅ Documentation complète (README)
- ✅ Configuration de build (vcxproj, CMake, vcpkg)
- ✅ Exclusion des fichiers inutiles (.gitignore)
- ✅ Structure claire et professionnelle

**Bon push ! 🚀**
