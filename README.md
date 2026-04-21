# Hearted - 2D RPG-Like Game

## 🎮 About the Project
"Hearted" is a custom-built 2D RPG-like game developed entirely in C++ using the SFML library. Featuring completely hand-drawn pixel art, the game combines narrative exploration with a unique, hybrid combat system. 
Under the hood, the project is powered by a highly scalable, custom-written location and bounding-box engine, making it incredibly easy to expand the game world, add new interactive objects, and script cutscenes.

## ✨ Key Features
* **Custom Location & Hitbox Engine:** The game world is built on a robust, scalable engine that handles multiple types of bounding boxes:
  * `CollisionBox`: Solid environmental boundaries.
  * `TransitionBox`: Seamlessly teleports the player between different maps/rooms.
  * `InteractiveBox`: Triggers object inspections and loot discovery (e.g., finding keys or items).
  * `InnitiateBox`: Triggers scripted events and cinematic cutscenes (like dodging traffic).
* **Hybrid Combat System:** Features a unique battle mechanic inspired by bullet-hell RPGs. Players have turn-based options (Attack, Escape, Use Heal) combined with a real-time defense phase where they must maneuver a "heart" inside a bounding box to dodge incoming projectile attacks.
* **RPG Mechanics:** Full player progression system including XP, Leveling, Base HP, and Damage scaling.
  * An inventory system with finite capacity that handles items like Health Potions, Swords, and Keys.
  * In-game economy and a fully functional Shop.
* **Cinematic Storytelling:** Includes a typewriter-style dialogue system for interacting with objects and experiencing scripted cutscenes.
* **Custom Animation Handler:** A dedicated frame-by-frame animation class built from scratch to parse and render directional character movements effortlessly.

## 🛠 Technologies
* **Language:** C++
* **Graphics/Audio Library:** SFML (Simple and Fast Multimedia Library)
* **Concepts:** Advanced OOP, State Management, Custom Physics/Collision Detection, Generics (std::vector, std::map)

## 🚀 How to Build and Run

To run the game, you must clone the entire repository to get all the necessary hand-drawn assets and fonts.
1. **Clone the repository:**
   ```bash
   git clone [https://github.com/YevhenKoval01/RPG-game]
2. **Prerequisites:** Ensure you have SFML installed. The provided CMakeLists.txt will automatically find and link it.
3. **Fix Asset Paths:** Currently, asset paths in main.cpp are hardcoded to a local machine (e.g., C:\\Users\\QWERTY\\...). Before building, please update these to relative paths so the game can correctly load textures and fonts.
4. **Build and Run:**
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
