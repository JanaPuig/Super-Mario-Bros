#pragma once
#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"


struct SDL_Texture;

class Player : public Entity
{
public:
	Player();
	virtual ~Player();
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB)override;
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB)override;
	Vector2D GetPosition() const { return position; }
	void SetPosition(const Vector2D& newPosition);
	void ToggleGodMode();
	void PlayerFlight(float dt);

public:

	//Declare player parameters
	float speed = 5.0f;
	int texW, texH;
	bool isMoving = false;
	bool isSprinting = false;
	bool isDead = false;
	bool deathSoundPlayed = false;

	//Audio fx
	int jumpFxId= 0;
	int jumpVoiceIds[8];
	int DeathId = 0;
	int StartId = 0;
	int ohNoId = 0;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 3.0f; // The force to apply when jumping
	const float jumpFrameDuration = 120.0f;
	
	float desiredCamPosX = 0;
	//Animation Parameters
	int frameWidth = 0;
	int frameHeight = 0; // Tamaño de cada cuadro de la animación
	int currentFrame = 0;         // Cuadro actual de la animación
	float animationTimer = 0.0f;  // Temporizador de la animación
	float frameDuration = 130.0f;   // Duración de cada cuadro (en segundos)

	SDL_Texture* helpTexture = NULL; // Textura para la imagen del menu de ayuda
	SDL_Texture* idleTexture = NULL; // Textura para el spritesheet idle
	SDL_Texture* jumpTexture = NULL; // Textura para el spritesheet de saltar
	SDL_Texture* die = NULL;
	SDL_Texture* gameOver = NULL;
	SDL_Texture* walkingTexture = NULL;

	//BOOLEANS
	bool showHelpMenu = false;
	bool ToggleHelpMenu = false;
	bool facingLeft = false;
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool godMode = false;// Flag to check if the player is currently in god mode
	bool godModeToggle = false;
	
};