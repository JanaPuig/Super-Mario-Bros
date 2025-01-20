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
    Scene();                     
    virtual ~Scene();          

    // Module lifecycle methods
    bool Awake();                
    bool Start();               
    bool PreUpdate();            
    bool Update(float dt);       
    bool PostUpdate();           
    bool CleanUp();             

    // Gameplay methods
    void InitialItems();
    void InitialEnemies(bool useXMLPosition);
    void CreateLevelItems(int level);
    void CreateEnemies(int level);
    void HandleTeleport(const Vector2D& playerPos);
    bool IsInTeleportArea(const Vector2D& playerPos, float x, float y, float tolerance);
    Vector2D GetPlayerPosition();
    void SaveState();

    // UI and rendering methods
    void Settings();
    void DrawLives();
    void DrawObject();
    void DrawWorld();
    void DrawPuntation();
    void GameOver();
    void Credits();
    void menu();
    void ToggleFullscreen();
    void funcion_menu_pause();

    // Interaction and state management
    bool OnGuiMouseClickEvent(GuiControl* control);
    void UpdateEnemyHitCount(std::string enemyName, int hitCount);
    void UpdateItem(std::string itemName, int isPicked);
    void ChangeLevel(int newLevel);
    void ToggleMenu();

    // Getters
    Player* GetPlayer() const { return player; }

    // Flags
    bool bossFightActive = false;
    bool showWinScreen = false;
    bool isLoading = false;
    bool limitFPS = false;
    bool showRemainingTime = true;
    bool showCredits = false;
    bool showHelpMenu = false;
    bool ToggleHelpMenu = false;
    bool showingTransition = false;
    bool isFlaged = false;
    bool showMainMenu = true;
    bool showGroupLogo = false;
    bool timeUp = false;
    bool showSettings = false;
    bool manage_showSettings = false;
    bool showPauseMenu = false;
    bool hasPlayedWinMusic = false;
    bool fullscreen_window = false;
    bool activatebotton7 = false;
    bool musicButtonHeld = false;
    bool FxButtonHeld = false;
    bool mouseOverMusicControl = false;
    bool mouseOverFxControl = false;
    bool isGameIntroPlaying = false; 

    // Textures
    SDL_Texture* loadingScreen = nullptr;
    SDL_Texture* helpMenuTexture = nullptr;
    SDL_Texture* menu_pause = nullptr;
    SDL_Texture* tick = nullptr;
    SDL_Texture* settings = nullptr;
    SDL_Texture* black = nullptr;
    SDL_Texture* blur = nullptr;
    SDL_Texture* Title = nullptr;
    SDL_Texture* gameOver = nullptr;

    // Timers and durations
    float tolerance = 20.0f;
    float loadingScreenDuration = 3100.0f;
    float loadingTimer = 0.0f;
    float levelTime = 90000.0f;
    float elapsedTime = 0.0f;
    float logoTimer = 0.0f;
    float logoDuration = 3000.0f;
    float transitionDuration = 3100.0f;
    float transitionTimer = 0.0f;
    float currentTime = 0;
    const float tolerance = 20.0f;

    //Texts
    char timerText[64];

    // Sound effects
    int pipeFxId = 0;
    int CastleFxId = 0;
    int SelectFxId = 0;
    int SelectFxId2 = 0;
    int marioTime = 0;
    int hereWeGo = 0;
    int MenuStart = 0;
    int BowserLaugh = 0;
    int EndGame = 0;
    int gameIntroMusicId = 0;
    int menuMusicId = 0;
    int selectedOption = 0;

    // Audio settings
    int sdlVolume = 0;
    int musicPosX = 1350;
    int FxPosX = 1350;

    // Level state
    int level = 0;                  

    // Private methods
private:
    void ShowTransitionScreen();
    void FinishTransition();
    void StartNewGame();
    void LoadGame();

    // Player and game objects
    Player* player = nullptr;
    Enemy* enemy = nullptr;
    Item* item = nullptr;

    // Game state lists
    std::vector<int> hitcountList;
    std::vector<std::pair<std::string, int>> enemyStateList;
    std::vector<std::pair<std::string, int>> itemStateList;
    std::vector<Item*> itemList;
    std::vector<Enemy*> enemyList;

    // Private textures
    SDL_Texture* level1Transition = nullptr;
    SDL_Texture* level2Transition = nullptr;
    SDL_Texture* GroupLogo = nullptr;
    SDL_Texture* mainMenu = nullptr;
    SDL_Texture* newGameButton = nullptr;
    SDL_Texture* loadGameButton = nullptr;
    SDL_Texture* leaveGameButton = nullptr;
    SDL_Texture* newGameButtonSelected = nullptr;
    SDL_Texture* loadGameButtonSelected = nullptr;
    SDL_Texture* leaveGameButtonSelected = nullptr;

    Vector2D mousePos;

    // GUI controls
    GuiControlButton* guiBt = nullptr;
    int isSave = 0;

    // Saved enemy hit counts
    int save_hitCount_koopa = 0;
    int save_hitCount_koopa2 = 0;
    int save_hitCount_goomba = 0;
    int save_hitCount_goomba2 = 0;
    int save_hitCount_bowser = 0;
};