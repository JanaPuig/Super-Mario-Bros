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
    void InitialItems();
    void InitialEnemies(bool useXMLPosition);

    void CreateLevelItems(int level);
    void CreateEnemies(int level);

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

    // Guardar Escena
    void SaveState();

    bool OnGuiMouseClickEvent(GuiControl* control);
    void Settings();
    void DrawLives();
    void DrawObject();
    void DrawWorld();
    void DrawPuntation();
    void GameOver();

    // Returns the player instance
    Player* GetPlayer() const { return player; }

    // Changes the current level
    void ChangeLevel(int newLevel);

    // Toggles the help menu
    void ToggleMenu();

    void UpdateEnemyHitCount(std::string enemyName, int hitCount);
    void UpdateItem(std::string itemName, int isPicked);

    void Credits();
    void menu();
    void ToggleFullscreen();
    void MenuPause();

    // Public member variables
    bool bossFightActive = false;
    bool showWinScreen = false;
    bool isLoading = false; // Bandera para mostrar pantalla de carga
    bool limitFPS = false;
    bool showRemainingTime = true;  // Mostrar tiempo restante o acumulado (true = restante, false = acumulado)
    bool showCredits = false;
    bool showHelpMenu = false;       // Whether the help menu is visible
    bool ToggleHelpMenu = false;     // Toggle flag for the help menu
    bool showingTransition = false;       // Whether the transition screen is visible
    bool isFlaged = false;
    bool showMainMenu = true;        // Whether the main menu is visible
    bool showGroupLogo = false;
    bool timeUp = false;// Controla si se muestra el logo
    bool showSettings = false;
    bool manageShowSettings = false;
    bool showPauseMenu = false;
    bool hasPlayedWinMusic;
    bool fullscreen_window = false;
    bool activatebotton7 = false;
    bool musicButtonHeld = false; // Detecta si el clic está siendo mantenido
    bool FxButtonHeld = false; // Detecta si el clic está siendo mantenido
    bool mouseOverMusicControl = false;
    bool mouseOverFxControl = false;

    SDL_Texture* loadingScreen = NULL; 
    SDL_Texture* helpMenuTexture = NULL; 
    SDL_Texture* menu_pause = NULL;
    SDL_Texture* tick = NULL;
    SDL_Texture* settings = NULL;
    SDL_Texture* black = NULL;
    SDL_Texture* blur = NULL;
    SDL_Texture* Title = NULL; 
    SDL_Texture* gameOver = NULL;

    float tolerance = 20.0f;       
    float loadingScreenDuration = 3100.0f; 
    float loadingTimer = 0.0f;    
    float levelTime = 90000.0f;  
    float elapsedTime = 0.0f;    
    float logoTimer = 0;               
    float logoDuration = 3000.0f;     

    int pipeFxId = 0;                // Sound effect ID for pipe interaction
    int CastleFxId = 0;
    int SelectFxId = 0;
    int SelectFxId2 = 0;
    int marioTime = 0;
    int hereWeGo = 0;
    int MenuStart = 0;// Sound effect ID for castle interaction
    int level = 0;                   // Current map level
    int BowserLaugh = 0;
    int sdlVolume;
    int musicPosX = 1350;
    int FxPosX = 1350;
    int EndGame = 0;

private:

    void ShowTransitionScreen();
    void FinishTransition();
    void StartNewGame();
    void LoadGame();

    // Player instance
    Player* player;

    Enemy* enemy;
    //Item Instance
    Item* item;

    //Private Lists
    std::vector<int> hitcountList;
    std::vector<std::pair<std::string, int>> enemyStateList; 
    std::vector<std::pair<std::string, int>> itemStateList;
    std::vector<Item*> itemList;
    std::vector<Enemy*> enemyList;

    int gameIntroMusicId = 0; 
    int menuMusicId = 0; 
    int selectedOption = 0; 

    float transitionDuration = 3100;     
    float transitionTimer = 0.0f;        


    bool isGameIntroPlaying = false; 

    //Private Textures
    SDL_Texture* GroupLogo = NULL;
    SDL_Texture* mainMenu = NULL;

    Vector2D mousePos;

    GuiControlButton* guiBt;
    int isSave = 0;

    int save_hitCount_koopa = 0; //Saber el hitcount de los enemigos cuando se hace un save
    int save_hitCount_koopa2 = 0;
    int save_hitCount_goomba = 0;
    int save_hitCount_goomba2 = 0;
    int save_hitCount_bowser = 0;



    std::vector<Vector2D> checkpoints;
    int currentCheckpointIndex = 0;
};