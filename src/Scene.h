#pragma once

#include "Module.h"
#include "Player.h"
#include <list>
#include <vector>
#include "Enemy.h"
#include "Item.h"
#include "GuiControlButton.h"

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

    bool OnGuiMouseClickEvent(GuiControl* control);
    void Settings();
    void DrawLives();
    void GameOver();

    // Returns the player instance
    Player* GetPlayer() const { return player; }

    // Changes the current level
    void ChangeLevel(int newLevel);

    // Toggles the help menu
    void ToggleMenu();

    void UpdateEnemyHitCount(std::string enemyName, int hitCount);
    void UpdateItem(std::string itemName, int isPicked);
    void CreateEnemies();
  
    void Credits();   
    void menu();
    void ToggleFullscreen();
    void funcion_menu_pause();


    bool limitFPS=false;
    // Public member variables
    int level = 0;                   // Current map level
    SDL_Texture* loadingScreen = NULL; //texture for the loading screen
    bool isLoading = false; // Bandera para mostrar pantalla de carga
    float loadingScreenDuration = 3100.0f; // Duración de la pantalla de carga (en segundos)
    float loadingTimer = 0.0f;    // Temporizador para la pantalla de carga
    float levelTime = 90000.0f;    // Tiempo total para completar el nivel (en segundos)
    float elapsedTime = 0.0f;    // Tiempo acumulado desde el inicio del nivel
    bool showRemainingTime = true;  // Mostrar tiempo restante o acumulado (true = restante, false = acumulado)
    bool showCredits = false;
    SDL_Texture* helpMenuTexture = NULL; // Texture for the help menu
    bool showHelpMenu = false;       // Whether the help menu is visible
    bool ToggleHelpMenu = false;     // Toggle flag for the help menu
    bool showingTransition = false;       // Whether the transition screen is visible
    bool isFlaged = false;
    SDL_Texture* menu_pause = NULL;
    SDL_Texture* tick = NULL;
    SDL_Texture* settings = NULL;
    SDL_Texture* black = NULL;
    SDL_Texture* blur = NULL;
    SDL_Texture* Title = NULL; // Texture for the main menu
    SDL_Texture* gameOver = NULL;
    bool showMainMenu = true;        // Whether the main menu is visible
    bool showGroupLogo =false;  
    bool timeUp = false;// Controla si se muestra el logo
    bool showSettings = false;
    bool manage_showSettings = false;
    bool showPauseMenu = false;

    float logoTimer = 0;                // Temporizador para el logo
    float logoDuration = 3000.0f;      // Duración del logo en segundos
    int pipeFxId = 0;                // Sound effect ID for pipe interaction
    int CastleFxId = 0;   
    int SelectFxId = 0;
    int SelectFxId2 = 0;
    int marioTime = 0;
    int hereWeGo = 0;
    int MenuStart = 0;// Sound effect ID for castle interaction
 
    int BowserLaugh = 0;
    float tolerance = 20.0f;         // Tolerance value for proximity checks
    GuiControlButton* guiBt;

    bool fullscreen_window = false;
    bool activatebotton7 = false;
    bool musicButtonHeld = false; // Detecta si el clic está siendo mantenido
    bool FxButtonHeld = false; // Detecta si el clic está siendo mantenido
    
    bool mouseOverMusicControl = false;
    bool mouseOverFxControl = false;
    int musicPosX = 1350;
    int FxPosX = 1350;
private:
    // Player instance
    Player* player;
    Item* item;

    std::vector<Enemy*> enemyList;
    std::vector<std::pair<std::string, int>> enemyStateList; //-> usar esta lista para almacenar el estado de los enemigos
    std::vector<std::pair<std::string, int>> itemStateList; 

    std::vector<int> hitcountList;

    // Transition-related variables
    SDL_Texture* level1Transition = NULL; // Texture for Level 1 transition
    SDL_Texture* level2Transition = NULL; // Texture for Level 2 transition
    SDL_Texture* GroupLogo = NULL;
    int gameIntroMusicId = 0; // Identificador de la música GameIntro
    int menuMusicId = 0; // Identificador de la música MenuMusic
    float transitionDuration = 3100;      // Duration of the transition screen (in ms)
    float transitionTimer = 0.0f;         // Timer for tracking transition time
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
    void StartNewGame();
    void LoadGame();

    Vector2D mousePos;
};