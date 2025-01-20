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
#include "GuiControl.h"
#include "GuiManager.h"
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

    player = static_cast<Player*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));
    player->SetParameters(configParameters.child("entities").child("player"));
    InitialItems();
    InitialEnemies(true);

    return true;
}

void Scene::InitialItems()
{
    pugi::xml_node defaultItemNode = configParameters.child("entities").child("items").child("coin");

    Item* item = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    item->SetParameters(defaultItemNode);
    itemStateList.push_back(std::make_pair(std::string(defaultItemNode.attribute("name").as_string()), 0));
    itemList.push_back(item);
    // Log the item's creation
    LOG("Creating item at position: (%f, %f)", item->position.getX(), item->position.getY());

    //Crear Flagpole
    Item* flagpole = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    pugi::xml_node flagpoleNode = configParameters.child("entities").child("items").child("flagpole");
    flagpole->SetParameters(flagpoleNode);
    itemStateList.push_back(std::make_pair(std::string(flagpoleNode.attribute("name").as_string()), 0));
    itemList.push_back(flagpole);

    // Crear flag
    Item* flag = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    pugi::xml_node flagNode = configParameters.child("entities").child("items").child("flag");
    flag->SetParameters(flagNode);
    itemStateList.push_back(std::make_pair(std::string(flagNode.attribute("name").as_string()), 0));
    itemList.push_back(flag);

    // Crear 1Up
    Item* OneUp = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    pugi::xml_node oneUpeNode = configParameters.child("entities").child("items").child("OneUp");
    OneUp->SetParameters(oneUpeNode);
    itemStateList.push_back(std::make_pair(std::string(oneUpeNode.attribute("name").as_string()), 0));
    itemList.push_back(OneUp);

    //Crear BigCoin
    const int hiddenX = -1000, hiddenY = -1000;
    Item* BigCoin = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    pugi::xml_node bigCoinNode = configParameters.child("entities").child("items").child("BigCoin");

    for (int i = 0; i < 3; i++) {  // Siempre crear 3 BigCoins
        Item* bigCoin = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
        bigCoin->position = Vector2D(hiddenX, hiddenY); // Inicialmente fuera de pantalla
        bigCoin->SetParameters(bigCoinNode);
        itemStateList.push_back(std::make_pair(std::string(bigCoinNode.attribute("name").as_string()), 0));
        itemList.push_back(bigCoin);

        LOG("BigCoin %d created at hidden position.", i + 1);
    }

    // Crear PowerUp
    Item* PowerUp = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    pugi::xml_node PowerUpNode = configParameters.child("entities").child("items").child("PowerUp");
    PowerUp->SetParameters(PowerUpNode);
    PowerUp->position = Vector2D(640, 285); // Asigna una posición inicial válida
    itemStateList.push_back(std::make_pair(std::string(PowerUpNode.attribute("name").as_string()), 0));
    itemList.push_back(PowerUp);
    LOG("Creating Power-Up at position: (%f, %f)", PowerUp->position.getX(), PowerUp->position.getY());

    // Crear flagpole del final del nivel
    Item* flagpole2 = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    pugi::xml_node flagpole2Node = configParameters.child("entities").child("items").child("finish_flagpole");
    flagpole2->SetParameters(flagpole2Node);
    flagpole2->position = Vector2D(640, 285); // Asigna una posición inicial válida
    itemStateList.push_back(std::make_pair(std::string(flagpole2Node.attribute("name").as_string()), 0));
    itemList.push_back(flagpole2);
    LOG("Creating Final Flagpole at position: (%f, %f)", flagpole2->position.getX(), flagpole2->position.getY());

    // Crear flag del final del nivel
    Item* flag2 = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    pugi::xml_node flag2Node = configParameters.child("entities").child("items").child("finish_flag");
    flag2->SetParameters(flag2Node);
    flag2->position = Vector2D(640, 285); // Asigna una posición inicial válida
    itemStateList.push_back(std::make_pair(std::string(flag2Node.attribute("name").as_string()), 0));
    itemList.push_back(flag2);
    LOG("Creating Final Flag at position: (%f, %f)", flag2->position.getX(), flag2->position.getY());

}
void Scene::InitialEnemies(bool useXMLPosition = true) {
    // Iteramos a través de los enemigos del archivo XML
    for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").first_child();
        enemyNode;
        enemyNode = enemyNode.next_sibling()) {

        int levels = enemyNode.attribute("levels").as_int();
        std::string enemyName = enemyNode.attribute("name").as_string();

        // Solo crear enemigos para los niveles 1, 2 y 3
        if (levels == 1 || levels == 2 || levels == 3) {
            LOG("Level create enemies: %d", levels);

            // Crear un nuevo enemigo
            Enemy* enemy = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));

            if (!enemy) {
                LOG("Failed to create enemy: %s.", enemyName.c_str());
                continue;
            }

            // Establece los parámetros del enemigo
            enemy->SetParameters(enemyNode, useXMLPosition);

            // Agregar el enemigo a las listas de estados y enemigos
            enemyStateList.push_back(std::make_pair(enemyName, 0));
            enemyList.push_back(enemy);

            LOG("Enemy %s created for level %d", enemyName.c_str(), level);
        }
    }
}
// Creates items for Level 1
void Scene::CreateLevelItems(int level)
{

    Engine::GetInstance().entityManager->ResetItems(); // Reiniciar ítems al cambiar de nivel

    if (level == 1) {
        // Posiciones para BigCoins en el nivel 1
        std::vector<Vector2D> bigCoinPositions = {
            Vector2D(633, 120),
            Vector2D(3417, 120),
            Vector2D(4590, 30)
        };

        int bigCoinIndex = 0;
        for (auto& it : itemList) {
            if (it->name == "BigCoin" && bigCoinIndex < bigCoinPositions.size()) {
                it->position = bigCoinPositions[bigCoinIndex++];
            }
            else if (it->name == "flagpole") {
                it->position = Vector2D(3232, 384);
            }
            else if (it->name == "flag") {
                it->position = Vector2D(3248, 384);
            }
            else if (it->name == "finish_flagpole") {
                it->position = Vector2D(6343, 90);
            }
            else if (it->name == "finish_flag") {
                it->position = Vector2D(6299, 110);
            }
            else if (it->name == "OneUp") {
                it->position = Vector2D(1690, 543);
            }
            else if (it->name == "PowerUp") {
                it->position = Vector2D(640, 285);
            }
            else if (it->name == "coin") {
                it->position = Vector2D(1690, 720);
            }
            else {
                it->position = Vector2D(-1000, -1000); // Ocultar ítems que no se usan
            }
        }
    }
    else if (level == 2) {
        // Posiciones para BigCoins en el nivel 2
        std::vector<Vector2D> bigCoinPositions = {
            Vector2D(5050, 230),
            Vector2D(2712, 715),
            Vector2D(5850, 720)
        };

        int bigCoinIndex = 0;
        for (auto& it : itemList) {
            if (it->name == "BigCoin" && bigCoinIndex < bigCoinPositions.size()) {
                it->position = bigCoinPositions[bigCoinIndex++];
            }
            else if (it->name == "flagpole") {
                it->position = Vector2D(3302, 800);
            }
            else if (it->name == "flag") {
                it->position = Vector2D(3318, 800);
            }
            else if (it->name == "finish_flagpole") {
                it->position = Vector2D(6533, 410);
            }
            else if (it->name == "finish_flag") {
                it->position = Vector2D(6489, 430);
            }
            else if (it->name == "PowerUp") {
                it->position = Vector2D(1535, 575);
            }
            else {
                it->position = Vector2D(-1000, -1000);
            }
        }
    }
    else if (level == 3) {
        // Posiciones para BigCoins en el nivel 3
        std::vector<Vector2D> bigCoinPositions = {
            Vector2D(4660, 560),
            Vector2D(3420, 735),
            Vector2D(1810, 735)
        };

        int bigCoinIndex = 0;
        for (auto& it : itemList) {
            if (it->name == "BigCoin" && bigCoinIndex < bigCoinPositions.size()) {
                it->position = bigCoinPositions[bigCoinIndex++];
            }
            else if (it->name == "flagpole") {
                it->position = Vector2D(3480, 545);
            }
            else if (it->name == "flag") {
                it->position = Vector2D(3496, 545);
            }
            else if (it->name == "OneUp") {
                it->position = Vector2D(2575, 767);
            }
            else if (it->name == "PowerUp") {
                it->position = Vector2D(3902, 735);
            }
            else {
                it->position = Vector2D(-1000, -1000); // Ocultar ítems que no se usan
            }
        }
    }
}
void Scene::CreateEnemies(int level) {
    LOG("Creating enemies for level %d", level);

    // Limpiar enemigos existentes
    enemyList.clear();
    enemyStateList.clear();
    Engine::GetInstance().entityManager->RemoveAllEnemies();

    if (level == 1) {
        // Crear Koopas y Goombas para el nivel 1
        Enemy* koopa1 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        koopa1->SetParameters(configParameters.child("entities").child("enemies").child("enemy_koopa"), false);
        koopa1->SetPosition(Vector2D(1500, 100));
        enemyList.push_back(koopa1);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_koopa")).attribute("name").as_string()), 0));
        save_hitCount_koopa = 0;

        Enemy* koopa2 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        koopa2->SetPosition(Vector2D(4500, 100));
        koopa2->SetParameters(configParameters.child("entities").child("enemies").child("enemy_koopa2"),false);
        enemyList.push_back(koopa2);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_koopa2")).attribute("name").as_string()), 0));
        save_hitCount_koopa2 = 0;

        Enemy* goomba1 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        goomba1->SetPosition(Vector2D(2000, 415));
        goomba1->SetParameters(configParameters.child("entities").child("enemies").child("enemy"),false);
        enemyList.push_back(goomba1);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy")).attribute("name").as_string()), 0));
        save_hitCount_goomba = 0;

        Enemy* goomba2 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        goomba2->SetPosition(Vector2D(5500, 415));
        goomba2->SetParameters(configParameters.child("entities").child("enemies").child("enemy2"),false);
        enemyList.push_back(goomba2);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy2")).attribute("name").as_string()), 0));
        save_hitCount_goomba2 = 0;

    }
    else if (level == 2) {
        // Crear Koopas para el nivel 2
        Enemy* koopa1 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        koopa1->SetPosition(Vector2D(2000, 150)); // Nueva posición
        koopa1->SetParameters(configParameters.child("entities").child("enemies").child("enemy_koopa"), false);
        enemyList.push_back(koopa1);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_koopa")).attribute("name").as_string()), 0));
        save_hitCount_koopa = 0;

        Enemy* koopa2 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        koopa2->SetPosition(Vector2D(3500, 150)); // Nueva posición
        koopa2->SetParameters(configParameters.child("entities").child("enemies").child("enemy_koopa2"), false);
        enemyList.push_back(koopa2);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_koopa2")).attribute("name").as_string()), 0));
        save_hitCount_koopa2 = 0;

    }
    else if (level == 3) {
        // Crear Koopas para el nivel 3

        Enemy* goomba1 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        goomba1->SetPosition(Vector2D(2000, 470));
        goomba1->SetParameters(configParameters.child("entities").child("enemies").child("enemy"), false);
        enemyList.push_back(goomba1);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy")).attribute("name").as_string()), 0));
        save_hitCount_goomba = 0;

        Enemy* goomba2 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        goomba2->SetPosition(Vector2D(1100, 470));
        goomba2->SetParameters(configParameters.child("entities").child("enemies").child("enemy2"), false);
        enemyList.push_back(goomba2);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy2")).attribute("name").as_string()), 0));
        save_hitCount_goomba2 = 0;

        Enemy* koopa1 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        koopa1->SetPosition(Vector2D(2600, 150));
        koopa1->SetParameters(configParameters.child("entities").child("enemies").child("enemy_koopa"), false);
        enemyList.push_back(koopa1);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_koopa")).attribute("name").as_string()), 0));
        save_hitCount_koopa = 0;

        Enemy* koopa2 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        koopa2->SetPosition(Vector2D(3800, 150));
        koopa2->SetParameters(configParameters.child("entities").child("enemies").child("enemy_koopa2"), false);
        enemyList.push_back(koopa2);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_koopa2")).attribute("name").as_string()), 0));
        save_hitCount_koopa2 = 0;

        // Crear Bowser para nivel 3
        Enemy* bowser = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
        bowser->SetPosition(Vector2D(6200, 330)); // Posición de Bowser
        bowser->SetParameters(configParameters.child("entities").child("enemies").child("enemy_bowser"), false);
        enemyList.push_back(bowser);
        enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_bowser")).attribute("name").as_string()), 0));
        save_hitCount_bowser = 0;

        LOG("Bowser created for level 3 at position (6200, 330)");
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
    EndGame = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/EndGame.wav");
    BowserLaugh = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/BowserLaugh.wav");

    // Load textures for menus and transitions
    mainMenu = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("mainMenu").attribute("path").as_string());
    Title = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("title").attribute("path").as_string());
    helpMenuTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("helpMenu").attribute("path").as_string());
    gameOver = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("gameOver").attribute("path").as_string());
    GroupLogo = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("GroupLogo").attribute("path").as_string());
    black = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("black").attribute("path").as_string());
    settings = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("settings").attribute("path").as_string());
    tick = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("tick").attribute("path").as_string());
    menu_pause = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("menu_pause").attribute("path").as_string());


    showGroupLogo = true;
    logoTimer = 0.0f;
    return true;

}
// Called before each frame update
bool Scene::PreUpdate()
{
    return true;
}
// Main update logic for the Scene
bool Scene::Update(float dt)
{
    if (!showPauseMenu) {
        DrawLives();
        DrawObject();
        DrawPuntation();
        DrawWorld();
    }
    int cameraX = Engine::GetInstance().render.get()->camera.x;
    int cameraY = Engine::GetInstance().render.get()->camera.y;

    if (showGroupLogo)
    {
        logoTimer += dt;
        Engine::GetInstance().render.get()->DrawTexture(GroupLogo, -cameraX, -cameraY);
        if (logoTimer >= logoDuration)
        {
            showGroupLogo = false;
            showMainMenu = true;  // Mostrar el menú principal
        }
        return true;  
    }

    if (showMainMenu) {
        // Reproducir GameIntro solo una vez
        if (!isGameIntroPlaying) {
            Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GameIntro.wav", 0.f);
            isGameIntroPlaying = true;
        }
        menu();
        Engine::GetInstance().render.get()->DrawTexture(mainMenu, -cameraX, -cameraY);
        if (showCredits) {
            isGameIntroPlaying = false;
            Credits(); 
            if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
                showCredits = false; // Vuelve al menu si se presiona backspace
          
            }
            return true;
        }
        if (showSettings) {
            Settings();
            isGameIntroPlaying = false;
            mousePos = Engine::GetInstance().input->GetMousePosition();
            LOG("Mouse X: %f, Mouse Y: %f", mousePos.getX(), mousePos.getY());
            if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
                showSettings = false;
            }
            return true;
        }

        return true; // Evita que se ejecute el código del resto del juego mientras el menú esté activo
    }
    else {
        Engine::GetInstance().guiManager->CleanUp();

    }
    if (showWinScreen) {
        int cameraX = Engine::GetInstance().render.get()->camera.x;
        int cameraY = Engine::GetInstance().render.get()->camera.y;

        // Dibuja la pantalla negra
        Engine::GetInstance().render.get()->DrawTexture(black, -cameraX, -cameraY);

        // Dibuja el texto "YOU WIN!"
        Engine::GetInstance().render.get()->DrawText("YOU WIN!", 725, 400, 400, 250);
        Engine::GetInstance().render.get()->DrawText("PRESS ENTER TO RETURN TO MENU!", 700, 800, 450, 75);

        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
            showWinScreen = false;
            showMainMenu = true;
        }
        return true;
    }
    // Handle level transition screen
    if (showingTransition) {
        transitionTimer += dt;
        Engine::GetInstance().render.get()->DrawTexture(black, -cameraX, -cameraY);
        if (level == 1) { Engine::GetInstance().render->DrawText("WORLD 1-1", 730, 450, 475, 150); }
        else if (level == 2) { Engine::GetInstance().render->DrawText("WORLD 1-2", 730, 450, 475, 150); }
        else if (level == 3) { Engine::GetInstance().render->DrawText("CASTLE 1-3", 720, 450, 475, 150); }
        if (transitionTimer >= transitionDuration) {
            FinishTransition();
        }
        return true; // Skip the rest of the game logic during transition
    }
    //Handle Loading Screen
    if (isLoading) {
        loadingTimer += dt;
        Engine::GetInstance().render.get()->DrawTexture(black, -cameraX, -cameraY);   // Renderiza la pantalla de carga
        Engine::GetInstance().render->DrawText("LOADING...", 730, 450, 475, 150);
        Engine::GetInstance().audio.get()->StopMusic();
        if (loadingTimer >= loadingScreenDuration) {     // Si el tiempo de espera ha terminado, carga el estado del juego
            isLoading = false; // Desactiva la pantalla de carga
            Engine::GetInstance().audio.get()->PlayFx(hereWeGo);
            LoadGame(); // Load Game State
        }
        return true; // Detenemos el resto de la lógica mientras está la pantalla de carga
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
        showPauseMenu = !showPauseMenu; // Alternar el estado del menú

        if (showPauseMenu) {    //Que no se vean los items, los enemigos ni el player
            player->StopMovement(); //Parar movimineto del player
            for (Enemy* enemy : enemyList) { //Parar movimineto del enemigo
                if (enemy != nullptr) {
                    enemy->visible = false;
                    enemy->StopMovement();
                }
                else {
                    continue;
                }
            }
            for (Item* item : itemList) { //Dejar de ver items
                item->apear = false;
            }

            Engine::GetInstance().render.get()->camera.x = Engine::GetInstance().render.get()->camera.x;
            Engine::GetInstance().render.get()->camera.y = Engine::GetInstance().render.get()->camera.y;
        }
        else {
            LOG("Calling player->ResumeMovement()");

            player->ResumeMovement(); //Renundar movimineto del player

            for (Enemy* enemy : enemyList) { //Renundar movimineto del enemigo
                if (enemy != nullptr) {
                    enemy->visible = true;
                    enemy->ResumeMovement();
                }
                else { 
                    continue;
                }
              
            }
            for (Item* item : itemList) { //Ver items
                item->apear = true;
            }
            showPauseMenu = false; // Cerrar el menú al presionar Backspace      
        }
    }
    // Si el menú de pausa está activo, dibujarlo y detener el resto de la lógica
    if (showPauseMenu) {
        funcion_menu_pause();

        if (manage_showSettings) {
            Settings();
            mousePos = Engine::GetInstance().input->GetMousePosition();
            LOG("Mouse X: %f, Mouse Y: %f", mousePos.getX(), mousePos.getY());

            //Engine::GetInstance().guiManager->Draw();
            if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
                manage_showSettings = false;
            }

        }
        return true;
    }

    Vector2D playerPos = player->GetPosition();

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN && !ToggleHelpMenu) {
        ToggleMenu();
        ToggleHelpMenu = true;
    }
    if (showHelpMenu) {
        int cameraX = Engine::GetInstance().render.get()->camera.x;
        int cameraY = Engine::GetInstance().render.get()->camera.y;
        Engine::GetInstance().render.get()->DrawTexture(helpMenuTexture, -cameraX, -cameraY + 375);
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
    if (level == 1 || level == 2 || level == 3) {
        HandleTeleport(playerPos);
    }

    // Temporizador del nivel
    elapsedTime += dt;  // Aumenta el tiempo acumulado
    float currentTime = showRemainingTime ? (levelTime - elapsedTime) : elapsedTime;

    // Lógica si se acaba el tiempo
    if ((currentTime / 1000) <= 0.0f)
    {
        std::cout << "Time's up! Game over!" << std::endl;
        timeUp = true;
    }

    // Renderizar el tiempo en pantalla
    char timerText[64];
    if (showRemainingTime)
        sprintf_s(timerText, "%0.f", currentTime / 1000);
    if (!timeUp) { Engine::GetInstance().render.get()->DrawText(timerText, 1620, 20, 30, 30); }
    if (timeUp) { Engine::GetInstance().render.get()->DrawText("0", 1620, 20, 20, 30); }
    Engine::GetInstance().render.get()->DrawText("Time Remaining:", 1380, 20, 225, 30);

    return true;
}
// Handles teleportation logic
void Scene::HandleTeleport(const Vector2D& playerPos)
{
    const float tolerance = 20.0f;
    if (level == 1) {
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
    if (level == 2) {
        if (IsInTeleportArea(playerPos, 6690, 764, tolerance))
        {
            Engine::GetInstance().audio.get()->PlayFx(CastleFxId);
            ChangeLevel(3);

        }
    }
    if (level == 3) {
        if (bossFightActive == false && (IsInTeleportArea(playerPos, 5100, 400, tolerance) || IsInTeleportArea(playerPos, 5100, 360, tolerance) || IsInTeleportArea(playerPos, 5100, 440, tolerance)))
        {
            bossFightActive = true;
            Engine::GetInstance().audio.get()->PlayFx(BowserLaugh);
            Engine::GetInstance().audio.get()->StopMusic();
            Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/BossFight.wav");

        }
        else if (IsInTeleportArea(playerPos, 6630, 415, tolerance) || IsInTeleportArea(playerPos, 6630, 375, tolerance))
        {
            showWinScreen = true;
            Engine::GetInstance().audio->StopMusic();
            if (!hasPlayedWinMusic) {
                Engine::GetInstance().audio.get()->PlayFx(EndGame);
                hasPlayedWinMusic = true; // Marcar que la música ha sido reproducida
            }
        }
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
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
        ChangeLevel(3);
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
        ShowTransitionScreen();
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
        LoadGame();
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
    Engine::GetInstance().textures.get()->UnLoad(helpMenuTexture);
    Engine::GetInstance().textures.get()->UnLoad(Title);
    Engine::GetInstance().textures.get()->UnLoad(gameOver);
    Engine::GetInstance().textures.get()->UnLoad(GroupLogo);
    Engine::GetInstance().textures.get()->UnLoad(black);
    Engine::GetInstance().textures.get()->UnLoad(settings);
    Engine::GetInstance().textures.get()->UnLoad(tick);
    Engine::GetInstance().textures.get()->UnLoad(menu_pause);
    Engine::GetInstance().textures.get()->UnLoad(menu_pause);
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
// Changes the current level
void Scene::ChangeLevel(int newLevel)
{
    if (level == newLevel) return;
    LOG("Changing level from %d to %d", level, newLevel);

    Engine::GetInstance().entityManager->RemoveAllEnemies();
    Engine::GetInstance().map->CleanUp();

    level = newLevel;
    ShowTransitionScreen();
    CreateLevelItems(level);
}
// Shows the transition screen
void Scene::ShowTransitionScreen()
{
    showingTransition = true;
    transitionTimer = 0.0f;
    if (player != nullptr) {
        player->SetActive(false);
    }
    Engine::GetInstance().audio.get()->StopMusic();
}

// Finishes the transition and loads the next level
void Scene::FinishTransition()
{
    elapsedTime = 0.0f;
    isFlaged = false;
    Engine::GetInstance().entityManager->starPowerDuration = 0;
    Engine::GetInstance().entityManager->isStarPower = false;
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
        Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav", 0.f);
        Mix_VolumeMusic(sdlVolume);
        CreateEnemies(1);
    }
    else if (level == 2) {
        player->SetPosition(Vector2D(100, 740));
        Engine::GetInstance().map->Load(configParameters.child("map2").attribute("path").as_string(), configParameters.child("map2").attribute("name").as_string());
        Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/World2Theme.wav", 0.f);
        Mix_VolumeMusic(sdlVolume);
        CreateEnemies(2);
    }
    else if (level == 3) {
        player->SetPosition(Vector2D(100, 580));
        Engine::GetInstance().map->Load(configParameters.child("map3").attribute("path").as_string(), configParameters.child("map3").attribute("name").as_string());
        Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/CastleTheme.wav", 0.f);
        Mix_VolumeMusic(sdlVolume);
        CreateEnemies(3);
    }
}

void Scene::StartNewGame() {
    hasPlayedWinMusic = false;
    isGameIntroPlaying = false;
    // Cargar el archivo de configuración original
    pugi::xml_document originalConfig;
    pugi::xml_parse_result result = originalConfig.load_file("config.xml");
    if (result == NULL) {
        LOG("Error loading config.xml: %s", result.description());
        return;
    }

    // Restablecer el estado inicial en el archivo XML
    pugi::xml_node saveNode = originalConfig.child("config").child("scene").child("SaveFile");
    saveNode.attribute("level").set_value(1);
    saveNode.attribute("playerX").set_value(30);
    saveNode.attribute("playerY").set_value(430);
    saveNode.attribute("checkpoint").set_value(false);
    saveNode.attribute("lives").set_value(3);
    saveNode.attribute("object").set_value(0);
    saveNode.attribute("Puntuation").set_value(0);
    saveNode.attribute("isSave").set_value(false);

    // Restablecer enemigos
    saveNode = originalConfig.child("config").child("scene").child("entities").child("enemies");
    saveNode.child("enemy_koopa").attribute("hitcount").set_value(0);
    saveNode.child("enemy_koopa2").attribute("hitcount").set_value(0);
    saveNode.child("enemy").attribute("hitcount").set_value(0);
    saveNode.child("enemy2").attribute("hitcount").set_value(0);

    // Restablecer ítems
    saveNode = originalConfig.child("config").child("scene").child("entities").child("items");
    saveNode.child("coin").attribute("isPicked").set_value(0);

    // Guardar el archivo XML con los valores originales
    originalConfig.save_file("config.xml");

    // Reiniciar el estado del juego
    level = 1;
    player->SetPosition(Vector2D(30, 430));
    showMainMenu = false;

    // Reiniciar el juego
    Engine::GetInstance().entityManager->ResetItems();
    Engine::GetInstance().entityManager->ResetEnemies();
    CreateLevelItems(level);
    CreateEnemies(level);

    // Reproducir efectos de sonido
    Engine::GetInstance().audio.get()->PlayFx(MenuStart);
    Engine::GetInstance().audio.get()->PlayFx(marioTime);
    ShowTransitionScreen();
}

void Scene::LoadGame() {
    Engine::GetInstance().audio->StopMusic();
    pugi::xml_document LoadFile;
    pugi::xml_parse_result result = LoadFile.load_file("config.xml");

    if (result == NULL) {
        LOG("Error Loading config.xml: %s", result.description());
        return;
    }

    // Cargar el estado del jugador
    pugi::xml_node saveNode = LoadFile.child("config").child("scene").child("SaveFile");
    int Savedlevel = saveNode.attribute("level").as_int(); // Cargar el nivel guardado
    int playerX = saveNode.attribute("playerX").as_int();
    int playerY = saveNode.attribute("playerY").as_int();
    player->SetPosition(Vector2D(playerX, playerY));
    Engine::GetInstance().entityManager->lives = saveNode.attribute("lives").as_int();
    Engine::GetInstance().entityManager->objects = saveNode.attribute("object").as_int();
    Engine::GetInstance().entityManager->puntuation = saveNode.attribute("Puntuation").as_int();
    Engine::GetInstance().entityManager->isStarPower = saveNode.attribute("IsPowerUp").as_bool();
    Engine::GetInstance().entityManager->starPowerDuration = saveNode.attribute("PowerUpTime").as_float();
    elapsedTime = saveNode.attribute("time").as_float();
    isFlaged = saveNode.attribute("checkpoint").as_bool();

    for (int i = 1; i <= 5; ++i) {
       
        if (i == 1) {
            Enemy* enemyKoopa = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName("koopa");
            pugi::xml_node enemyNode = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa");
            std::string enemyName = enemyNode.attribute("name").as_string();
            float posX = enemyNode.attribute("x").as_int();
            float posY = enemyNode.attribute("y").as_int();

            if (enemyKoopa != nullptr) {
               
                enemyKoopa->Load_SetPosition(Vector2D(posX, posY));

            }
            else if(enemyKoopa == nullptr && save_hitCount_koopa == 0) {

                Enemy* koopa1 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
                koopa1->SetParameters(configParameters.child("entities").child("enemies").child("enemy_koopa"), false);
                koopa1->SetPosition(Vector2D(posX, posY));
                enemyList.push_back(koopa1);
                enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_koopa")).attribute("name").as_string()), 0));
                save_hitCount_koopa = 0;
            }
        }

        //Koopa2
        else if (i == 2) {
            Enemy* enemyKoopa2 = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName("koopa2");
            pugi::xml_node enemyNode = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2");
            std::string enemyName = enemyNode.attribute("name").as_string();
            int posX = enemyNode.attribute("x").as_int();
            int posY = enemyNode.attribute("y").as_int();
           
            if (enemyKoopa2 != nullptr) {
                enemyKoopa2->Load_SetPosition(Vector2D(posX, posY));

            }
            else if (enemyKoopa2 == nullptr && save_hitCount_koopa2 == 0) {
                Enemy* koopa2 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
                koopa2->SetParameters(configParameters.child("entities").child("enemies").child("enemy_koopa2"), false);
                koopa2->SetPosition(Vector2D(posX, posY));
                enemyList.push_back(koopa2);
                enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_koopa2")).attribute("name").as_string()), 0));
                save_hitCount_koopa2 = 0;
            }
        }
        //Goomba
        else if (i == 3) {
            Enemy* enemyGoomba = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName("goomba");
            pugi::xml_node enemyNode = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy");
            std::string enemyName = enemyNode.attribute("name").as_string();
            int posX = enemyNode.attribute("x").as_int();
            int posY = enemyNode.attribute("y").as_int();
            if (enemyGoomba != nullptr) {
                enemyGoomba->Load_SetPosition(Vector2D(posX, posY));

            }
            else if (enemyGoomba == nullptr && save_hitCount_goomba == 0) {
                Enemy* goomba1 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
                goomba1->SetParameters(configParameters.child("entities").child("enemies").child("enemy"), false);
                goomba1->SetPosition(Vector2D(posX, posY));
                enemyList.push_back(goomba1);
                enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy")).attribute("name").as_string()), 0));
                save_hitCount_goomba = 0;
            }
        }
        //Goomba2
        else if (i == 4) {
            Enemy* enemyGoomba2 = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName("goomba2");
            pugi::xml_node enemyNode = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy2");
            std::string enemyName = enemyNode.attribute("name").as_string();
            int posX = enemyNode.attribute("x").as_int();
            int posY = enemyNode.attribute("y").as_int();
            if (enemyGoomba2 != nullptr) {
                enemyGoomba2->Load_SetPosition(Vector2D(posX, posY));

            }
            else if (enemyGoomba2 == nullptr && save_hitCount_goomba2 == 0) {
                Enemy* goomba2 = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
                goomba2->SetParameters(configParameters.child("entities").child("enemies").child("enemy2"), false);
                goomba2->SetPosition(Vector2D(posX, posY));
                enemyList.push_back(goomba2);
                enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy2")).attribute("name").as_string()), 0));
                save_hitCount_goomba2 = 0;
            }

        }

        else if (i == 5 && level==3) {
            Enemy* enemyBowser = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName("bowser");
            pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy_bowser");
            std::string enemyName = enemyNode.attribute("name").as_string();
            int posX = enemyNode.attribute("x").as_int();
            int posY = enemyNode.attribute("y").as_int();
            if (enemyBowser != nullptr) {
                enemyBowser->Load_SetPosition(Vector2D(posX, posY));

            }
            else if (enemyBowser == nullptr && save_hitCount_goomba2 !=3) {
                Enemy* bowser = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
                bowser->SetParameters(configParameters.child("entities").child("enemies").child("enemy_bowser"), false);
                bowser->SetPosition(Vector2D(posX, posY)); // Posición de Bowser
                enemyList.push_back(bowser);
                enemyStateList.push_back(std::make_pair(std::string((configParameters.child("entities").child("enemies").child("enemy_bowser")).attribute("name").as_string()), 0));
                save_hitCount_bowser = 0;
            }

        }
        

    }
    
    pugi::xml_node itemsNode = LoadFile.child("config").child("scene").child("entities").child("items");
    for (pugi::xml_node itemNode = itemsNode.first_child(); itemNode; itemNode = itemNode.next_sibling()) {
        std::string itemName = itemNode.attribute("name").as_string();
        Item* itemEntity = (Item*)Engine::GetInstance().entityManager->GetEntityByName(itemName);
        if (itemEntity != nullptr) {
            itemEntity->SetIsPicked(itemNode.attribute("isPicked").as_int());
            itemEntity->SetPosition(Vector2D(itemNode.attribute("x").as_float(), itemNode.attribute("y").as_float()));
        }
    }

    // Reproducir la música del nivel correspondiente
    if (Engine::GetInstance().entityManager->isStarPower == true) {
        Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/StarPower.wav");
    }
    else if (Savedlevel == 1 && Engine::GetInstance().entityManager->isStarPower==false) {
        Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/GroundTheme.wav");
    }
    else if (Savedlevel == 2&&Engine::GetInstance().entityManager->isStarPower == false) {
        Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/World2Theme.wav");
    }
    else if (Savedlevel == 3&& Engine::GetInstance().entityManager->isStarPower == false) {
        Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/CastleTheme.wav");
    }
    // Cambiar al nivel cargado
    ChangeLevel(Savedlevel);
    if (showMainMenu == true) {
        showMainMenu = false;
    }
    LOG("Game loaded successfully.");

}

// Return the player position
Vector2D Scene::GetPlayerPosition()
{
    return player->GetPosition();
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
void Scene::UpdateItem(std::string itemName, int isPicked) {
    int i = 0;
    for (std::pair<std::string, int> item : itemStateList)
    {
        if (item.first == itemName)
        {
            itemStateList[i].second = isPicked;

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

    // Guardar el estado del jugador
    Vector2D playerPos = player->GetPosition();
    SaveFile.child("config").child("scene").child("SaveFile").attribute("lives").set_value(Engine::GetInstance().entityManager->lives);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("object").set_value(Engine::GetInstance().entityManager->objects);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("Puntuation").set_value(Engine::GetInstance().entityManager->puntuation);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("level").set_value(level);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("playerX").set_value(playerPos.getX());
    SaveFile.child("config").child("scene").child("SaveFile").attribute("playerY").set_value(playerPos.getY());
    SaveFile.child("config").child("scene").child("SaveFile").attribute("checkpoint").set_value(isFlaged);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("time").set_value(elapsedTime);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("IsPowerUp").set_value(Engine::GetInstance().entityManager->isStarPower);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("PowerUpTime").set_value(Engine::GetInstance().entityManager->starPowerDuration);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("isSave").set_value(1);
    isSave= SaveFile.child("config").child("scene").child("SaveFile").attribute("isSave").set_value(1);

    // Enemigos
    pugi::xml_node enemiesNode = SaveFile.child("config").child("scene").child("entities").child("enemies");
    for (std::pair<std::string, int> enemies : enemyStateList)
    {
        Enemy* enemyEntity = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName(enemies.first);
        if (enemyEntity != nullptr) {
            SaveFile.child("config").child("scene").child("entities").child("enemies").child(enemies.first.c_str()).attribute("hitcount").set_value(enemyEntity->hitCount);
            SaveFile.child("config").child("scene").child("entities").child("enemies").child(enemies.first.c_str()).attribute("isAlive").set_value(enemyEntity->isAlive ? 1 : 0);
              
            if (enemies.first == "koopa") {
                LOG("posx:%f, posY:%f", enemyEntity->GetPosition().getX(), enemyEntity->GetPosition().getY());

                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("x").set_value(enemyEntity->GetPosition().getX()-40);
                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("y").set_value(enemyEntity->GetPosition().getY()- 42);
            }
            if (enemies.first == "koopa2") {
                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2").attribute("x").set_value(enemyEntity->GetPosition().getX()-40);
                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2").attribute("y").set_value(enemyEntity->GetPosition().getY()- 42);
            }
            if (enemies.first == "goomba") {
                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("x").set_value(enemyEntity->GetPosition().getX() - 16);
                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("y").set_value(enemyEntity->GetPosition().getY() - 16);

            }
            if (enemies.first == "goomba2") {
                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy2").attribute("x").set_value(enemyEntity->GetPosition().getX() - 16);
                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy2").attribute("y").set_value(enemyEntity->GetPosition().getY() - 16);

            }
            if (enemies.first == "bowser" && level == 3) {
                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_bowser").attribute("x").set_value(enemyEntity->GetPosition().getX()-72);
                SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_bowser").attribute("y").set_value(enemyEntity->GetPosition().getY()-64);

            }
        }
        else if (enemyEntity == nullptr) {
            if (enemies.first == "koopa") save_hitCount_koopa = 1;
            if (enemies.first == "koopa2") save_hitCount_koopa2 = 1;
            if (enemies.first == "goomba") save_hitCount_goomba = 1;
            if (enemies.first == "goomba2") save_hitCount_goomba2 = 1;
            if (enemies.first == "bowser")  save_hitCount_bowser = 3;

        }
        
    }

    // Guardar el estado de los ítems
    pugi::xml_node itemsNode = SaveFile.child("config").child("scene").child("entities").child("items");
    for (const auto& item : itemStateList) {
        Item* itemEntity = (Item*)Engine::GetInstance().entityManager->GetEntityByName(item.first);
        if (itemEntity != nullptr) {
            itemsNode.child(item.first.c_str()).attribute("isPicked").set_value(itemEntity->GetisPicked());
            itemsNode.child(item.first.c_str()).attribute("x").set_value(itemEntity->GetPosition().getX());
            itemsNode.child(item.first.c_str()).attribute("y").set_value(itemEntity->GetPosition().getY());
        }
    }

    // Guardar el archivo XML
    SaveFile.save_file("config.xml");
    LOG("Game state saved successfully.");
}

void Scene::menu()
{
    Engine::GetInstance().guiManager->CleanUp();

    SDL_Rect buttonPositions[] = {
   {150, 350, 250, 120},
   {150, 485, 250, 120},
   {150, 620, 250, 120},
   {150, 755, 250, 120},
   {150, 890, 250, 120}
    };
    const char* buttonTexts[] = { "New Game", "Load", "Settings", "Credits", "Leave" };

    for (int i = 0; i < 5; ++i) {
        guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, i + 1, buttonTexts[i], buttonPositions[i], this));
    }
}

void Scene::funcion_menu_pause()
{
     Engine::GetInstance().guiManager->CleanUp();
    int cameraX = Engine::GetInstance().render.get()->camera.x;
    int cameraY = Engine::GetInstance().render.get()->camera.y;

    Engine::GetInstance().render.get()->DrawTexture(menu_pause, -cameraX, -cameraY);
    Engine::GetInstance().render.get()->DrawText("Pause", 768, 195, 360, 60);

    SDL_Rect ResumePosition = { 700, 340 - 30, 365 - 20, 60 };
    SDL_Rect SettingsPosition = { 700, 440 - 30, 378 - 20, 60 };
    SDL_Rect Back_tile = { 700, 540 - 30, 230, 60 };
    SDL_Rect Exit = { 700, 640 - 30, 200, 45 };

    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, "resume", ResumePosition, this));
    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "Settings", SettingsPosition, this));
    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, "Return", Back_tile, this));
    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "Exit", Exit, this));

}

