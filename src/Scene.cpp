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
    InitialEnemies();
    
    return true;
}

void Scene::InitialItems()
{
    const int startX = 1664, startY = 672;

    pugi::xml_node defaultItemNode = configParameters.child("entities").child("items").child("item");

    Item* item = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    item->position = Vector2D(startX, startY);
    item->SetParameters(defaultItemNode);
    itemStateList.push_back(std::make_pair(std::string(defaultItemNode.attribute("name").as_string()), 0));
    itemList.push_back(item);
    // Log the item's creation
    LOG("Creating item at position: (%f, %f)", item->position.getX(), item->position.getY());

    // Crear flagpole
    const int positionX_flagpole = 3232, positionY_flagpole = 384;
    Item* flagpole = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    flagpole->position = Vector2D(positionX_flagpole, positionY_flagpole);

    pugi::xml_node flagpoleNode = configParameters.child("entities").child("items").child("flagpole");
    flagpole->SetParameters(flagpoleNode);
    itemStateList.push_back(std::make_pair(std::string(flagpoleNode.attribute("name").as_string()), 0));
    itemList.push_back(flagpole);

    LOG("Creating flagpole at position: (%f, %f)", flagpole->position.getX(), flagpole->position.getY());

    // Crear flag
    const int positionX_flag = 3248, positionY_flag = 384;
    Item* flag = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    flag->position = Vector2D(positionX_flag, positionY_flag);

    pugi::xml_node flagNode = configParameters.child("entities").child("items").child("flag");
    flag->SetParameters(flagNode);
    itemStateList.push_back(std::make_pair(std::string(flagNode.attribute("name").as_string()), 0));
    itemList.push_back(flag);

    // Crear 1Up
    const int oneUpPpositionX = 500, oneUpPpositionY = 200;
    Item* OneUp = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    OneUp->position = Vector2D(oneUpPpositionX, oneUpPpositionY);

    pugi::xml_node oneUpeNode = configParameters.child("entities").child("items").child("OneUp");
    OneUp->SetParameters(oneUpeNode);
    itemStateList.push_back(std::make_pair(std::string(oneUpeNode.attribute("name").as_string()), 0));
    itemList.push_back(OneUp);

    LOG("Creating 1UP at position: (%f, %f)", OneUp->position.getX(), OneUp->position.getY());

    //Crear BigCoin
    const int BigCoinpositionX = 800, BigCoinpositionY = 200;
    Item* BigCoin = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    BigCoin->position = Vector2D(BigCoinpositionX, BigCoinpositionY);

    pugi::xml_node BigCoinNode = configParameters.child("entities").child("items").child("BigCoin");
    BigCoin->SetParameters(BigCoinNode);
    itemStateList.push_back(std::make_pair(std::string(BigCoinNode.attribute("name").as_string()), 0));
    itemList.push_back(BigCoin);

    LOG("Creating Star Coin at position: (%f, %f)", BigCoin->position.getX(), BigCoin->position.getY());

    //Crear PowerUp
    const int PowerUppositionX = 800, PowerUppositionY = 300;
    Item* PowerUp = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    PowerUp->position = Vector2D(PowerUppositionX, PowerUppositionY);

    pugi::xml_node PowerUpNode = configParameters.child("entities").child("items").child("PowerUp");
    PowerUp->SetParameters(PowerUpNode);
    itemStateList.push_back(std::make_pair(std::string(PowerUpNode.attribute("name").as_string()), 0));
    itemList.push_back(PowerUp);

    LOG("Creating Power-Up at position: (%f, %f)", PowerUp->position.getX(), PowerUp->position.getY());

    // Crear flagpole del final del nivel
    const int positionX_flagpole2 = 6343, positionY_flagpole2 = 90;
    Item* flagpole2 = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    flagpole2->position = Vector2D(positionX_flagpole2, positionY_flagpole2);

    pugi::xml_node flagpoleNode2 = configParameters.child("entities").child("items").child("finish_flagpole");
    flagpole2->SetParameters(flagpoleNode2);
    itemStateList.push_back(std::make_pair(std::string(flagpoleNode2.attribute("name").as_string()), 0));
    itemList.push_back(flagpole2);

    LOG("Creating flagpole at position: (%f, %f)", flagpole2->position.getX(), flagpole2->position.getY());

    // Crear flag del final del nivel
    const int positionX_flag2 = 6299, positionY_flag2 = 110;
    Item* flag2 = static_cast<Item*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
    flag2->position = Vector2D(positionX_flag2, positionY_flag2);

    pugi::xml_node flagNode2 = configParameters.child("entities").child("items").child("finish_flag");
    flag2->SetParameters(flagNode2);
    itemStateList.push_back(std::make_pair(std::string(flagNode2.attribute("name").as_string()), 0));
    itemList.push_back(flag2);
}
void Scene::InitialEnemies()
{
   // Iteramos a través de los enemigos del archivo XML
    for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").first_child();
        enemyNode;
        enemyNode = enemyNode.next_sibling()) {

        int levels = enemyNode.attribute("levels").as_int();
        std::string enemyName = enemyNode.attribute("name").as_string();


        LOG("Level createenemies: %d", levels);

        // Crear un nuevo enemigo
        Enemy* enemy = static_cast<Enemy*>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));

        if (!enemy) {
            LOG("Failed to create enemy: %s.", enemyName.c_str());
            continue;
        }

        // Establece los parámetros del enemigo
        enemy->SetParameters(enemyNode);

        // Agregar el enemigo a las listas de estados y enemigos
        enemyStateList.push_back(std::make_pair(enemyName, 0));
        enemyList.push_back(enemy);

        LOG("Enemy %s created for level %d", enemyName.c_str(), level);

    }
}
// Creates items for Level 1
void Scene::CreateLevelItems(int level)
{
    if (level == 1) {

        for (auto& it : itemList)
        {
            if (it->name == "flagpole") {
                it->position = (Vector2D(3232, 384));

            }
            else if (it->name == "flag") {
                it->position = (Vector2D(3248, 384));

            }
            else if (it->name == "finish_flagpole") {
                it->position = (Vector2D(6343, 90));

            }
            else if (it->name == "finish_flag") {
                it->position = (Vector2D(6299, 110));

            }
            else if (it->name == "coin") {
                it->position = (Vector2D(1664, 672));

            }
            else if (it->name == "OneUp") {
                it->position = (Vector2D(500, 200));

            }
            else if (it->name == "PowerUp") {
                it->position = (Vector2D(800, 300));

            }
            else if (it->name == "BigCoin") {
                it->position = (Vector2D(800, 200));

            }
            
        }
    }

    else if (level == 2)
    {
       
        Engine::GetInstance().entityManager.get()->ResetItems();

        for (auto& it : itemList) 
        {
            if (it->name == "flagpole") {
                it->position = (Vector2D(3232+70, 384+415));

            }
            else if (it->name == "flag") {
                it->position = (Vector2D(3248 + 70, 384+415));

            }
            else if (it->name == "finish_flagpole") {
                it->position = (Vector2D(6343 + 190, 90 + 320));

            }
            else if (it->name == "finish_flag") {
                it->position = (Vector2D(6299 + 190, 110 + 320));

            }
            else {
                it->position = (Vector2D(-1000, -1000));
            }
        }

    }

    else if (level == 3) {

        for (auto& it : itemList)
        {
            it->position = (Vector2D(-1000, -1000));
        }
    }
   
}

