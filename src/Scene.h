#pragma once

#include "Module.h"
#include "Player.h"

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
    void CreateLevel1Items();

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

    // Returns the player instance
    Player* GetPlayer() const { return player; }

    // Changes the current level
    void ChangeLevel(int newLevel);

    // Toggles the help menu
    void ToggleMenu();

    // Public member variables
    int level = 0;                   // Current map level

    SDL_Texture* helpMenuTexture = NULL; // Texture for the help menu
    bool showHelpMenu = false;       // Whether the help menu is visible
    bool ToggleHelpMenu = false;     // Toggle flag for the help menu

    SDL_Texture* Title = NULL;    // Texture for the main menu
    bool showMainMenu = true;        // Whether the main menu is visible

    int pipeFxId = 0;                // Sound effect ID for pipe interaction
    int CastleFxId = 0;   
    int SelectFxId = 0;
    int SelectFxId2 = 0;
    int MenuStart = 0;// Sound effect ID for castle interaction
    float tolerance = 20.0f;         // Tolerance value for proximity checks

private:
    // Player instance
    Player* player;

    // Transition-related variables
    SDL_Texture* level1Transition = NULL; // Texture for Level 1 transition
    SDL_Texture* level2Transition = NULL; // Texture for Level 2 transition

    int gameIntroMusicId = 0; // Identificador de la música GameIntro
    int menuMusicId = 0; // Identificador de la música MenuMusic
    float transitionDuration = 3100;      // Duration of the transition screen (in ms)
    float transitionTimer = 0.0f;         // Timer for tracking transition time
    bool showingTransition = false;       // Whether the transition screen is visible
    bool isGameIntroPlaying = false; // Bandera para verificar si GameIntro está sonando
    // Displays the transition screen
    void ShowTransitionScreen();

    // Finalizes the transition process
    void FinishTransition();
    SDL_Texture* mainMenu = NULL;
    SDL_Texture* newGameButton = NULL;  // Textura para el botón Nuevo Juego
    SDL_Texture* loadGameButton = NULL; // Textura para el botón Cargar Juego
    SDL_Texture* leaveGameButton = NULL; // Textura para el botón Salir
    SDL_Texture* newGameButtonSelected = NULL;  
    SDL_Texture* loadGameButtonSelected = NULL; 
    SDL_Texture* leaveGameButtonSelected = NULL;
    int selectedOption = 0; // Opción seleccionada (0: Nuevo Juego, 1: Cargar Juego, 2: Salir)

    // Métodos para manejar la selección de opciones
    void HandleMainMenuSelection();
    void StartNewGame();
    void LoadGame();
};