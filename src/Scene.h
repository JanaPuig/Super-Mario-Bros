#pragma once

#include "Module.h"
#include "Player.h"
#include <list>
#include <vector>
#include "Enemy.h"

// Forward declaration of SDL_Texture structure
struct SDL_Texture;

// Scene class, derived from Module
class Scene : public Module
{
public:
    // Constructor
    Scene();

    // Destructor
    virtual ~Scene();

    // Called before render is available
    bool Awake();

    // Creates items specific to Level 1
    void CreateLevelItems();

    // Called before the first frame
    bool Start();

    // Called before all updates
    bool PreUpdate();

    // Called each loop iteration
    bool Update(float dt);

    // Handles teleportation logic
    void HandleTeleport(const Vector2D& playerPos);

    // Checks if the player is within a teleportation area
    bool IsInTeleportArea(const Vector2D& playerPos, float x, float y, float tolerance);

    // Called after all updates
    bool PostUpdate();

    // Called before quitting
    bool CleanUp();

    // Return the player position
    Vector2D GetPlayerPosition();
    // Cargar Escena
    void LoadState();
    // Guardar Escena
    void SaveState();

    // Returns the player instance
    Player* GetPlayer() const { return player; }

    // Changes the current level
    void ChangeLevel(int newLevel);

    // Toggles the help menu
    void ToggleMenu();

    void UpdateEnemyHitCount(std::string enemyName, int hitCount);

    // Public member variables
    int level = 0;                   // Current map level
    SDL_Texture* loadingScreen = NULL; //texture for the loading screen
    bool isLoading = false; // Bandera para mostrar pantalla de carga
    float loadingScreenDuration = 3100.0f; // Duraci�n de la pantalla de carga (en segundos)
    float loadingTimer = 0.0f;    // Temporizador para la pantalla de carga


    SDL_Texture* helpMenuTexture = NULL; // Texture for the help menu
    bool showHelpMenu = false;       // Whether the help menu is visible
    bool ToggleHelpMenu = false;     // Toggle flag for the help menu
    bool showingTransition = false;       // Whether the transition screen is visible
    bool isFlaged = false;

    SDL_Texture* Title = NULL; // Texture for the main menu
    SDL_Texture* gameOver = NULL;
    bool showMainMenu = true;        // Whether the main menu is visible

    int pipeFxId = 0;                // Sound effect ID for pipe interaction
    int CastleFxId = 0;   
    int SelectFxId = 0;
    int SelectFxId2 = 0;
    int marioTime = 0;
    int hereWeGo = 0;
    int MenuStart = 0;// Sound effect ID for castle interaction
    float tolerance = 20.0f;         // Tolerance value for proximity checks

private:
    // Player instance
    Player* player;
    //Enemy* enemy;
    //std::vector<Enemy*> enemyList;
    std::vector<std::pair<std::string, int>> enemyStateList; //-> usar esta lista para almacenar el estado de los enemigos

    std::vector<int> hitcountList;


    // Transition-related variables
    SDL_Texture* level1Transition = NULL; // Texture for Level 1 transition
    SDL_Texture* level2Transition = NULL; // Texture for Level 2 transition

    int gameIntroMusicId = 0; // Identificador de la m�sica GameIntro
    int menuMusicId = 0; // Identificador de la m�sica MenuMusic
    float transitionDuration = 3100;      // Duration of the transition screen (in ms)
    float transitionTimer = 0.0f;         // Timer for tracking transition time

    bool isGameIntroPlaying = false; // Bandera para verificar si GameIntro est� sonando
    // Displays the transition screen
    void ShowTransitionScreen();

    // Finalizes the transition process
    void FinishTransition();
    SDL_Texture* mainMenu = NULL;
    SDL_Texture* newGameButton = NULL;  // Textura para el bot�n Nuevo Juego
    SDL_Texture* loadGameButton = NULL; // Textura para el bot�n Cargar Juego
    SDL_Texture* leaveGameButton = NULL; // Textura para el bot�n Salir
    SDL_Texture* newGameButtonSelected = NULL;  
    SDL_Texture* loadGameButtonSelected = NULL; 
    SDL_Texture* leaveGameButtonSelected = NULL;
    int selectedOption = 0; // Opci�n seleccionada (0: Nuevo Juego, 1: Cargar Juego, 2: Salir)

    // M�todos para manejar la selecci�n de opciones
    void HandleMainMenuSelection();
    void StartNewGame();
    void LoadGame();

};