void Scene::CreateEnemies(int level) {
    LOG("Creating enemies for level %d", level);

    if (level == 1) {
        for (auto& e : enemyList) {

            if (e->name == "koopa") {
                e->SetPosition(Vector2D(1500, 100));  // Cambiar la posición de Bowser
            }

            else if (e->name == "koopa2") {
                e->SetPosition(Vector2D(4500, 100));  // Cambiar la posición de Bowser
            }

            else if (e->name == "goomba") {
                e->SetPosition(Vector2D(2000, 400));  // Cambiar la posición de Bowser

            }
            else if (e->name == "goomba2") {
                e->SetPosition(Vector2D(5500, 400));  // Cambiar la posición de Bowser

            }

            else if (e->name == "bowser") {
                e->SetPosition(Vector2D(-1000, -1000));  // Cambiar la posición de Bowser
            }
        }
        activate_gravity_goomba = true;

    }

    if (level == 2) {
        //Mover posiciones enemigos
        for (auto& e : enemyList) {
            e->SetPosition(Vector2D(-1000, -1000));
        }
        activate_gravity_goomba = false;

    }

    else if (level == 3) {
        // Mover Bowser a la nueva posición
        for (auto& e : enemyList) {
            if (e->name == "bowser") {

                e->SetPosition(Vector2D(5500, 320));  // Cambiar la posición de Bowser
                LOG("Bowser moved to (4500, 300) in level 2");
            }
            else {
                e->SetPosition(Vector2D(-1000, -1000));
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
    if (showWinScreen) {
        int cameraX = Engine::GetInstance().render.get()->camera.x;
        int cameraY = Engine::GetInstance().render.get()->camera.y;

        // Dibuja la pantalla negra
        Engine::GetInstance().render.get()->DrawTexture(black, cameraX, cameraY);

        // Dibuja el texto "YOU WIN!"
        Engine::GetInstance().render.get()->DrawText("YOU WIN!", 960, 400, 200, 100); 

        // Centra al jugador en la pantalla
        Vector2D playerPos = Vector2D(960 - (player->texW / 2), 500); 
        player->SetPosition(playerPos);

        // Dibuja la animación de victoria del jugador

        Engine::GetInstance().render.get()->DrawTexture(player->texturePlayer, (int)playerPos.getX(), (int)playerPos.getY(), &player->winAnimation.GetCurrentFrame());
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
            showWinScreen = false; 
            showMainMenu = true; 
        }
        return true;
    }

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
        // Si el temporizador supera la duración del logo, ocultarlo
        if (logoTimer >= logoDuration)
        {
            showGroupLogo = false;
            showMainMenu = true;  // Mostrar el menú principal
        }
        return true;  // Evitar que se ejecute el resto del código mientras el logo está activo
    }
    // Si estamos en el menú principal, no se procesan otras lógicas
    if (showMainMenu) {
        // Reproducir GameIntro solo una vez
        if (!isGameIntroPlaying) {
            Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GameIntro.wav", 0.f);
            isGameIntroPlaying = true;
            }
        menu();
        Engine::GetInstance().render.get()->DrawTexture(mainMenu, -cameraX, -cameraY); // Dibujar el fondo del menú principal
        Engine::GetInstance().guiManager->Draw();
        if (showCredits) {
            Credits(); // Si showCredits es true, dibuja la pantalla de cr�ditos
            if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
                showCredits = false; // Vuelve al menu si se presiona backspace
            }
            return true;
        }

        if (showSettings) {
            Settings();
         
            mousePos = Engine::GetInstance().input->GetMousePosition();
            LOG("Mouse X: %f, Mouse Y: %f", mousePos.getX(), mousePos.getY());
            Engine::GetInstance().guiManager->Draw();
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
        Engine::GetInstance().render.get()->DrawTexture(loadingScreen, -cameraX, -cameraY);   // Renderiza la pantalla de carga
        if (loadingTimer >= loadingScreenDuration) {     // Si el tiempo de espera ha terminado, carga el estado del juego
            isLoading = false; // Desactiva la pantalla de carga
            Engine::GetInstance().audio.get()->PlayFx(hereWeGo);
            Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav", 0.f);
            LoadState(); // Load Game State
        }
        return true; // Detenemos el resto de la lógica mientras está la pantalla de carga
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
        showPauseMenu = !showPauseMenu; // Alternar el estado del menú

        if (showPauseMenu) {    //Que no se vean los items, los enemigos ni el player
            player->StopMovement(); //Parar movimineto del player
            for (Enemy* enemy : enemyList) { //Parar movimineto del enemigo
                enemy->StopMovement(); 
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
                enemy->ResumeMovement();
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
        Engine::GetInstance().guiManager->Draw();

        if (manage_showSettings) {
            Settings();
            mousePos = Engine::GetInstance().input->GetMousePosition();
            LOG("Mouse X: %f, Mouse Y: %f", mousePos.getX(), mousePos.getY());

            Engine::GetInstance().guiManager->Draw();
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
    if (level == 1 ||level ==2) {
        HandleTeleport(playerPos);
    }

    //position_player();

    // Temporizador del nivel
    elapsedTime += dt;  // Aumenta el tiempo acumulado
    float currentTime = showRemainingTime ? (levelTime - elapsedTime) : elapsedTime;

    // Lógica si se acaba el tiempo
    if ((currentTime/1000) <= 0.0f)
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
    else if (level == 2 && IsInTeleportArea(playerPos, 6690, 764, tolerance))
    {
        Engine::GetInstance().audio.get()->PlayFx(CastleFxId);
        ChangeLevel(3);

    }
    else if (level == 3 && IsInTeleportArea(playerPos, 5000, 764, tolerance))
    {
        Engine::GetInstance().audio.get()->PlayFx(BowserLaugh);
        Engine::GetInstance().audio.get()->StopMusic();
        Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/BossFight.wav");
        
    }
    else if (level == 3 && IsInTeleportArea(playerPos, 6100, 764, tolerance))
    {
        EndGameScreen();
        Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/WorldClear_Theme.wav");
    }
}
// Checks if the player is in a teleportation area
bool Scene::IsInTeleportArea(const Vector2D& playerPos, float x, float y, float tolerance)
{
    return playerPos.getX() >= x - tolerance && playerPos.getX() <= x + tolerance &&
        playerPos.getY() >= y - tolerance && playerPos.getY() <= y + tolerance;
}
void Scene::EndGameScreen() {
    showWinScreen = true; // Activa la pantalla de victoria
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

void Scene::position_player() 
{
    if (level == 1 && player->GetPosition().getY()== 429) 
    {
        activate_gravity_goomba = true;
    }
    else {
        activate_gravity_goomba = false;

    }
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

    //Engine::GetInstance().entityManager->RemoveAllEnemies();
    Engine::GetInstance().map->CleanUp();
    //Engine::GetInstance().entityManager->RemoveAllItems();

    level = newLevel;
    CreateEnemies(level);
    CreateLevelItems(level);
    ShowTransitionScreen();
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
    }
    else if (level == 2) {
        player->SetPosition(Vector2D(100, 740));
        Engine::GetInstance().map->Load(configParameters.child("map2").attribute("path").as_string(), configParameters.child("map2").attribute("name").as_string());
        Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/World2Theme.wav", 0.f);
        Mix_VolumeMusic(sdlVolume);
    }
    else if (level == 3) {
        player->SetPosition(Vector2D(100, 580));
        Engine::GetInstance().map->Load(configParameters.child("map3").attribute("path").as_string(), configParameters.child("map3").attribute("name").as_string());
        Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/CastleTheme.wav", 0.f);
        Mix_VolumeMusic(sdlVolume);
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
    saveNode.attribute("checkpoint").set_value(false);
    saveNode.attribute("live").set_value(3);
    saveNode.attribute("object").set_value(0);
    saveNode.attribute("Puntuation").set_value(0);
    saveNode.attribute("isSave").set_value(false);
  

    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("hitcount").set_value(0);
    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2").attribute("hitcount").set_value(0);
    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("hitcount").set_value(0);
    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy2").attribute("hitcount").set_value(0);
 
    SaveFile.child("config").child("scene").child("entities").child("items").child("item").attribute("isPicked").set_value(0);

    // Reiniciar enemigos
    saveNode.attribute("enemy1X").set_value(1500);
    saveNode.attribute("enemy1Y").set_value(100);
    saveNode.attribute("enemy2X").set_value(4500);
    saveNode.attribute("enemy2Y").set_value(100);
    saveNode.attribute("enemy3X").set_value(2000);
    saveNode.attribute("enemy3Y").set_value(415);
    saveNode.attribute("enemy4X").set_value(5500);
    saveNode.attribute("enemy4Y").set_value(415);
    saveNode.attribute("itemX").set_value(1600);
    saveNode.attribute("itemY").set_value(768);


    SaveFile.save_file("config.xml");
    isFlaged = saveNode.attribute("checkpoint").as_bool();
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

    //Cargar enemigos
    for (int i = 1; i <= 4; ++i) {
        std::string enemyPosX = "enemy" + std::to_string(i) + "X";
        std::string enemyPosY = "enemy" + std::to_string(i) + "Y";

        int posX = saveNode.attribute(enemyPosX.c_str()).as_int();
        int posY = saveNode.attribute(enemyPosY.c_str()).as_int();

        //koopa
        if (i == 1) {
            Enemy* enemyKoopa = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName("koopa");
            enemyKoopa->SetPosition(Vector2D(posX, posY));
        }
        //Koopa2
        else if (i == 2) {
            Enemy* enemyKoopa2 = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName("koopa2");
            enemyKoopa2->SetPosition(Vector2D(posX, posY));
        }
        //Goomba
        else if (i == 3) {
            Enemy* enemyGoomba = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName("goomba");
            enemyGoomba->SetPosition(Vector2D(posX, posY));
        }
        //Goomba2
        else if (i == 4) {
            Enemy* enemyGoomba2 = (Enemy*)Engine::GetInstance().entityManager->GetEntityByName("goomba2");
            enemyGoomba2->SetPosition(Vector2D(posX, posY));
        }

    }
    // Si hay más datos a cargar, puedes hacerlo aquí.
    //cargar checkpoint
    Engine::GetInstance().entityManager->lives = LoadFile.child("config").child("scene").child("SaveFile").attribute("lives").as_int();
    Engine::GetInstance().entityManager->objects = LoadFile.child("config").child("scene").child("SaveFile").attribute("object").as_int();
    Engine::GetInstance().entityManager->puntuation = LoadFile.child("config").child("scene").child("SaveFile").attribute("Puntuation").as_int();
    elapsedTime = LoadFile.child("config").child("scene").child("SaveFile").attribute("time").as_float();
    isFlaged= saveNode.attribute("checkpoint").as_bool();
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
    elapsedTime = LoadFile.child("config").child("scene").child("SaveFile").attribute("time").as_float();
    Engine::GetInstance().entityManager->lives = LoadFile.child("config").child("scene").child("SaveFile").attribute("lives").as_int();
    Engine::GetInstance().entityManager->objects = LoadFile.child("config").child("scene").child("SaveFile").attribute("object").as_int();
    Engine::GetInstance().entityManager->puntuation=LoadFile.child("config").child("scene").child("SaveFile").attribute("Puntuation").as_int();
    posPlayer.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("playerX").as_int());
    posPlayer.setY(LoadFile.child("config").child("scene").child("SaveFile").attribute("playerY").as_int());
    int savedLevel = LoadFile.child("config").child("scene").child("SaveFile").attribute("level").as_int();
    Vector2D posKoopa, posGoomba;
    int KoopaHitcount = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("hitcount").as_int();
    int KoopaHitcount2 = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2").attribute("hitcount").as_int();
    int GoombaHitcount = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("hitcount").as_int();
    int GoombaHitcount2 = LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy2").attribute("hitcount").as_int();
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
                    enemEntity->hitCount = GoombaHitcount2;
                }
                else if (node.attribute("name").as_string() == enemEntity->name)
                {
                    enemEntity->hitCount = KoopaHitcount;
                }
                else if (node.attribute("name").as_string() == enemEntity->name)
                {
                    enemEntity->hitCount = KoopaHitcount2;
                }
            }
        }
    }
    if (KoopaHitcount == 0) {
        posKoopa.setX(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("x").as_int());
        posKoopa.setY(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("y").as_int());

        posEnemy.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("enemy1X").as_int());
        posEnemy.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("enemy1Y").as_int());
    }
    if (KoopaHitcount2 == 0) {
        posKoopa.setX(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2").attribute("x").as_int());
        posKoopa.setY(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2").attribute("y").as_int());

        posEnemy.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("enemy2X").as_int());
        posEnemy.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("enemy2Y").as_int());
    }
    if (GoombaHitcount == 0) {
        posGoomba.setX(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("x").as_int());
        posGoomba.setY(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("y").as_int());

        posEnemy.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("enemy3X").as_int());
        posEnemy.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("enemy3Y").as_int());
    }
    if (GoombaHitcount2 == 0) {
        posGoomba.setX(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy2").attribute("x").as_int());
        posGoomba.setY(LoadFile.child("config").child("scene").child("entities").child("enemies").child("enemy2").attribute("y").as_int());

        posEnemy.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("enemy4X").as_int());
        posEnemy.setX(LoadFile.child("config").child("scene").child("SaveFile").attribute("enemy4Y").as_int());
    }
    //Item
    int isPicked = LoadFile.child("config").child("scene").child("entities").child("items").child("item").attribute("isPicked").as_int();
    Item* item = static_cast<Item*>(Engine::GetInstance().entityManager->GetEntityByName("item_name")); // Obtén la referencia al ítem
    if (item != nullptr) {
        item->SetIsPicked(isPicked); // Sincroniza el estado del ítem
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
void Scene::UpdateItem(std::string itemName, int isPicked) {
    int i = 0;
    for (std::pair<std::string, int> item : itemStateList)
    {
        if (item.first == itemName)
        {
            //itemStateList[i].second = isPicked;  // Update the state in the list

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
    // read the player position and set the value in the XML
    SaveFile.child("config").child("scene").child("SaveFile").attribute("isSave").set_value(true);

    Vector2D playerPos = player->GetPosition();
    SaveFile.child("config").child("scene").child("SaveFile").attribute("lives").set_value(Engine::GetInstance().entityManager->lives);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("object").set_value(Engine::GetInstance().entityManager->objects);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("Puntuation").set_value(Engine::GetInstance().entityManager->puntuation);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("level").set_value(level);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("playerX").set_value(playerPos.getX());
    SaveFile.child("config").child("scene").child("SaveFile").attribute("playerY").set_value(playerPos.getY());
    SaveFile.child("config").child("scene").child("SaveFile").attribute("checkpoint").set_value(isFlaged);
    SaveFile.child("config").child("scene").child("SaveFile").attribute("time").set_value(elapsedTime);
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
                else if (enemy.first == "koopa2")
                {
                    //Guardo el hitcount que tengo en la lista de enemigos
                    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2").attribute("hitcount").set_value(enemy.second);
                }
                else if (enemy.first == "goomba")
                {
                    //Guardo el hitcount que tengo en la lista de enemigos
                    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("hitcount").set_value(enemy.second);
                }
                else if (enemy.first == "goomba2")
                {
                    //Guardo el hitcount que tengo en la lista de enemigos
                    SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy2").attribute("hitcount").set_value(enemy.second);
                }
                // Si el enemigo no esta muerto actualizo su posicion 
                if (enemy.second == 0)
                {
                    Vector2D posEnemy;
                    if (enemy.first == "koopa")
                    {
                        SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("x").as_int();
                        SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa").attribute("y").as_int();

                        SaveFile.child("config").child("scene").child("SaveFile").attribute("enemy1X").set_value(enemyEntity->GetPosition().getX() - 44);
                        SaveFile.child("config").child("scene").child("SaveFile").attribute("enemy1Y").set_value(enemyEntity->GetPosition().getY() - 44);
                    }
                    else if (enemy.first == "koopa2")
                    {
                        SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2").attribute("x").as_int();
                        SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy_koopa2").attribute("y").as_int();

                        SaveFile.child("config").child("scene").child("SaveFile").attribute("enemy2X").set_value(enemyEntity->GetPosition().getX() - 44);
                        SaveFile.child("config").child("scene").child("SaveFile").attribute("enemy2Y").set_value(enemyEntity->GetPosition().getY() - 44);

                    }
                    else if (enemy.first == "goomba")
                    {
                        SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy").attribute("x").as_int();

                        SaveFile.child("config").child("scene").child("SaveFile").attribute("enemy3X").set_value(enemyEntity->GetPosition().getX() - 16);

                    }
                    else if (enemy.first == "goomba2")
                    {
                        SaveFile.child("config").child("scene").child("entities").child("enemies").child("enemy2").attribute("x").as_int();

                        SaveFile.child("config").child("scene").child("SaveFile").attribute("enemy4X").set_value(enemyEntity->GetPosition().getX() - 16);
                    }
                }
            }
        }
    }
    //Item
    pugi::xml_node itemNode = SaveFile.child("config").child("scene").child("entities").child("items").child("item");
    Item* item = static_cast<Item*>(Engine::GetInstance().entityManager->GetEntityByName("item_name"));
    if (item != nullptr) {
        itemNode.attribute("isPicked").set_value(item->GetisPicked());
    }
    // save the XML modification to disk
    SaveFile.save_file("config.xml");
    //guardar mas cosas; enemies, items...
}

