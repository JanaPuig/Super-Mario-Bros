#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Enemy.h"
#include "EntityManager.h"
#include "tracy/Tracy.hpp"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}
Player::~Player() {
}
bool Player::Awake() {
	return true;
}
bool Player::Start() {
	// Load FX 
	for (int i = 0; i < 8; ++i)
	{
		std::string path = "Assets/Audio/Fx/MarioVoices/Jump_Random_" + std::to_string(i + 1) + ".wav";
		jumpVoiceIds[i] = Engine::GetInstance().audio.get()->LoadFx(path.c_str());
	}
	jumpFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Jump_Small.wav");
	hereWeGoAgain = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/HereWeGoAgain.wav");
	DeathId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Mario_Death.wav");
	ohNoId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/Death.wav");
	EnemyDeathSound = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Stomp.wav");
	BowserHit = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/BowserHit.wav");
	PowerDown = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/PowerDown.wav");
	//Load Player Texture
	texturePlayer = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_player").as_string());

	//Player Configuration 
	position.setX(parameters.attribute("x").as_float());
	position.setY(parameters.attribute("y").as_float());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load Animations
	idleRAnimation.LoadAnimations(parameters.child("animations").child("idle"));
	idleLAnimation.LoadAnimations(parameters.child("animations").child("idleL"));
	walkingRAnimation.LoadAnimations(parameters.child("animations").child("walking"));
	walkingLAnimation.LoadAnimations(parameters.child("animations").child("walkingL"));
	jumpRAnimation.LoadAnimations(parameters.child("animations").child("jumping"));
	jumpLAnimation.LoadAnimations(parameters.child("animations").child("jumpingL"));
	deadAnimation.LoadAnimations(parameters.child("animations").child("die"));
	crouchLAnimation.LoadAnimations(parameters.child("animations").child("crouchL"));
	crouchRAnimation.LoadAnimations(parameters.child("animations").child("crouchR"));
	WalkingFireLAnimation.LoadAnimations(parameters.child("animations").child("walkingLFire"));
	WalkingFireRAnimation.LoadAnimations(parameters.child("animations").child("walkingRFire"));
	IdleFireLAnimation.LoadAnimations(parameters.child("animations").child("idleLFire"));
	IdleFireRAnimation.LoadAnimations(parameters.child("animations").child("idleRFire"));
	JumpFireLAnimation.LoadAnimations(parameters.child("animations").child("jumpingLFire"));
	JumpFireRAnimation.LoadAnimations(parameters.child("animations").child("jumpingRFire"));
	crouchFireRAnimation.LoadAnimations(parameters.child("animations").child("crouchRFire"));
	crouchFireLAnimation.LoadAnimations(parameters.child("animations").child("crouchLFire"));

	currentAnimation = &idleRAnimation;

	// A�adir f�sica
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	if (!parameters.attribute("gravity").as_bool()) {
		pbody->body->SetGravityScale(0);
	}

	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(position.getX()), PIXEL_TO_METERS(position.getY()));
	pbody->body->SetTransform(bodyPos, 0);

	return true;
}

