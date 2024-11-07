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

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	Player* GetPlayer() const { return player; }
	
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
};