void Scene::menu()
{
    Engine::GetInstance().guiManager->CleanUp();

    active_menu = true;
    active_settings = false;
    active_menu_pause = false;

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
    active_menu = false;
    active_settings = false;
    active_menu_pause = true;

    Engine::GetInstance().guiManager->CleanUp();
    int cameraX = Engine::GetInstance().render.get()->camera.x;
    int cameraY = Engine::GetInstance().render.get()->camera.y;

    Engine::GetInstance().render.get()->DrawTexture(menu_pause, -cameraX, -cameraY);
    Engine::GetInstance().render.get()->DrawText("Pause", 768, 195, 360, 60);

    SDL_Rect ResumePosition = { 700, 340 - 30, 365-20, 60 };
    SDL_Rect SettingsPosition = { 700, 440 - 30, 378-20, 60 };
    SDL_Rect Back_tile = { 700, 540 - 30, 230, 60 };
    SDL_Rect Exit = { 700, 640 - 30, 200, 45 };

    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, "resume", ResumePosition, this));
    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "Settings", SettingsPosition, this));
    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, "Return", Back_tile, this));
    guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "Exit", Exit, this));

}

void Scene::Credits()
{
    active_menu = false;
    active_settings = false;
    active_menu_pause = false;
    if (!active_settings && !active_menu_pause) {
        Engine::GetInstance().guiManager->CleanUp();

        int cameraX = Engine::GetInstance().render.get()->camera.x;
        int cameraY = Engine::GetInstance().render.get()->camera.y;

        Engine::GetInstance().render.get()->DrawTexture(black, -cameraX, -cameraY);
        Engine::GetInstance().render.get()->DrawText("Credits", 780, 250, 352, 128);
        Engine::GetInstance().render.get()->DrawText("Toni Llovera Roca", 780, 500, 378, 64);
        Engine::GetInstance().render.get()->DrawText("Jana Puig Sola", 780, 650, 378, 64);
    }
}

