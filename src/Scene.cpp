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
        player->SetParameters(configParameters.child("entities").child("player"));

        if (level == 1) {
            for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").first_child(); enemyNode; enemyNode = enemyNode.next_sibling())
            {
                //if (enemyNode.attribute("hitcount").as_int() == 0) {
                Enemy* enemy = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
                enemy->SetParameters(enemyNode);
                enemyStateList.push_back(std::make_pair(std::string(enemyNode.attribute("name").as_string()), 0));
                //enemyList.push_back(enemy);
                //hitcountList.push_back(enemy->hitCount);
                // Log para depuración
                LOG("Enemy created: %s", enemyNode.attribute("name").as_string());
                /* }*/

            }
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

                pugi::xml_node defaultItemNode = configParameters.child("entities").child("items").child("item");
                item->SetParameters(defaultItemNode);

                // Log the item's creation
                LOG("Creating item at position: (%f, %f)", item->position.getX(), item->position.getY());
            }
        }
    }
}

// Called before the first frame
bool Scene::Start()
{
    Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());

    // Load sound effects
    pipeFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Pipe.wav");
    CastleFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Music/StageClear_Theme.wav");
    MenuStart = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/StartNewGame.wav");
    SelectFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/SelectDown.wav");
    SelectFxId2 = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/SelectUp.wav");
    marioTime = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/mariotime.wav");
    hereWeGo = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/Start.wav");

    // Load textures for menus and transitions
    mainMenu = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("mainMenu").attribute("path").as_string());
    Title = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("title").attribute("path").as_string());
    helpMenuTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("helpMenu").attribute("path").as_string());
    level1Transition = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("level1Transition").attribute("path").as_string());
    level2Transition = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("level2Transition").attribute("path").as_string());
    newGameButtonSelected = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("newGameButtonSelected").attribute("path").as_string());
    loadGameButtonSelected = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("loadGameButtonSelected").attribute("path").as_string());
    leaveGameButtonSelected = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("leaveGameButtonSelected").attribute("path").as_string());
    newGameButton = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("newGameButton").attribute("path").as_string());
    loadGameButton = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("loadGameButton").attribute("path").as_string());
    leaveGameButton = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("leaveGameButton").attribute("path").as_string());
    loadingScreen = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("loadingScreen").attribute("path").as_string());
    gameOver = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("gameOver").attribute("path").as_string());
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
    Engine::GetInstance().map->CleanUp();
    Engine::GetInstance().entityManager->RemoveAllItems();
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
        // Reproducir GameIntro solo una vez
        if (!isGameIntroPlaying) {
            Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GameIntro.wav", 0.f);
            isGameIntroPlaying = true;
            }
        // Dibujar el fondo del menú principal
        Engine::GetInstance().render.get()->DrawTexture(mainMenu, -cameraX, -cameraY);

        // Dibujar los botones con la textura correcta según la opción seleccionada
        if (selectedOption == 0) {
            Engine::GetInstance().render.get()->DrawTexture(newGameButtonSelected, -cameraX + 65, -cameraY + 335);
        }
        else {
            Engine::GetInstance().render.get()->DrawTexture(newGameButton, -cameraX + 25, -cameraY + 350);
        }

        if (selectedOption == 1) {
            Engine::GetInstance().render.get()->DrawTexture(loadGameButtonSelected, -cameraX + 55, -cameraY + 480);
        }
        else {
            Engine::GetInstance().render.get()->DrawTexture(loadGameButton, -cameraX + 25, -cameraY + 490);
        }

        if (selectedOption == 2) {
            Engine::GetInstance().render.get()->DrawTexture(leaveGameButtonSelected, -cameraX + 50, -cameraY + 630);
        }
        else {
            Engine::GetInstance().render.get()->DrawTexture(leaveGameButton, -cameraX + 15, -cameraY + 640);
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
        return true; // Skip the rest of the game logic during transition
    }
    //Handle Loading Screen
    if (isLoading) {
        loadingTimer += dt;

        // Renderiza la pantalla de carga
        Engine::GetInstance().render.get()->DrawTexture(loadingScreen, -cameraX, -cameraY);

        // Si el tiempo de espera ha terminado, carga el estado del juego
        if (loadingTimer >= loadingScreenDuration) {
            isLoading = false; // Desactiva la pantalla de carga
            LoadState(); // Load Game State
        
        }

        return true; // Detenemos el resto de la lógica mientras está la pantalla de carga
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
        Engine::GetInstance().render.get()->DrawTexture(helpMenuTexture, -cameraX, -cameraY+375);
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
    const float tolerance = 15.0f;

    if (IsInTeleportArea(playerPos, 1440, 290, tolerance) ||
        IsInTeleportArea(playerPos, 1472, 288, tolerance))
    {
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
            Engine::GetInstance().audio.get()->PlayFx(pipeFxId);
            player->SetPosition(Vector2D(1550, 550));
        }
    }
    else if (IsInTeleportArea(playerPos, 1885, 864, tolerance)) {
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
            player->SetPosition(Vector2D(1800, 290));
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
    // Debug controls for level changes
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
        LOG("F2 presionado");
        ChangeLevel(2);
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
        ChangeLevel(1);
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
        ShowTransitionScreen();
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
        return false;
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
        LoadState();
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
        SaveState();
    }
    return true;
}

