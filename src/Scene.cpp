#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"
#include "Player.h"

using namespace std;

// Constructor: Initializes the Scene object and default values
Scene::Scene()
    : Module(), level(1), showHelpMenu(false), showMainMenu(true), ToggleHelpMenu(false)
{
    name = "scene";
}

// Destructor
Scene::~Scene() {}

// Called during the initialization phase
bool Scene::Awake()
{
    LOG("Loading Scene");

    if (level == 1 || level == 2) 
    {
        // Create the player entity
        player = static_cast<Player*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));

       // Load enemy configurations from XML and initialize them
        for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy");
            enemyNode;
            enemyNode = enemyNode.next_sibling("enemy"))
        {
            Enemy* enemy = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
            enemy->SetParameters(enemyNode);
        }
    }

    // Load level-specific items
    if (level == 1) {
        CreateLevel1Items();
    }

    return true;
}

// Creates items for Level 1
void Scene::CreateLevel1Items()
{
    if (level == 1) {
        const int startX = 1600, startY = 768;
        const int numItems = 7, spacing = 32;
        const int numRows = 3, rowSpacing = 64;

        for (int row = 0; row < numRows; ++row) {
            for (int i = 0; i < numItems; ++i) {
                Item* item = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
                item->position = Vector2D(startX + i * spacing, startY - row * rowSpacing);

                // Log the item's creation
                LOG("Creating item at position: (%f, %f)", item->position.getX(), item->position.getY());
            }
        }
    }
}

// Called before the first frame
bool Scene::Start()
{
    Engine::GetInstance().map->Load("Assets/Maps/", "Background.tmx");
    Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav", 0.5f);

    // Load sound effects
    pipeFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Pipe.wav");
    CastleFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Music/StageClear_Theme.wav");

    // Load textures for menus and transitions
    mainMenu = Engine::GetInstance().textures.get()->Load("Assets/Textures/mainMenu.png");
    Title = Engine::GetInstance().textures.get()->Load("Assets/Textures/title.png");
    helpMenuTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Help_Menu.png");
    level1Transition = Engine::GetInstance().textures.get()->Load("Assets/Textures/world1-1.png");
    level2Transition = Engine::GetInstance().textures.get()->Load("Assets/Textures/world1-2.png");
    newGameButtonSelected = Engine::GetInstance().textures.get()->Load("Assets/Textures/NewGameButtonSelected.png");
    loadGameButtonSelected = Engine::GetInstance().textures.get()->Load("Assets/Textures/LoadGameButtonSelected.png");
    leaveGameButtonSelected = Engine::GetInstance().textures.get()->Load("Assets/Textures/LeaveGameButtonSelected.png");
    newGameButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/NewGameButton.png");
    loadGameButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/LoadGameButton.png");
    leaveGameButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/LeaveGameButton.png");
    return true;
}

// Called before each frame update
bool Scene::PreUpdate()
{
    return true;
}

// Changes the current level
void Scene::ChangeLevel(int newLevel)
{
    if (level == newLevel) return;

    LOG("Changing level from %d to %d", level, newLevel);

    //Remove enemy
    Engine::GetInstance().entityManager->RemoveAllEnemies();

    // Remove all items and clean up the current map
    Engine::GetInstance().entityManager->RemoveAllItems();
    Engine::GetInstance().map.get()->CleanUp();
    level = newLevel;

    // Show the transition screen
    ShowTransitionScreen();
}