void Scene::Settings()
{
    active_menu = false;
    active_settings = true;
    active_menu_pause = false;

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
    pugi::xml_parse_result result = SaveFile.load_file("config.xml");
    pugi::xml_node fullscreenNode;
    static bool wasClicked = true;

    pugi::xml_document LoadFile;
    pugi::xml_parse_result result_load = LoadFile.load_file("config.xml");

    // L15: DONE 5: Implement the OnGuiMouseClickEvent method
    switch (control->id) {
    case 1: // Menu; New Game
        if (active_menu) {
            LOG("New Game button clicked");

            Engine::GetInstance().entityManager.get()->ResetItems();

            StartNewGame();
            player->isDead = false;
            player->canMove = true;
            Engine::GetInstance().entityManager->puntuation = 0;
            Engine::GetInstance().entityManager->lives = 3;
            Engine::GetInstance().entityManager->objects = 0;
            levelTime = 90000.0f;
            timeUp = false;

            Engine::GetInstance().audio.get()->PlayFx(MenuStart);
            Engine::GetInstance().audio.get()->PlayFx(marioTime);
            ShowTransitionScreen();
        }   
        break;
    case 2: // Menu; Load Game
        
        if (LoadFile.child("config").child("scene").child("SaveFile").attribute("isSave").as_bool() == true && active_menu) {
            LOG("Load Game button clicked");
            LoadGame();
            Engine::GetInstance().audio.get()->StopMusic();
            isLoading = true;
            Engine::GetInstance().audio.get()->PlayFx(MenuStart);         

        }
        break;

    case 3: //Menu: Setings Gamme
        if (active_menu) {
            showSettings = true;
        }
        break;
    case 4:// Menu: Credits Game
        if (active_menu) {
            showCredits = true;
        }
        break;
    case 5:// Menu and Menu pause: Leave Game
        if (active_menu || menu_pause) {
            LOG("Leave button clicked");
            Engine::GetInstance().CleanUp();
            exit(0);
        }
        break;
    case 6:// Settings: Full screen window
        if (active_settings) {
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
        }
        break;

    case 7:// Settings: FPS Toggle
        if (active_settings) {


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
        }
        break; 

    case 8:// Settings: Music Volume
        if (active_settings) {
            musicButtonHeld = true;
        }
        break;
    case 9:// Settings: Fx Volume
        if (active_settings) {
            FxButtonHeld = true;
        }
        break;
    case 10: //Menu pause: Back to tile
        if (active_menu_pause) {
            showMainMenu = true;

        }
        break;
    case 11: //Menu pause: Settings
        if (active_menu_pause) {
            Engine::GetInstance().entityManager->ResetEnemies();

            manage_showSettings = true;
        }
        break;
    case 12: //Menu pause: Resume
        if (active_menu_pause) {

        showPauseMenu = true;
        LOG("Calling player->ResumeMovement()");

        player->ResumeMovement();

        for (Enemy* enemy : enemyList) {
            enemy->ResumeMovement();
        }
        showPauseMenu = false; // Cerrar el menú al presionar Backspace        
        }
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