bool Player::Update(float dt) {
	CheckPlayerFire(dt);
	
	int cameraX = Engine::GetInstance().render.get()->camera.x;
	int cameraY = Engine::GetInstance().render.get()->camera.y;
	if (Engine::GetInstance().scene.get()->showMainMenu|| Engine::GetInstance().scene.get()->isLoading|| !isActive||!canMove) {
		return true; // Si estamos en el men�, no hacer nada m�s, ni dibujar al jugador
	}
	//Si se agota el tiempo...
	if (Engine::GetInstance().scene.get()->timeUp) {
		isDead = true;
	}
	//Actualize textures to be sure they are Drawn
	if (toggleTexture) {
		Engine::GetInstance().render.get()->DrawTexture(texturePlayer,(int)position.getX(),(int)position.getY(),&currentAnimation->GetCurrentFrame());
	}
	currentAnimation->Update();
	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	// Death handling
	if (isDead) {
		if (!deathSoundPlayed) {
			Engine::GetInstance().audio.get()->PlayFx(ohNoId, 0);
			Engine::GetInstance().audio.get()->StopMusic(0.2f);
			Engine::GetInstance().audio.get()->PlayFx(DeathId, 0);
			deathSoundPlayed = true;
		}
		//Drawn Death Texture
		currentAnimation = &deadAnimation;
		Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene->gameOver, -cameraX + 225, -cameraY);
		//Change collision 
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + 16, (int)position.getY() + 16, texW / 2, bodyType::STATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::PLAYER;
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
			if (Engine::GetInstance().entityManager->lives <= 0 || Engine::GetInstance().scene->timeUp) {
				// Volver al men�
				Engine::GetInstance().scene.get()->GameOver();
			}
			else {
				// Reiniciar estado del jugador
				Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
				pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
				pbody->listener = this;
				pbody->ctype = ColliderType::PLAYER;
				isDead = false;
				deathSoundPlayed = false;
				Engine::GetInstance().scene.get()->timeUp = false;
				// Reiniciar enemigos
				Engine::GetInstance().entityManager.get()->ResetEnemies();

				// Reiniciar posici�n del jugador
				if (Engine::GetInstance().scene.get()->level == 1 && Engine::GetInstance().scene->isFlaged) {
					SetPosition(Vector2D(3250, 430)); // Posici�n del bander�n
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				else if (Engine::GetInstance().scene.get()->level == 1) {
					SetPosition(Vector2D(30, 430)); // Inicio del nivel 1
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				else if (Engine::GetInstance().scene.get()->level == 2 && Engine::GetInstance().scene->isFlaged) {
					SetPosition(Vector2D(3300, 830)); // Posici�n del bander�n
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/Wolrd2Theme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				else if (Engine::GetInstance().scene.get()->level == 2) {
					SetPosition(Vector2D(200, 700)); // Inicio del nivel 2
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/World2Theme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				else if (Engine::GetInstance().scene.get()->level == 3 && Engine::GetInstance().scene->isFlaged) {
					SetPosition(Vector2D(3300, 830)); // Posici�n del bander�n
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/CastleTheme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				else if (Engine::GetInstance().scene.get()->level == 3) {
					SetPosition(Vector2D(100, 580)); // Inicio del nivel 3
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/CastleTheme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				Engine::GetInstance().audio.get()->PlayFx(hereWeGoAgain, 0);
			}
		}
		return true;
	}
	//crouching animation
	 if (iscrouching) {
		if (Engine::GetInstance().entityManager->isFirey == true)
		{
			currentAnimation = facingLeft ? &crouchFireLAnimation : &crouchFireRAnimation;
		}
		else {
			currentAnimation = facingLeft ? &crouchLAnimation : &crouchRAnimation;
		}
	}
	// Moving Animation
	else if (isMoving&&!isJumping) {
		if (Engine::GetInstance().entityManager->isFirey == true)
		{
			currentAnimation = facingLeft ? &WalkingFireLAnimation : &WalkingFireRAnimation;
		}
		else {
			currentAnimation = facingLeft ? &walkingLAnimation : &walkingRAnimation;
		}
	}
	// Jumping Animation
	else if (isJumping&&!iscrouching) {
		if (Engine::GetInstance().entityManager->isFirey == true)
		{
			currentAnimation = facingLeft ? &JumpFireLAnimation : &JumpFireRAnimation;
		}
		else {
			currentAnimation = facingLeft ? &jumpLAnimation : &jumpRAnimation;
		}
	}
	//Fire Power Up animation
	//idle animation
	else {
		if (Engine::GetInstance().entityManager->isFirey == true)
		{
			currentAnimation = facingLeft ? &IdleFireLAnimation : &IdleFireRAnimation;
		}
		else {
			currentAnimation = facingLeft ? &idleLAnimation : &idleRAnimation;
		}
	}

	//DEBUG FUNCTIONS
	// God Mode toggle
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && !godModeToggle) {
		Player::ToggleGodMode();
		godModeToggle = true;
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_UP) {
		godModeToggle = false;
	}
	if (godMode) {
		Player::pbody->body->SetGravityScale(0);
		Player::PlayerFlight(dt);
	}
	else {
		Player::pbody->body->SetGravityScale(1);
	}

	
		// PLAYER MOVEMENT CODE
		float movementSpeed = 3.5f;  // walking speed
		// Sprint
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && !isDead) {
			movementSpeed = 5.5f;
			isSprinting = true;
			frameDuration = 80.0f;
		}
		else {
			isSprinting = false; frameDuration = 130.0f;
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && !isDead) {
			iscrouching = true;
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_UP && !isDead) {
			iscrouching = false;
		}
		// Left
		if (!iscrouching && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isDead) {
			velocity.x = -movementSpeed;
			isMoving = true;
			facingLeft = true;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_UP) {
			isMoving = false;
		}
		// Right movement
		if (!iscrouching && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isDead) {
			velocity.x = movementSpeed;
			isMoving = true;
			facingLeft = false;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_UP) {
			isMoving = false;
		}
		// Jump
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && jumpcount < 2) {
			pbody->body->SetLinearVelocity(b2Vec2(0, 0));
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			animationTimer = 0.0f;
			isJumping = true;
			++jumpcount;
			Engine::GetInstance().audio.get()->PlayFx(jumpFxId);
			// Play random sound
			int randomSound = rand() % 8;
			Engine::GetInstance().audio.get()->PlayFx(jumpVoiceIds[randomSound]);
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN && Engine::GetInstance().entityManager->isFirey == true)
		{
			//CODIGO DE CREACION ATAQUE DE FUEGO MARIO
		}
		if (isJumping) velocity.y = pbody->body->GetLinearVelocity().y;
	
		// Apply the velocity to the player
		pbody->body->SetLinearVelocity(velocity);

		b2Transform pbodyPos = pbody->body->GetTransform();

		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
	
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texturePlayer);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype) {
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		isJumping = false;	//reset the jump flag when touching the ground
		jumpcount = 0;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::DEATH:
		LOG("Player hit a death zone, resetting position...");
		LoseLife();
		break;
	case ColliderType::WALL:
		LOG("Player collision with WALL...");
		jumpcount = 1;
		break;
	case ColliderType::ENEMY: {
		LOG("Collision with ENEMY");

		Enemy* enemy = dynamic_cast<Enemy*>(physB->listener);
		if (enemy != nullptr) {
			float playerBottom = this->position.getY() + this->texH; // Usamos todo el alto del jugador
			float playerVelocityY = pbody->body->GetLinearVelocity().y; // Velocidad en Y del jugador
			float enemyTop = enemy->GetPosition().getY(); // Parte superior del enemigo

			// Verificar si el jugador est� cayendo y si el jugador est� justo encima del enemigo
			if (playerBottom <= enemyTop + 5.0f && playerVelocityY > 0) { // Aseguramos que el jugador est� cayendo
				pbody->body->SetLinearVelocity(b2Vec2(0, -7)); // Rebote hacia arriba

				// L�gica espec�fica para el enemigo (ejemplo con Bowser)
				if (enemy->name == "bowser") {
					Engine::GetInstance().audio.get()->StopFx(); // Detener sonidos actuales
					Engine::GetInstance().audio.get()->PlayFx(BowserHit); // Sonido de impacto en Bowser
				}
				Engine::GetInstance().audio.get()->PlayFx(EnemyDeathSound, 0); // Sonido de muerte del enemigo

				enemy->hitCount++; // Incrementa el contador de impactos
				Engine::GetInstance().scene->UpdateEnemyHitCount(enemy->name, enemy->hitCount);
			}
			else {
				// Si el jugador no est� cayendo o no est� por encima del enemigo
				if (!isInvincible) {
					if (Engine::GetInstance().entityManager->isFirey) {
						// El jugador pierde el power-up de fuego
						Engine::GetInstance().entityManager->isFirey = false;
						isInvincible = true;
						invincibilityTimer = INVINCIBILITY_DURATION; // Temporizador de invencibilidad
						Engine::GetInstance().audio->PlayFx(PowerDown); // Sonido de perder poder
					}
					else {
						LoseLife(); // El jugador pierde una vida
					}
				}
			}
		}
		break;
	}
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype) {
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		jumpcount = 1;
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::DEATH:
		LOG("Position Reset");
		break;
	case ColliderType::WALL:
		LOG("Player hit a death zone, resetting position...");
		jumpcount = 1;
		break;
	case ColliderType::ENEMY:

		break;
	default:
		break;
	}
}
void Player::SetPosition(const Vector2D& newPosition)
{
	if (pbody == nullptr || pbody->body == nullptr) {
		LOG("Error: pbody o pbody->body no est�n inicializados.");
		return;

	}
	b2Vec2 newPos = b2Vec2(PIXEL_TO_METERS(newPosition.getX()), PIXEL_TO_METERS(newPosition.getY()));
	pbody->body->SetTransform(newPos, 0);
	// Actualizar la posici�n visual del jugador
	position = newPosition;
	LOG("SetPosition called: X: %f, Y: %f", position.getX(), position.getY());
}

