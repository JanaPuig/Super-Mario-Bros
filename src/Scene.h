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

    bool Awake();

    void InitialItems();

    void InitialEnemies(bool useXMLPosition);

    void CreateLevelItems(int level);

    void CreateEnemies(int level);

    bool Start();

    bool PreUpdate();

    bool Update(float dt);

    void HandleTeleport(const Vector2D& playerPos);

    bool IsInTeleportArea(const Vector2D& playerPos, float x, float y, float tolerance);

    bool PostUpdate();

    bool CleanUp();

    Vector2D GetPlayerPosition();

    void UpdateEnemyHitCount(std::string enemyName, int hitCount);

    void UpdateItem(std::string itemName, int isPicked);

    void SaveState();

    bool OnGuiMouseClickEvent(GuiControl* control);

    void Settings();

    void DrawLives();

    void DrawObject();

    void DrawWorld();

    void DrawPuntation();

    void GameOver();

    void ChangeLevel(int newLevel);

    void ToggleMenu();

    void Credits();

    void menu();

    void ToggleFullscreen();

    void MenuPause();

    Player* GetPlayer() const { return player; }

    // Public member variables
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
    bool manageShowSettings = false;
    bool showPauseMenu = false;
    bool hasPlayedWinMusic;
    bool fullscreen_window = false;
    bool activatebotton7 = false;
    bool musicButtonHeld = false; 
    bool FxButtonHeld = false; 
    bool mouseOverMusicControl = false;
    bool mouseOverFxControl = false;
    bool isMusicPlaying = false;

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

    int pipeFxId = 0;           
    int CastleFxId = 0;
    int SelectFxId = 0;
    int SelectFxId2 = 0;
    int marioTime = 0;
    int hereWeGo = 0;
    int MenuStart = 0;
    int level = 0;                 
    int BowserLaugh = 0;
    int sdlVolume;
    int musicPosX = 1350;
    int FxPosX = 1350;
    int EndGame = 0;

    bool endFinalBoss=false;

private:

    void ShowTransitionScreen();
    void FinishTransition();
    void StartNewGame();
    void LoadGame();

    //Instances
    Player* player;
    Enemy* enemy;
    Item* item;

    //Lists
    std::vector<int> hitcountList;
    std::vector<std::pair<std::string, int>> enemyStateList; 
    std::vector<std::pair<std::string, int>> itemStateList;
    std::vector<Item*> itemList;
    std::vector<Enemy*> enemyList;

    int gameIntroMusicId = 0; 
    int menuMusicId = 0; 
    int selectedOption = 0; 
    int isSave = 0;

    float transitionDuration = 3100;     
    float transitionTimer = 0.0f;        

    bool isGameIntroPlaying = false; 

    //Private Textures
    SDL_Texture* GroupLogo = NULL;
    SDL_Texture* mainMenu = NULL;

    Vector2D mousePos;

    GuiControlButton* guiBt;

    int save_hitCount_koopa = 0; 
    int save_hitCount_koopa2 = 0;
    int save_hitCount_goomba = 0;
    int save_hitCount_goomba2 = 0;
    int save_hitCount_bowser = 0;

    //Checkpoints
    std::vector<Vector2D> checkpoints;
    int currentCheckpointIndex = -1;
};