// Cleans up the scene
bool Scene::CleanUp()
{
    Engine::GetInstance().textures.get()->UnLoad(mainMenu);
    Engine::GetInstance().textures.get()->UnLoad(loadingScreen);
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
    Engine::GetInstance().textures.get()->UnLoad(gameOver);
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

    Engine::GetInstance().render.get()->DrawTexture((level == 1) ? level1Transition : level2Transition, -cameraX, -cameraY);
}

// Finishes the transition and loads the next level
void Scene::FinishTransition()
{
    isFlaged = false;
    showingTransition = false;
    Engine::GetInstance().audio.get()->PlayFx(hereWeGo);
    // Reactivate the player after transition
    if (player != nullptr) {
        player->SetActive(true);
    }
    // Load the respective level
    if (level == 1) {
        player->SetPosition(Vector2D(30, 430));
        Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());
        CreateLevel1Items();

    }
    else if (level == 2) {
        player->SetPosition(Vector2D(100, 740));
        Engine::GetInstance().map->Load(configParameters.child("map2").attribute("path").as_string(), configParameters.child("map2").attribute("name").as_string());
    }
    // Resume music after transition
    Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav", 0.f);
}
void Scene::HandleMainMenuSelection()
{
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN) {
        selectedOption = (selectedOption + 1) % 3; // Mover hacia abajo
        Engine::GetInstance().audio.get()->PlayFx(SelectFxId, 0.f);
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_DOWN) {
        selectedOption = (selectedOption + 2) % 3; // Mover hacia arriba
        Engine::GetInstance().audio.get()->PlayFx(SelectFxId2, 0.f);
    }

    // Si se presiona Enter, ejecutar la opción seleccionada
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
        switch (selectedOption) {
        case 0: StartNewGame(); Engine::GetInstance().audio.get()->PlayFx(MenuStart); Engine::GetInstance().audio.get()->PlayFx(marioTime); ShowTransitionScreen(); break;
        case 1: LoadGame(); Engine::GetInstance().audio.get()->PlayFx(MenuStart);break;
        case 2: Engine::GetInstance().CleanUp(); break;
        }
    }
}
void Scene::StartNewGame() {

    level = 1;
    player->SetPosition(Vector2D(30, 430));
    showMainMenu = false;

    // Guardar el estado inicial en el archivo XML
    pugi::xml_document SaveFile;
    pugi::xml_parse_result result = SaveFile.load_file("config.xml");
    if (result == NULL) {
        LOG("Error Saving config.xml: %s", result.description());
        return;
    }

    pugi::xml_node saveNode = SaveFile.child("config").child("scene").child("SaveFile");
    saveNode.attribute("level").set_value(1);
    saveNode.attribute("playerX").set_value(30);
    saveNode.attribute("playerY").set_value(430);

    // Reiniciar enemigos
    int enemy1X = saveNode.attribute("enemy1X").as_int();
    int enemy1Y = saveNode.attribute("enemy1Y").as_int();
    int enemy2X = saveNode.attribute("enemy2X").as_int();
    int enemy2Y = saveNode.attribute("enemy2Y").as_int();


    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("hitcount").set_value(0);
    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("hitcount").set_value(0);

    SaveFile.save_file("config.xml");
}

void Scene::LoadGame() {
    pugi::xml_document LoadFile;
    pugi::xml_parse_result result = LoadFile.load_file("config.xml");

    if (result == NULL) {
        LOG("Error Loading config.xml: %s", result.description());
        return;
    }
     
    pugi::xml_node saveNode = LoadFile.child("config").child("scene").child("SaveFile");

    level = saveNode.attribute("level").as_int();
    int playerX = saveNode.attribute("playerX").as_int();
    int playerY = saveNode.attribute("playerY").as_int();

    player->SetPosition(Vector2D(playerX, playerY));
     
    // Cargar enemigos
    saveNode.attribute("enemy1X").set_value(200);
    saveNode.attribute("enemy1Y").set_value(400);
    saveNode.attribute("enemy2X").set_value(400);
    saveNode.attribute("enemy2Y").set_value(400);

  /*  LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("hitcount").set_value(0);
    LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("hitcount").set_value(0);*/   
    // Si hay más datos a cargar, puedes hacerlo aquí.
    showMainMenu = false;
    LOG("Game loaded successfully."); 
}

