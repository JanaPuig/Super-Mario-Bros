# Super Mario Bros 
![SuperMarioBros Logo](https://upload.wikimedia.org/wikipedia/commons/2/2b/Super_Mario_Bros._Logo.svg)

## About the game
_Super Mario Bros_ is a platform video game developed and published by Nintendo in 1985. The game follows the adventures of Mario, a plumber who tries to rescue Princess Peach from the evil Bowser. It is considered one of the most iconic titles in video game history and a cornerstone of the platformer genre.

## Our Objective
Our objective is to recreate the first level of the original video game using the C++ programming language and the Box2D physics engine in Visual Studio.

## Gameplay
The player controls Mario (or his brother Luigi in multiplayer mode) through levels filled with enemies and obstacles. Players must move sideways, jump, and run to avoid dangers, collect coins, and unlock power-ups like the mushroom that increases Mario's size or the fire flower that allows him to throw fireballs. 
For now, only level 1 has been implemented, but the levels include different types of terrain, such as grasslands, underground caves, aquatic worlds, and castles filled with traps.

## Controls
* Move left/right: A/D
* Jump: Space/W
* Run: Left Control
* Revive when dead: T

### Debugging Controls
* H Shows / hides a help menu showing the keys
* F1/F2 Change to Level 1 & 2
* F3 Restart the level
* F5/F6 Save & Load the game
* F7 Travel across checkpoints
* F8 Show/Hide Buttons Boundaries
* F9 Visualize the colliders / logic / paths
* F10 God Mode (fly around, cannot be killed)
* F11 Enable/Disable FPS cap to 30

## List of Features Implemented Assignment 1 
* Tiled background
* Coin Grab
* Pipes Teleport
* Castle Teleport to start
* Camera follows the player + camera limits
* Music: "GroundTheme"
* Jump Fx when jumping
* Coin Fx when grabbing a coin
* Pipe Fx when teleporting in and out of pipes
* Castle Fx when entering the castle
* Debug: Show/hide collisions with F9
* Debug: 30 fps cap with F11
* Debug: God Mode with F10
* Debug: Show/hide Help Menu with H
* Jumping animation
* Walking animation
* Idle animation
* Main Menu/Title
* Death collisions and logic
* Death Fx and Voice Fx
* Random jump Voice Fx
* Start Voice Fx
* Death animation
* Game Over screen
* Try Again logic
* Double Jump

## List of Features Implemented Assignment 2 
* Pathfinding
* Enemies
  * Goomba movement & Animations
  * Koopa movement & Animations
  * Enemies Death Fx
* Positions
  * Save & Load player position
  * Save & Load enemy position
* Checkpoints
  * Flag texture (checkpoint)
  * Checkpoint functionality
  * Flag Fx
  * Save & Load Checkpoint State
* Textures
  * Add all game textures (config xml)
* Game Menu
  * New Game (Deletes previous Saved State)
  * Load (Loads previous Saved State)
  * Exit
  * Menu Music
  * Menu Fx
* Levels
  * Level 2 implementation (no enemies yet)
  * Shortcuts to switch levels:
    * F1: Switch to level 1
    * F2: Switch to level 2
  * F3: Reset current Level
  * Save & Load level
* Advanced Options
  * F5: Save state (even across levels)
  * F6: Load the previous state (even across levels)

## List of Features Implemented Assignment 3 
* Enemies
  * Bowser Movement & Animations
  * Enemies on each level
* Items
  * Big Coin
  * One Up 
  * Star
* Save & Load
  * Player position & state
  * Enemy position & state
  * Items position & state
* Levels
  * Level 3 Implementation
  * Shortcuts to switch levels:
    * F1: Switch to level 1
    * F2: Switch to level 2
    * F3: Reset current level
    * F4: Switch to level 4
* GUI 
  * State buttons: (display, normal, press and focus)
  * Intro screen
  * Screen HUD
    * Elements:
      * Player lives
      * Coins
      * Timer
      * Score   
  * Menus:
    * Main menu
      * Buttons:
         * Play
         * Load Game
         * Settings
         * Credits
         * Exit
    * Pause Menu
      * Buttons:
         * Resmue
         * Settings
         * Back to title
         * Exit

* Optimitzation (optimization techniques)
  * Frustrum culling
  * Lazy updates
  * Pathfinding search (Navegation mersh: A*)
* Advanced Options
  * F7: Teleport to Next checkpoint
  * F8: Show/Hide buttons boundaries
* Music & Sound Fx
  * Level 3 Music
  * Boss Fight Music
  * Star Power Music
  * Power Up Fx
  * Power Down Fx
  * Bowser dies Fx
  * Bowser Laught Fx
  * One Up Fx
  * Big Coin Fx
  * You Win Music
  * Sound buttons

## Team Members
### Jana Puig & Toni Llovera
## Our Latest Release & Wiki: [Click Here](https://github.com/JanaPuig/Super-Mario-Bros/wiki/Release)
