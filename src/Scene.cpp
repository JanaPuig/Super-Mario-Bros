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

using namespace std;
Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{}

bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	
//Coin Item Creation
	int startX = 1600; // Posición inicial en X
	int startY = 768;  // Posición inicial en Y
	int numItems = 7;  // Número de ítems en cada fila
	int spacing = 32;  // Espacio entre ítems
	int numRows = 3;   // Número de filas
	int rowSpacing = 64; // Espacio entre filas

	for (int row = 0; row < numRows; ++row) {
		for (int i = 0; i < numItems; ++i) {
			Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
			item->position = Vector2D(startX + i * spacing, startY - row * rowSpacing);
		}
	}
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//Load Map, Music and Textures
	Engine::GetInstance().map->Load("Assets/Maps/", "Background.tmx");
	Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav", 0.5f);
	pipeFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Pipe.wav");
	CastleFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Music/StageClear_Theme.wav");
	mainMenu = Engine::GetInstance().textures.get()->Load("Assets/Textures/Main_Menu.png");
	helpTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Help_Menu.png");
	return true;
	
}
// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	
	Vector2D playerPos = player->GetPosition();

	//MainMenu code
	if (showMainMenu==true) Engine::GetInstance().render.get()->DrawTexture(mainMenu, -275, -250);
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) showMainMenu = false;
	
	// Handle Help Menu toggle
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN && !ToggleHelpMenu) Scene::ToggleMenu(), ToggleHelpMenu = true; 
		
	if (showHelpMenu == true) {
		int cameraX = Engine::GetInstance().render.get()->camera.x;
		int cameraY = Engine::GetInstance().render.get()->camera.y;
		Engine::GetInstance().render.get()->DrawTexture(helpTexture, -cameraX, -cameraY + 352);
		Engine::GetInstance().render.get()->DrawTexture(mainMenu, -cameraX-275, -cameraY - 250);
	}// Cambia el estado del menú de ayuda

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_UP) {ToggleHelpMenu = false;}

	//Camera Code
	const float cameraMinX = 0.0f;  //Camera Left Limit
	const float cameraMaxX = 4862.0f; //Camera Right Limit
	const float cameraFollowOffset = 192.0f;

	//Camera position to where player is
	float desiredCamPosX = playerPos.getX() - Engine::GetInstance().render.get()->camera.w / 2 + cameraFollowOffset;

	if (desiredCamPosX < cameraMinX) {
		desiredCamPosX = cameraMinX;
	}
	else if (desiredCamPosX > cameraMaxX) {
		desiredCamPosX = cameraMaxX;
	}

	//Apply camera position
	Engine::GetInstance().render.get()->camera.x = -desiredCamPosX;


	//TELEPORT CODE
	//Going to the underground
	if ((playerPos.getX() >= 1440 - tolerance && playerPos.getX() <= 1440 + tolerance &&
		playerPos.getY() >= 288 - tolerance && playerPos.getY() <= 288 + tolerance) ||
		(playerPos.getX() >= 1472 - tolerance && playerPos.getX() <= 1472 + tolerance &&
			playerPos.getY() >= 288 - tolerance && playerPos.getY() <= 288 + tolerance)) {
		if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)) 
		{
			Engine::GetInstance().audio.get()->PlayFx(pipeFxId);
			player->SetPosition(Vector2D(31, 11)); }
	}
	//Getting out of the underground
		else if(playerPos.getX() >= 1856 - 5 && playerPos.getX() <= 1856 + 5 &&
		playerPos.getY() >= 864 - 5 && playerPos.getY() <= 864 + 5)
	{
		if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)) {
			player->SetPosition(Vector2D(36, 6));
			Engine::GetInstance().audio.get()->PlayFx(pipeFxId);
			LOG("Teleporting player to (1505, 864)");	}
	}

	//Entering the castle
		else if ((playerPos.getX() >= 6527 - tolerance && playerPos.getX() <= 6527 + tolerance &&
		playerPos.getY() >= 416 - tolerance && playerPos.getY() <= 416 + tolerance) ||
		(playerPos.getX() >= 6527 - tolerance && playerPos.getX() <= 6527 + tolerance &&
			playerPos.getY() >= 384 - tolerance && playerPos.getY() <= 384 + tolerance) ||
		(playerPos.getX() >= 6527 - tolerance && playerPos.getX() <= 6527 + tolerance &&
			playerPos.getY() >= 352 - tolerance && playerPos.getY() <= 352 + tolerance))
	{
		Engine::GetInstance().audio.get()->PlayFx(CastleFxId);
		player->SetPosition(Vector2D(3, 8.2));
		Engine::GetInstance().map.get()->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Map2.tmx");
	} 

	//Get mouse position and obtain the map coordinate
	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
	Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap(mousePos.getX() - Engine::GetInstance().render.get()->camera.x,
																	 mousePos.getY()-Engine::GetInstance().render.get() ->camera.y);

	//Pintar tile sobre la que esta el raton
	Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld(mouseTile.getX(), mouseTile.getY());
	//Engine::GetInstance().render.get()->DrawTexture(mouseTileTex, highlightTile.getX()-Engine::GetInstance().map.get()->GetTileWidth()/2, highlightTile.getY());


	//if mouse button pressed modify player position
	if (Engine::GetInstance().input.get()->GetMouseButtonDown(1)==KEY_DOWN)
	{
		player->SetPosition(mousePos);
	}
	LOG("After Teleport: X: %f, Y: %f", player->GetPosition().getX(), player->GetPosition().getY());
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	return ret;
}

// Called before quitting
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