void Scene::Credits()
{
   
    Engine::GetInstance().guiManager->CleanUp();

    int cameraX = Engine::GetInstance().render.get()->camera.x;
    int cameraY = Engine::GetInstance().render.get()->camera.y;

    Engine::GetInstance().render.get()->DrawTexture(black, -cameraX, -cameraY);
    Engine::GetInstance().render.get()->DrawText("Credits", 780, 250, 352, 128);
    Engine::GetInstance().render.get()->DrawText("Toni Llovera Roca", 780, 500, 378, 64);
    Engine::GetInstance().render.get()->DrawText("Jana Puig Sola", 780, 650, 378, 64);
    
}

void Scene::Settings()
{
    Engine::GetInstance().guiManager->CleanUp();

    int cameraX = Engine::GetInstance().render.get()->camera.x;
    int cameraY = Engine::GetInstance().render.get()->camera.y;

    Engine::GetInstance().render.get()->DrawTexture(settings, -cameraX, -cameraY);
    Engine::GetInstance().render.get()->DrawText("SETTINGS", 768, 140, 378, 64);
    Engine::GetInstance().render.get()->DrawText("Music Volume", 450, 300, 378, 64);
    Engine::GetInstance().render.get()->DrawText("Fx Volume", 450, 420, 378, 64);
    Engine::GetInstance().render.get()->DrawText("Full Screen", 450, 540, 378, 64);
    Engine::GetInstance().render.get()->DrawText("Vsync", 450, 660, 250, 50);

    SDL_Rect buttonPosition = { 940, 530, 64, 64 };
    SDL_Rect VsyncPosition = { 940, 655, 64, 64 };
    SDL_Rect MusicPosition = { musicPosX, 311, 15, 35 };
    guiBt->bounds.x = musicPosX; //Botones se reubiquen
    SDL_Rect FxPosition = { FxPosX, 431, 15, 35 };
    guiBt->bounds.x = FxPosX; //Botones se reubiquen

    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "  ", buttonPosition, this));
    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "  ", VsyncPosition, this));
    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "  ", MusicPosition, this));
    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "  ", FxPosition, this));

    //Evitar que Fx Volume y Music Volume se muevan a la vez. Mirar donde está la posición del ratón en el eje de la y
    if (mousePos.getX() >= 940 && mousePos.getX() <= 1350 && mousePos.getY() >= 300 && mousePos.getY() <= 340) {
        mouseOverMusicControl = true;  // El ratón está sobre el control de volumen de música
    }
    else {
        mouseOverMusicControl = false; // El ratón no está sobre el control de música
    }

    // Comprobar si el mouse está sobre el control de FX
    if (mousePos.getX() >= 940 && mousePos.getX() <= 1350 && mousePos.getY() >= 420 && mousePos.getY() <= 460) {
        mouseOverFxControl = true;  // El ratón está sobre el control de volumen de FX
    }
    else {
        mouseOverFxControl = false; // El ratón no está sobre el control de FX
    }

    if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
        if (mouseOverMusicControl) {
            musicButtonHeld = true;  // Activar movimiento solo si estamos sobre el botón de música
        }
        if (mouseOverFxControl) {
            FxButtonHeld = true;  // Activar movimiento solo si estamos sobre el botón de FX
        }
    }

    if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
        FxButtonHeld = false;  // Activar el movimiento solo si estamos presionando el botón. Fx Volumen       
        musicButtonHeld = false;  // Si se suelta el botón, desactivar el movimiento. Music Volumen         
    }

    if (musicButtonHeld) {

        if (mousePos.getX() < 940) {
            musicPosX = 940;
        }
        else if (mousePos.getX() > 1350) {
            musicPosX = 1350;
        }
        else {
            musicPosX = mousePos.getX();
        }
    }

    if (FxButtonHeld) {

        if (mousePos.getX() < 940) {
            FxPosX = 940;
        }
        else if (mousePos.getX() > 1350) {
            FxPosX = 1350;
        }
        else {
            FxPosX = mousePos.getX();
        }
    }

    SDL_Rect newMusicPos = { musicPosX, 311, 15, 35 }; // Nueva posición. Music Volumen 
    guiBt->bounds = newMusicPos;

    SDL_Rect newFxPos = { FxPosX, 431, 15, 35 }; // Nueva posición. Music Volumen 
    guiBt->bounds = newFxPos;

    //Subir bajar volumen de la música
    float volume = (float)(musicPosX - 940) / (1350 - 940);
    volume = std::max(0.0f, std::min(1.0f, volume));
    sdlVolume = (int)(volume * MIX_MAX_VOLUME);
    Mix_VolumeMusic(sdlVolume);

    //Subir bajar volumen de Fx
    float volumeFx = (float)(FxPosX - 940) / (1350 - 940);
    int sdlVolumeFx = (int)(volumeFx * MIX_MAX_VOLUME);
    Mix_Volume(-1, sdlVolumeFx);

    //Rectángulo volumen musica
    Engine::GetInstance().render.get()->DrawRectangle({ 940, 300 + 20, 400 + 20, 19 }, 64, 224, 208, 200, true, false);
    //Rectángulo Fx musica
    Engine::GetInstance().render.get()->DrawRectangle({ 940, 420 + 20, 400 + 20, 19 }, 64, 224, 208, 200, true, false);

    ToggleFullscreen(); //Full screen

    if (activatebotton7 == true) {
        LOG("Drawing tick because limitFPS is enabled.");
        Engine::GetInstance().render.get()->DrawTexture(tick, 940, 645);  // Mostrar el "tick". Vsync
    }

}
bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
    pugi::xml_document SaveFile;
    pugi::xml_document LoadFile;

    pugi::xml_parse_result result_load = LoadFile.load_file("config.xml");

    pugi::xml_parse_result result = SaveFile.load_file("config.xml");
    pugi::xml_node fullscreenNode;
    static bool wasClicked = true;

    // L15: DONE 5: Implement the OnGuiMouseClickEvent method
    switch (control->id) {
    case 1: // Menu; New Game
        CreateLevelItems(1);
        LOG("New Game button clicked");

        //Engine::GetInstance().entityManager.get()->ResetItems();

        StartNewGame();
        player->isDead = false;
        player->canMove = true;
        Engine::GetInstance().entityManager->puntuation = 0;
        Engine::GetInstance().entityManager->lives = 3;
        Engine::GetInstance().entityManager->objects = 0;
        levelTime = 150000.0f;
        timeUp = false;

        Engine::GetInstance().audio.get()->PlayFx(MenuStart);
        Engine::GetInstance().audio.get()->PlayFx(marioTime);
        ShowTransitionScreen();
        
        break;

    case 2: // Menu; Load Game
        if (isSave == 1 || LoadFile.child("config").child("scene").child("SaveFile").attribute("isSave").as_int() == 1) {
            LOG("Load Game button clicked");
            LoadGame();
            Engine::GetInstance().audio.get()->StopMusic();
            isLoading = true;
            Engine::GetInstance().audio.get()->PlayFx(MenuStart);
        }

        break;

    case 3: //Menu: Setings Gamme

         showSettings = true;

        break;

    case 4:// Menu: Credits Game

        showCredits = true;

        break;

    case 5:// Menu and Menu pause: Leave Game

        LOG("Leave button clicked");
        Engine::GetInstance().CleanUp();
        exit(0);

        break;

    case 6:// Settings: Full screen window
        LOG("Leave button clicked");
        fullscreenNode = SaveFile.child("config").child("window").child("fullscreen_window");

        if (fullscreen_window == false)
        {
            if (fullscreenNode) {
                fullscreenNode.attribute("value").set_value("true");
            }
            else {
                LOG("Error: fullscreen_window node not found.");
                return false;
            }

            // Guardar el archivo XML
            if (SaveFile.save_file("config.xml")) {
                LOG("XML file saved successfully");
            }
            else {
                LOG("Error saving XML file");
                return false;
            }

            // Leer el valor modificado de fullscreen_window
            fullscreen_window = SaveFile.child("config").child("window").child("fullscreen_window").attribute("value").as_bool();

            LOG("Fullscreen Window: %s", fullscreen_window ? "true" : "false");
        }

        else if (fullscreen_window == true) {
            if (fullscreenNode) {
                fullscreenNode.attribute("value").set_value("false");
            }
            else {
                LOG("Error: fullscreen_window node not found.");
                return false;
            }

            // Guardar el archivo XML
            if (SaveFile.save_file("config.xml")) {
                LOG("XML file saved successfully");
            }
            else {
                LOG("Error saving XML file");
                return false;
            }

            // Leer el valor modificado de fullscreen_window
            fullscreen_window = SaveFile.child("config").child("window").child("fullscreen_window").attribute("value").as_bool();

            LOG("Fullscreen Window: %s", fullscreen_window ? "true" : "false");
             }
        break;

    case 7:// Settings: FPS Toggle

         if (wasClicked) {
            // Alternar limitFPS
            Engine::GetInstance().limitFPS = !Engine::GetInstance().limitFPS;
            Engine::GetInstance().maxFrameDuration = Engine::GetInstance().limitFPS ? 34 : 16;

            std::cout << "FPS Limit toggled: " << (Engine::GetInstance().limitFPS ? "Enabled" : "Disabled") << std::endl;

            activatebotton7 = Engine::GetInstance().limitFPS;

            wasClicked = false;
         }

        else if (activatebotton7 || !activatebotton7) {
            wasClicked = true;
        }
            
        break;

    case 8:// Settings: Music Volume

            musicButtonHeld = true;

        break;

    case 9:// Settings: Fx Volume

            FxButtonHeld = true;

        break;

    case 10: //Menu pause: Back to tile

            showMainMenu = true;

        break;

    case 11: //Menu pause: Settings

            manage_showSettings = true;

        break;

    case 12: //Menu pause: Resume

        showPauseMenu = true;
        LOG("Calling player->ResumeMovement()");

        player->ResumeMovement();

        for (Enemy* enemy : enemyList) { //Renundar movimineto del enemigo
            if (enemy != nullptr) {
                enemy->visible = true;
                enemy->ResumeMovement();
            }
            else {
                continue;
            }

        }
        for (Item* item : itemList) { //Ver items
            item->apear = true;
        }
        showPauseMenu = false; // Cerrar el menú al presionar Backspace        

        break;
    }

    return true;
}
void Scene::GameOver() {
    showMainMenu = true;
    Engine::GetInstance().audio.get()->StopMusic(0.2f);  // Detener música
}
void Scene::DrawLives() {
    char livesText[64];
    sprintf_s(livesText, "Lives: %d", Engine::GetInstance().entityManager->lives);  // Muestra el número de vidas
    Engine::GetInstance().render.get()->DrawText(livesText, 600, 20, 80, 30);  // Ajusta la posición y tamaño
}
void Scene::DrawObject() {
    char objectText[100];
    sprintf_s(objectText, "Objects: %d", Engine::GetInstance().entityManager->objects);
    Engine::GetInstance().render.get()->DrawText(objectText, 850, 20, 90, 35);
}
void Scene::DrawPuntation() {
    char puntuationText[100];
    sprintf_s(puntuationText, "Puntuation: %.1f", Engine::GetInstance().entityManager->puntuation);
    Engine::GetInstance().render.get()->DrawText(puntuationText, 200, 20, 230, 30);

}
void Scene::DrawWorld() {
    char drawlevel[64];
    sprintf_s(drawlevel, "World: 1-%d", level);
    Engine::GetInstance().render.get()->DrawText(drawlevel, 1125, 20, 90, 35);

}

void Scene::ToggleFullscreen()
{
    pugi::xml_document LoadFile;
    pugi::xml_parse_result result = LoadFile.load_file("config.xml");
    if (result == NULL) {
        LOG("Error Loading config.xml: %s", result.description());
        return;
    }

    pugi::xml_node fullscreenNode = LoadFile.child("config").child("window").child("fullscreen_window");
    bool fullscreen = fullscreenNode.attribute("value").as_bool();  // Leer el valor de fullscreen_window

    SDL_Window* window = Engine::GetInstance().window.get()->window;
    if (window != nullptr) {
        if (fullscreen) {
            // Activar pantalla completa
            if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0) {
                LOG("Error setting fullscreen: %s", SDL_GetError());
            }
            Engine::GetInstance().render.get()->DrawTexture(tick, 940, 525);
        }
        else {
            // Volver al modo ventana
            if (SDL_SetWindowFullscreen(window, 0) != 0) {  // 0 para modo ventana
                LOG("Error setting windowed mode: %s", SDL_GetError());
            }
        }
        LoadFile.save_file("config.xml");
    }
    else {
        LOG("Window is not initialized properly.");
    }
}