// Main update logic for the Scene
bool Scene::Update(float dt)
{
    int cameraX = Engine::GetInstance().render.get()->camera.x;
    int cameraY = Engine::GetInstance().render.get()->camera.y;

    // Si estamos en el menú principal, no se procesan otras lógicas
    if (showMainMenu) {
        // Dibujar el fondo del menú principal
        Engine::GetInstance().render.get()->DrawTexture(mainMenu, -cameraX, -cameraY-100);

        // Dibujar los botones con la textura correcta según la opción seleccionada
        if (selectedOption == 0) {
            Engine::GetInstance().render.get()->DrawTexture(newGameButtonSelected, -cameraX + 730, -cameraY + 350);
        }
        else {
            Engine::GetInstance().render.get()->DrawTexture(newGameButton, -cameraX + 730, -cameraY + 350);
        }

        if (selectedOption == 1) {
            Engine::GetInstance().render.get()->DrawTexture(loadGameButtonSelected, -cameraX + 710, -cameraY + 500);
        }
        else {
            Engine::GetInstance().render.get()->DrawTexture(loadGameButton, -cameraX + 710, -cameraY + 500);
        }

        if (selectedOption == 2) {
            Engine::GetInstance().render.get()->DrawTexture(leaveGameButtonSelected, -cameraX + 690, -cameraY + 650);
        }
        else {
            Engine::GetInstance().render.get()->DrawTexture(leaveGameButton, -cameraX + 690, -cameraY + 650);
        }

        // Manejar la selección de opciones
        HandleMainMenuSelection();

        return true; // Evita que se ejecute el código del resto del juego mientras el menú esté activo
    }
    // Handle level transition screen
    if (showingTransition) {
        transitionTimer += dt;

        Engine::GetInstance().render.get()->DrawTexture(
            (level == 1) ? level1Transition : level2Transition, -cameraX, -cameraY
        );

        if (transitionTimer >= transitionDuration) {
            FinishTransition();
        }
        return true; // Skip the game logic during transition
    }

    // Debug controls for level changes
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
        ChangeLevel(2);
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
        ChangeLevel(1);
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
        player->SetPosition(level == 1 ? Vector2D(3, 9) : Vector2D(3, 14.5));
        ShowTransitionScreen();
    }

    Vector2D playerPos = player->GetPosition();

    // Handle help menu toggle
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN && !ToggleHelpMenu) {
        ToggleMenu();
        ToggleHelpMenu = true;
    }
    if (showHelpMenu) {
        int cameraX = Engine::GetInstance().render.get()->camera.x;
        int cameraY = Engine::GetInstance().render.get()->camera.y;
        Engine::GetInstance().render.get()->DrawTexture(helpMenuTexture, -cameraX, -cameraY + 352);
        Engine::GetInstance().render.get()->DrawTexture(Title, -cameraX - 275, -cameraY - 250);
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_UP) {
        ToggleHelpMenu = false;
    }

    // Camera follows the player
    const float cameraMinX = 0.0f, cameraMaxX = 4862.0f, cameraFollowOffset = 192.0f;
    float desiredCamPosX = playerPos.getX() - Engine::GetInstance().render.get()->camera.w / 2 + cameraFollowOffset;
    desiredCamPosX = std::max(cameraMinX, std::min(cameraMaxX, desiredCamPosX));
    Engine::GetInstance().render.get()->camera.x = -desiredCamPosX;

    // Handle teleportation in Level 1
    if (level == 1) {
        HandleTeleport(playerPos);
    }

    return true;
}

// Handles teleportation logic
void Scene::HandleTeleport(const Vector2D& playerPos)
{
    const float tolerance = 5.0f;

    if (IsInTeleportArea(playerPos, 1440, 290, tolerance) ||
        IsInTeleportArea(playerPos, 1472, 288, tolerance))
    {
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
            Engine::GetInstance().audio.get()->PlayFx(pipeFxId);
            player->SetPosition(Vector2D(31, 11));
        }
    }
    else if (IsInTeleportArea(playerPos, 1856, 864, tolerance)) {
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
            player->SetPosition(Vector2D(36, 6));
            Engine::GetInstance().audio.get()->PlayFx(pipeFxId);
        }
    }
    else if (IsInTeleportArea(playerPos, 6527, 416, tolerance)) {
        Engine::GetInstance().audio.get()->PlayFx(CastleFxId);
        ChangeLevel(2);
    }
}