Vector2D Player::GetPosition() {
	if (!pbody || !pbody->body) {
		std::cerr << "Error: pbody or pbody->body is null!" << std::endl;
		return Vector2D(0, 0); // Devuelve un valor por defecto o maneja el error
	}

	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}
void Player::ToggleGodMode() {
	godMode = !godMode;
	LOG(godMode ? "GOD MODE ACTIVATED" : "GOD MODE DEACTIVATED");
	b2Filter filter = pbody->body->GetFixtureList()->GetFilterData();
	filter.maskBits = godMode ? 0x0000 : 0xFFFF;
	pbody->body->GetFixtureList()->SetFilterData(filter);
	pbody->body->SetGravityScale(godMode ? 0 : 1);

	if (godMode) pbody->body->SetLinearVelocity(b2Vec2(0, 0));
}
void Player::PlayerFlight(float dt) {
	b2Vec2 velocity = pbody->body->GetLinearVelocity();
	velocity.SetZero();// Inicialize velocity at 0

	// Flight Moviment
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) velocity.y = -0.2f * dt;  // up
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) velocity.y = 0.2f * dt;   // down
	pbody->body->SetLinearVelocity(velocity);// Apply velocity to the body
}
void Player::LoseLife() {
	if (!isDead)
	{
		isDead = true;
		Engine::GetInstance().entityManager->lives--;
	}
	else {
		return;
	}
}
void Player::UpdateColliderSize(bool isFirey) {
	// Eliminar el cuerpo f�sico actual
	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	//// Ajustar el tama�o del collider basado en el estado
	int newRadius = isFirey ? texW / 1.25 : 32; // Cambiar tama�o del collider
	int yOffset = isFirey ? 0 : 0; // Ajustar el offset vertical para reducir hacia abajo

	//// Crear un nuevo collider con el tama�o actualizado
	pbody = Engine::GetInstance().physics.get()->CreateCircle(
		(int)position.getX(), (int)position.getY() + yOffset, newRadius, bodyType::DYNAMIC
	);
	//int newRadius = 32;  // Radio del c�rculo, ajusta seg�n tus necesidades

	//// Centrar la colisi�n con respecto al sprite
	//int centerX = (int)position.getX() + (texW / 2); // Centra horizontalmente
	//int centerY = (int)position.getY() + (texH/2); // Centra verticalmente (ajusta con yOffset si es necesario)

	//// Crear el c�rculo para colisi�n en el centro calculado
	//pbody = Engine::GetInstance().physics.get()->CreateCircle(
	//	centerX, centerY, newRadius, bodyType::DYNAMIC
	//);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	LOG("texW: %d", texW);
	// Log para depuraci�n
	//LOG("Collider actualizado: radio=%d, yOffset=%d", newRadius, yOffset);
}

void Player::StopMovement() {
	canMove = false;
}
void Player::ResumeMovement() {
	canMove = true;
}

void Player::CheckPlayerFire(float dt)
{
	static bool wasFirey = false;

	if (Engine::GetInstance().entityManager->isFirey != wasFirey) {
		wasFirey = Engine::GetInstance().entityManager->isFirey;
		UpdateColliderSize(wasFirey);
	}
	// L�gica de invencibilidad
	if (isInvincible) {
		invincibilityTimer -= dt;

		// Manejo del parpadeo
		blinkTimer += dt;
		if (blinkTimer >= 80.0f) { // Alternar visibilidad cada 0.1 segundos
			toggleTexture = !toggleTexture;
			blinkTimer = 0.0f;
		}

		// Fin de invencibilidad
		if (invincibilityTimer <= 0.0f) {
			isInvincible = false;
			toggleTexture = true; // Asegurar visibilidad al finalizar
		}
	}
	else {
		toggleTexture = true; // Siempre visible si no es invencible
	}

	if (Engine::GetInstance().entityManager->isFirey == true)
	{
		jumpForce = 6.8f;
	}
	else {
		jumpForce = 2.8f;
	}
}