// Return the player position
Vector2D Scene::GetPlayerPosition()
{
    return player->GetPosition();
}
void Scene::LoadState()
{
    pugi::xml_document LoadFile;
    pugi::xml_parse_result result = LoadFile.load_file("config.xml");

    if (result == NULL) {
        LOG("Error Loading config.xml: %s", result.description());
        return;
    }

    // read the player position from the XML
    Vector2D posPlayer;
    Vector2D posEnemy;
    posPlayer.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("playerX").as_int());
    posPlayer.setY(LoadFile.child("config").child("scene").child("SaveFile").attribute("playerY").as_int());
    int savedLevel = LoadFile.child("config").child("scene").child("SaveFile").attribute("level").as_int();
    Vector2D posKoopa, posGoomba;
    int KoopaHitcount = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("hitcount").as_int();
    int GoombaHitcount = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("hitcount").as_int();

    pugi::xml_node enemiesNode = LoadFile.child("config").child("scene").child("entities").child("enemies");

    for (std::pair<std::string, int> enemy : enemyStateList)
    {
        //Buscar el enemigo en la lista de enemigos usando su nombre
        Enemy* enemEntity = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName(enemy.first);

        //Si el enemigo existe, actualizar su hitcount
        if (enemEntity != nullptr) {
            for (pugi::xml_node node : enemiesNode.children())
            {
                if (node.attribute("name").as_string() == enemEntity->name)
                {
                    enemEntity->hitCount = GoombaHitcount;
                }

                else if (node.attribute("name").as_string() == enemEntity->name)
                {
                    enemEntity->hitCount = KoopaHitcount;
                }
            }
        }
    }

    if (KoopaHitcount == 0) {
        posKoopa.setX(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("x").as_int());
        posKoopa.setY(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("y").as_int());
    }

    if (GoombaHitcount == 0) {
        posGoomba.setX(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("x").as_int());
        posGoomba.setY(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("y").as_int());
    }

    //cargar mas cosas; enemies, items...   
    ChangeLevel(savedLevel);
    player->SetPosition(posPlayer);   // set the player position

}

void Scene::UpdateEnemyHitCount(std::string enemyName, int hitCount) {

    int i = 0;
    for (std::pair<std::string, int> enemy : enemyStateList)
    {
        if (enemy.first == enemyName)
        {
            enemyStateList[i].second = hitCount;
            break;
        }
        i++;
    }
}

void Scene::SaveState()
{
    pugi::xml_document SaveFile;
    pugi::xml_parse_result result = SaveFile.load_file("config.xml");
    if (result == NULL) {
        LOG("Error Saving config.xml: %s", result.description());
        return;
    }
    // read the player position and set the value in the XML
    Vector2D playerPos = player->GetPosition();
    SaveFile.child("config").child("scene").child("SaveFile").attribute("level").set_value(level);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("playerX").set_value(playerPos.getX());
    SaveFile.child("config").child("scene").child("SaveFile").attribute("playerY").set_value(playerPos.getY());
    //Actualizar enemigos
    pugi::xml_node enemiesNode = SaveFile.child("config").child("scene").child("entities").child("enemies");

    for (std::pair<std::string, int> enemy : enemyStateList)
    {
        //Buscar el enemigo en la lista de enemigos usando su nombre
        Enemy* enemyEntity = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName(enemy.first);

        for (pugi::xml_node node : enemiesNode.children())
        {
            if (node.attribute("name").as_string() == enemy.first)
            {
                if (enemy.first == "koopa")
                {
                    //Guardo el hitcount que tengo en la lista de enemigos
                    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("hitcount").set_value(enemy.second);
                }
             
                else if (enemy.first == "goomba")
                {
                    //Guardo el hitcount que tengo en la lista de enemigos
                    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("hitcount").set_value(enemy.second);
                }
                 

                // Si el enemigo no esta muerto actualizo su posicion 
                if (enemy.second == 0)
                {
                    Vector2D enemyPos = enemyEntity->GetPosition();
                    SaveFile.child("config").child("scene").child("SaveFile").attribute("enemy1X").set_value(enemyEntity->GetPosition().getX());
                    SaveFile.child("config").child("scene").child("SaveFile").attribute("enemy1Y").set_value(enemyEntity->GetPosition().getY());
                }

            }
        }
    }


    // save the XML modification to disk
    SaveFile.save_file("config.xml");

    //guardar mas cosas; enemies, items...

}