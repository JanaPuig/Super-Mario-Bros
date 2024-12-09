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

	void ResetPath();

	void UpdateColliderSize();
	int GetHitCount();
	bool isDead() {
		if (isDying) {
			return true;
		}
		else {

			return false;
		}
	}

	b2Vec2 MoveEnemy(Vector2D enemyPosition, float speed);

	void ResetPosition();

public:
	int hitCount;

	SDL_Texture* textureGoomba;
	SDL_Texture* textureKoopa;

	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation* currentAnimation_fly = nullptr;

	Animation idleGoomba;
	Animation deadGoomba;

	Animation flyingkoopaLeft;
	Animation flyingkoopaRight;
	Animation deadkoopa;
	b2Vec2 velocity = b2Vec2(0, 0);
	Vector2D nextPos;
	PhysBody* pbody;
	Pathfinding* pathfinding;
	Vector2D lastPlayerTile;
	//Animation enemy walking
	float deathTimer = 0.0f;
	float frameTime = 0;
	float frameDuration = 200.0f;
	int currentFrame = 0;
	int totalFrames = 3;
	float detectionRange = 500.0f; // Rango en pixeles
	bool movingRight = true;
	bool movingLeft = false;
	bool isEnemyDead = false; // Temporizador para la animación de muerte
	bool isDying = false;    // Indica si está en proceso de morir
	float speed = 3.0f;
	bool showPath = false;
};