// Checks if the player is in a teleportation area
bool Scene::IsInTeleportArea(const Vector2D& playerPos, float x, float y, float tolerance)
{
    return playerPos.getX() >= x - tolerance && playerPos.getX() <= x + tolerance &&
        playerPos.getY() >= y - tolerance && playerPos.getY() <= y + tolerance;
}

// Post-update logic
bool Scene::PostUpdate()
{
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
        return false;
    }
    return true;
}

// Cleans up the scene
bool Scene::CleanUp()
{
    Engine::GetInstance().textures.get()->UnLoad(mainMenu);
    Engine::GetInstance().textures.get()->UnLoad(newGameButton);
    Engine::GetInstance().textures.get()->UnLoad(loadGameButton);
    Engine::GetInstance().textures.get()->UnLoad(leaveGameButton);
    Engine::GetInstance().textures.get()->UnLoad(newGameButtonSelected); 
    Engine::GetInstance().textures.get()->UnLoad(loadGameButtonSelected); 
    Engine::GetInstance().textures.get()->UnLoad(leaveGameButtonSelected);
    Engine::GetInstance().textures.get()->UnLoad(helpMenuTexture);
    Engine::GetInstance().textures.get()->UnLoad(Title);
    Engine::GetInstance().textures.get()->UnLoad(level1Transition);
    Engine::GetInstance().textures.get()->UnLoad(level2Transition);
    Engine::GetInstance().audio.get()->StopMusic();
    LOG("Freeing scene");
    return true;
}

// Toggles the help menu visibility
void Scene::ToggleMenu()
{
    showHelpMenu = !showHelpMenu;
    LOG(showHelpMenu ? "SHOWING MENU" : "UNSHOWING MENU");
}

// Shows the transition screen
void Scene::ShowTransitionScreen()
{
    showingTransition = true;
    transitionTimer = 0.0f;

    // Disable the player during the transition
    if (player != nullptr) {
        player->SetActive(false);
    }

    Engine::GetInstance().audio.get()->StopMusic();

    int cameraX = Engine::GetInstance().render.get()->camera.x;
    int cameraY = Engine::GetInstance().render.get()->camera.y;

    Engine::GetInstance().render.get()->DrawTexture(
        (level == 1) ? level1Transition : level2Transition, -cameraX, -cameraY
    );
}

// Finishes the transition and loads the next level
void Scene::FinishTransition()
{
    showingTransition = false;

    // Reactivate the player after transition
    if (player != nullptr) {
        player->SetActive(true);
    }

    // Load the respective level
    if (level == 1) {
        player->SetPosition(Vector2D(3, 8.3));
        Engine::GetInstance().map->Load("Assets/Maps/", "Background.tmx");
        CreateLevel1Items();
    }
    else if (level == 2) {
        player->SetPosition(Vector2D(3, 14.5));
        Engine::GetInstance().map->Load("Assets/Maps/", "Map2.tmx");
    }

    // Resume music after transition
    Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav", 0.5f);
}
void Scene::HandleMainMenuSelection()
{
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN) {
        selectedOption = (selectedOption + 1) % 3; // Mover hacia abajo
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_DOWN) {
        selectedOption = (selectedOption + 2) % 3; // Mover hacia arriba
    }

    // Si se presiona Enter, ejecutar la opción seleccionada
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
        switch (selectedOption) {
        case 0: StartNewGame(); break;
        case 1: LoadGame(); break;
        case 2: Engine::GetInstance().CleanUp(); break;
        }
    }
}
void Scene::StartNewGame() {
    level = 1;  // Comienza un nuevo juego en el nivel 1
    player->SetPosition(Vector2D(3, 8.3)); // Reinicia la posición del jugador
    showMainMenu = false;  // Oculta el menú principal
    
}

void Scene::LoadGame() {
   
    //cargar juego guardado
    showMainMenu = false;  // Oculta el menú principal
}
