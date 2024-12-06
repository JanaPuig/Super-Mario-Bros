#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include <vector> 

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}


public:
	//audio Fx
	int pickCoinFxId;
	//Item Pick Flag
	bool isPicked = false;
	std::vector<Item*> items;

private:

	SDL_Texture* coinTexture;
	SDL_Texture* flagTexture;
	SDL_Texture* flagpoleTexture;

	const char* texturePath;
	int texW, texH;

	pugi::xml_node parameters;
	Animation* currentAnimation = &idle;
	Animation idle;

	Animation* currentAnimation_flag = &flag;
	Animation flag;
	Animation lower_flag;
	Animation lower_lower_flag;

	Animation* currentAnimation_flagpole = &flagpole;
	Animation flagpole;

	//L08 TODO 4: Add a physics to an item
	//PhysBody* pbody;
};
