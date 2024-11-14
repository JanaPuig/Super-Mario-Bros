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

using namespace std;

Scene::Scene() : Module(), level(1), showHelpMenu(false), showMainMenu(true), ToggleHelpMenu(false)
{
    name = "scene";
}
// Destructor
Scene::~Scene() {}

bool Scene::Awake()
{
    LOG("Loading Scene");
    player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);

    if (level == 1) {
        CreateLevel1Items();
    }

    for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy");
        enemyNode; enemyNode = enemyNode.next_sibling("enemy")) {
        Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
        enemy->SetParameters(enemyNode);
    }
    return true;
}

void Scene::CreateLevel1Items() {
    if (level == 1) {
        const int startX = 1600, startY = 768, numItems = 7, spacing = 32, numRows = 3, rowSpacing = 64;

        for (int row = 0; row < numRows; ++row) {
            for (int i = 0; i < numItems; ++i) {
                Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
                item->position = Vector2D(startX + i * spacing, startY - row * rowSpacing);

                // Agrega un log para verificar la creación de ítems
                LOG("Creating item at position: (%f, %f)", item->position.getX(), item->position.getY());
            }
        }
    }
}


bool Scene::Start()
{
    // Load Map, Music, and Textures
    Engine::GetInstance().map->Load("Assets/Maps/", "Background.tmx");
    Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav", 0.5f);
    pipeFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Pipe.wav");
    CastleFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Music/StageClear_Theme.wav");
    mainMenu = Engine::GetInstance().textures.get()->Load("Assets/Textures/Main_Menu.png");
    helpTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Help_Menu.png");
    return true;
}

bool Scene::PreUpdate() {
    return true;
}

void Scene::ChangeLevel(int newLevel)
{
    if (level == newLevel) return;

    LOG("Changing level from %d to %d", level, newLevel);

    // Eliminar los ítems y entidades del nivel anterior
    Engine::GetInstance().entityManager->RemoveAllItems();
    Engine::GetInstance().map.get()->CleanUp();

    level = newLevel;

    // Cargar mapa y crear ítems según el nivel
    if (level == 1) 
    {
        player->SetPosition(Vector2D(3, 8));
        Engine::GetInstance().map->Load("Assets/Maps/", "Background.tmx");
        CreateLevel1Items(); // Volver a crear los ítems del nivel 1 cada vez que entras
    }
    else if (level ==2)
    {
        player->SetPosition(Vector2D(3, 13));
        Engine::GetInstance().map->Load("Assets/Maps/", "Map2.tmx");
    }
}

bool Scene::Update(float dt)
{
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
        ChangeLevel(2);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
        ChangeLevel(1);
    }

    Vector2D playerPos = player->GetPosition();

    // MainMenu code
    if (showMainMenu) Engine::GetInstance().render.get()->DrawTexture(mainMenu, -275, -250);
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) showMainMenu = false;

    // Handle Help Menu toggle
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN && !ToggleHelpMenu) {
        ToggleMenu();
        ToggleHelpMenu = true;
    }

    if (showHelpMenu) {
        int cameraX = Engine::GetInstance().render.get()->camera.x;
        int cameraY = Engine::GetInstance().render.get()->camera.y;
        Engine::GetInstance().render.get()->DrawTexture(helpTexture, -cameraX, -cameraY + 352);
        Engine::GetInstance().render.get()->DrawTexture(mainMenu, -cameraX - 275, -cameraY - 250);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_UP) {
        ToggleHelpMenu = false;
    }

    // Camera Code
    const float cameraMinX = 0.0f, cameraMaxX = 4862.0f, cameraFollowOffset = 192.0f;
    float desiredCamPosX = playerPos.getX() - Engine::GetInstance().render.get()->camera.w / 2 + cameraFollowOffset;
    desiredCamPosX = std::max(cameraMinX, std::min(cameraMaxX, desiredCamPosX));

    Engine::GetInstance().render.get()->camera.x = -desiredCamPosX;

    if (level == 1) {
        HandleTeleport(playerPos);
    }

    // Get mouse position and modify player position on mouse click
    Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
    Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap(mousePos.getX() - Engine::GetInstance().render.get()->camera.x,
        mousePos.getY() - Engine::GetInstance().render.get()->camera.y);
    Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld(mouseTile.getX(), mouseTile.getY());

    if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN) {
        player->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
    }

    LOG("After Teleport: X: %f, Y: %f", player->GetPosition().getX(), player->GetPosition().getY());

    return true;
}

void Scene::HandleTeleport(const Vector2D& playerPos) {
    const float tolerance = 5.0f;
    if (IsInTeleportArea(playerPos, 1440, 288, tolerance) || IsInTeleportArea(playerPos, 1472, 288, tolerance)) {
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
            Engine::GetInstance().audio.get()->PlayFx(pipeFxId);
            player->SetPosition(Vector2D(31, 11));
        }
    }
    else if (IsInTeleportArea(playerPos, 1856, 864, 5)) {
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
            player->SetPosition(Vector2D(36, 6));
            Engine::GetInstance().audio.get()->PlayFx(pipeFxId);
            LOG("Teleporting player to (1505, 864)");
        }
    }
    else if (IsInTeleportArea(playerPos, 6527, 416, tolerance)) {
        Engine::GetInstance().audio.get()->PlayFx(CastleFxId);
        ChangeLevel(2);
    }
}

bool Scene::IsInTeleportArea(const Vector2D& playerPos, float x, float y, float tolerance) {
    return playerPos.getX() >= x - tolerance && playerPos.getX() <= x + tolerance &&
        playerPos.getY() >= y - tolerance && playerPos.getY() <= y + tolerance;
}

bool Scene::PostUpdate()
{
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
        return false;
    return true;
}

bool Scene::CleanUp()
{
    Engine::GetInstance().textures.get()->UnLoad(helpTexture);
    Engine::GetInstance().audio.get()->StopMusic();
    Engine::GetInstance().textures.get()->UnLoad(mainMenu);
    LOG("Freeing scene");
    return true;
}

void Scene::ToggleMenu() {
    showHelpMenu = !showHelpMenu;
    LOG(showHelpMenu ? "SHOWING MENU" : "UNSHOWING MENU");
}
