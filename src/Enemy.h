#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;


class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

public:
	bool isDead = false;

private:

	SDL_Texture* texture;
	SDL_Texture* texture_fly;

	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation* currentAnimation_fly = nullptr;

	Animation idle;
	Animation dead;

	Animation idlekoopa;
	Animation deadkoopa;

	PhysBody* pbody;
	Pathfinding* pathfinding;

	//Animation enemy walking
	float deathTimer = 0.0f;
	float frameTime = 0;
	float frameDuration = 130.0f;
	int currentFrame = 0;
	int totalFrames = 3;

	bool movingRight = true;
	bool movingLeft = false;


	float speed = 1.0f;
	float leftBoundary = 0.0f;
	float rightBoundary = 0.0f;
};
