#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
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



public:
	//audio Fx
	int pickCoinFxId;
	//Item Pick Flag
	bool isPicked = false;
	std::vector<Item*> items;

private:

	SDL_Texture* Coin;
	const char* texturePath;
	int texW, texH;

	//L08 TODO 4: Add a physics to an item
	//PhysBody* pbody;
};
