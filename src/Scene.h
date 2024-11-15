#pragma once

#include "Module.h"
#include "Player.h"

struct SDL_Texture;

class Scene : public Module
{
public:
	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	void CreateLevel1Items();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	void HandleTeleport(const Vector2D& playerPos);

	bool IsInTeleportArea(const Vector2D& playerPos, float x, float y, float tolerance);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	Player* GetPlayer() const { return player; }

	void ChangeLevel(int newLevel);

	int level = 0; //Map Level
	//Help menu
	void ToggleMenu();
	SDL_Texture* helpTexture = NULL;// Textura para la imagen del menu de ayuda
	bool showHelpMenu = false;
	bool ToggleHelpMenu = false;
	//Main menu
	SDL_Texture* mainMenu = NULL;
	bool showMainMenu = true;
	//fx
	int pipeFxId = 0;
	int CastleFxId = 0;
	float tolerance = 20.0f;

private:
	Player* player;

	// Nuevas variables para las texturas de transición
	SDL_Texture* level1Transition = NULL;
	SDL_Texture* level2Transition = NULL;

	// Duración y temporizador para la pantalla de transición
	float transitionDuration = 3100; 
	float transitionTimer = 0.0f;
	bool showingTransition = false;

	void ShowTransitionScreen();
	void FinishTransition();
};