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
	winAnimation.LoadAnimations(parameters.child("animations").child("Win"));
	WalkingStarLAnimation.LoadAnimations(parameters.child("animations").child("walkingLStar"));
	WalkingStarRAnimation.LoadAnimations(parameters.child("animations").child("walkingRStar"));
	IdleStarLAnimation.LoadAnimations(parameters.child("animations").child("idleLStar"));
	IdleStarRAnimation.LoadAnimations(parameters.child("animations").child("idleRStar"));
	JumpStarLAnimation.LoadAnimations(parameters.child("animations").child("jumpingLStar"));
	JumpStarRAnimation.LoadAnimations(parameters.child("animations").child("jumpingRStar"));
	crouchStarRAnimation.LoadAnimations(parameters.child("animations").child("crouchRStar"));
	crouchStarLAnimation.LoadAnimations(parameters.child("animations").child("crouchLStar"));
	DeadStarAnimation.LoadAnimations(parameters.child("animations").child("dieStar"));

	currentAnimation = &idleRAnimation;

	// Añadir física
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	if (!parameters.attribute("gravity").as_bool()) {
		pbody->body->SetGravityScale(1);
	}

	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(position.getX()), PIXEL_TO_METERS(position.getY()));
	pbody->body->SetTransform(bodyPos, 0);



	return true;
}

bool Player::Update(float dt) {
	ManageStarPower(dt);
	int cameraX = Engine::GetInstance().render.get()->camera.x;
	int cameraY = Engine::GetInstance().render.get()->camera.y;
	if (Engine::GetInstance().scene.get()->showMainMenu || Engine::GetInstance().scene.get()->isLoading || !isActive || !canMove || Engine::GetInstance().scene->showWinScreen == true) {
		return true; // Si estamos en el menú, no hacer nada más, ni dibujar al jugador
	}

	//Si se agota el tiempo...
	if (Engine::GetInstance().scene.get()->timeUp) {
		isDead = true;
	}
	//Actualize textures to be sure they are Drawn
	Engine::GetInstance().render.get()->DrawTexture(texturePlayer, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());

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
		if (Engine::GetInstance().entityManager->isStarPower) {
			currentAnimation = &DeadStarAnimation;
			Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene->gameOver, -cameraX + 225, -cameraY);
			//Change collision 
			Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
			pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + 16, (int)position.getY() + 16, texW / 2, bodyType::STATIC);
			pbody->listener = this;
			pbody->ctype = ColliderType::PLAYER;
		}
		else {
			currentAnimation = &deadAnimation;
			Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene->gameOver, -cameraX + 225, -cameraY);
			//Change collision 
			Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
			pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + 16, (int)position.getY() + 16, texW / 2, bodyType::STATIC);
			pbody->listener = this;
			pbody->ctype = ColliderType::PLAYER;
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
			if (Engine::GetInstance().entityManager->lives <= 0 || Engine::GetInstance().scene->timeUp) {
				// Volver al menú
				Engine::GetInstance().scene.get()->GameOver();
				Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
				pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
				pbody->listener = this;
				pbody->ctype = ColliderType::PLAYER;
				isDead = false;
				Engine::GetInstance().entityManager->isStarPower = false;
				Engine::GetInstance().entityManager.get()->ResetEnemies();
				Engine::GetInstance().entityManager.get()->ResetItems();

			}
			else {
				// Reiniciar estado del jugador
				Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
				pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
				pbody->listener = this;
				pbody->ctype = ColliderType::PLAYER;
				isDead = false;
				Engine::GetInstance().scene->bossFightActive = false;
				deathSoundPlayed = false;
				Engine::GetInstance().entityManager->isStarPower = false;
				Engine::GetInstance().scene.get()->timeUp = false;

				// Reiniciar enemigos

				// Reiniciar posición del jugador
				if (Engine::GetInstance().scene.get()->level == 1 && Engine::GetInstance().scene->isFlaged) {
					SetPosition(Vector2D(3250, 430)); // Posición del banderín
					Engine::GetInstance().entityManager.get()->ResetEnemies();
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				else if (Engine::GetInstance().scene.get()->level == 1) {
					SetPosition(Vector2D(30, 430)); // Inicio del nivel 1
					Engine::GetInstance().entityManager.get()->ResetEnemies();
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				else if (Engine::GetInstance().scene.get()->level == 2 && Engine::GetInstance().scene->isFlaged) {
					SetPosition(Vector2D(3300, 830)); // Posición del banderín
					Engine::GetInstance().entityManager.get()->ResetEnemies();
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/Wolrd2Theme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				else if (Engine::GetInstance().scene.get()->level == 2) {
					SetPosition(Vector2D(200, 700)); // Inicio del nivel 2
					Engine::GetInstance().entityManager.get()->ResetEnemies();
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/World2Theme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
				}
				else if (Engine::GetInstance().scene.get()->level == 3 && Engine::GetInstance().scene->isFlaged) {
					SetPosition(Vector2D(3500, 600)); // Posición del banderín
					Engine::GetInstance().entityManager.get()->ResetEnemies();
					Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/CastleTheme.wav");
					Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);

				}
				else if (Engine::GetInstance().scene.get()->level == 3) {
					SetPosition(Vector2D(100, 580)); // Inicio del nivel 3
					Engine::GetInstance().entityManager.get()->ResetEnemies();
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
		if (Engine::GetInstance().entityManager->isStarPower == true)
		{
			currentAnimation = facingLeft ? &crouchStarLAnimation : &crouchStarRAnimation;
		}
		else {
			currentAnimation = facingLeft ? &crouchLAnimation : &crouchRAnimation;
		}
	}
	// Moving Animation
	else if (isMoving && !isJumping) {
		if (Engine::GetInstance().entityManager->isStarPower == true)
		{
			currentAnimation = facingLeft ? &WalkingStarLAnimation : &WalkingStarRAnimation;
		}
		else {
			currentAnimation = facingLeft ? &walkingLAnimation : &walkingRAnimation;
		}
	}
	// Jumping Animation
	else if (isJumping && !iscrouching) {
		if (Engine::GetInstance().entityManager->isStarPower == true)
		{
			currentAnimation = facingLeft ? &JumpStarLAnimation : &JumpStarRAnimation;
		}
		else {
			currentAnimation = facingLeft ? &jumpLAnimation : &jumpRAnimation;
		}
	}

	//idle animation
	else {
		if (Engine::GetInstance().entityManager->isStarPower == true)
		{
			currentAnimation = facingLeft ? &IdleStarLAnimation : &IdleStarRAnimation;
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
		Player::PlayerFlight(dt);
	}


	// PLAYER MOVEMENT CODE
	if (Engine::GetInstance().entityManager->isStarPower)
	{
		movementSpeed = 5.0f;
	}
	else {
		movementSpeed = 3.5f;  // walking speed
	}

	// Sprint
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && !isDead) {
		if (Engine::GetInstance().entityManager->isStarPower)
		{
			movementSpeed = 7.0f;
		}
		else {
			movementSpeed = 5.5f;
		}
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
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN && Engine::GetInstance().entityManager->isStarPower == true)
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
	if (physA!= nullptr || physB!= nullptr) {
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
				float playerBottom = this->position.getY() + this->texH;
				float playerVelocityY = pbody->body->GetLinearVelocity().y; 
				float enemyTop = enemy->GetPosition().getY(); 

				if (Engine::GetInstance().entityManager->isStarPower == true)
				{
					//muerte enemigo
					b2Filter filter = enemy->pbody->body->GetFixtureList()->GetFilterData();
					filter.maskBits = 0x0000;
					enemy->pbody->body->GetFixtureList()->SetFilterData(filter);
					if (enemy->name == "bowser") {
						enemy->hitCount = enemy->hitCount + 3;
						Engine::GetInstance().scene->UpdateEnemyHitCount(enemy->name, enemy->hitCount);
					}
					else {
						if (enemy->name == "koopa" || enemy->name == "koopa2") {
							Engine::GetInstance().entityManager->puntuation += 650.50;
						}
						if (enemy->name == "goomba" || enemy->name == "goomba2") {
							Engine::GetInstance().entityManager->puntuation += 300.0;
						}
						enemy->isDying = true;
						b2Vec2 bounceVelocity(0.0f, -6.0f); // Rebote hacia arriba
						enemy->pbody->body->SetLinearVelocity(bounceVelocity);
					}

					Engine::GetInstance().audio.get()->PlayFx(EnemyDeathSound, 0);
				}
				// Verificar si el jugador está cayendo y si el jugador está justo encima del enemigo y cayendo
				else if (playerBottom <= enemyTop + 5.0f && Engine::GetInstance().entityManager->isStarPower == false) {
					pbody->body->SetLinearVelocity(b2Vec2(0, -7)); // Rebote hacia arriba

					if (enemy->name == "bowser" && enemy->isAttacking == false) {
						Engine::GetInstance().audio.get()->PlayFx(BowserHit);
						Engine::GetInstance().audio.get()->PlayFx(EnemyDeathSound, 0);
						enemy->hitCount++;
						Engine::GetInstance().scene->UpdateEnemyHitCount(enemy->name, enemy->hitCount);
					}
					else if (enemy->name == "bowser" && enemy->isAttacking == true)
					{
						Engine::GetInstance().audio.get()->PlayFx(EnemyDeathSound, 0);
					}
					else {
						Engine::GetInstance().audio.get()->PlayFx(EnemyDeathSound, 0);
						enemy->hitCount++;
						Engine::GetInstance().scene->UpdateEnemyHitCount(enemy->name, enemy->hitCount);
					}
				}
				else {
					LoseLife();
				}
			}
			break;
		}
		default:
			break;
		}
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
		LOG("Error: pbody o pbody->body no están inicializados.");
		return;
	}
	b2Vec2 newPos = b2Vec2(PIXEL_TO_METERS(newPosition.getX()), PIXEL_TO_METERS(newPosition.getY()));
	pbody->body->SetTransform(newPos, 0);
	// Actualizar la posición visual del jugador
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
	return;
}
void Player::StopMovement() {
	canMove = false;
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texW / 2, bodyType::STATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::PLAYER;
	}
}
void Player::ResumeMovement() {
	canMove = true;
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX()+ texH / 2, (int)position.getY() + texH / 2, texW / 2, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::PLAYER;
		if (godMode)
		{
			b2Filter filter = pbody->body->GetFixtureList()->GetFilterData();
			filter.maskBits = godMode ? 0x0000 : 0xFFFF;
			pbody->body->GetFixtureList()->SetFilterData(filter);
			pbody->body->SetGravityScale(0);
		}
		else {
			pbody->body->SetGravityScale(1);
		}
	}
}

void Player::ManageStarPower(float dt) {
	if (Engine::GetInstance().entityManager->isStarPower) {
		// Reproducir música de Star Power
		if (Engine::GetInstance().entityManager->StarMusicPlaying == false) {
			Engine::GetInstance().audio->StopMusic();
			Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/StarPower.wav");
			Engine::GetInstance().entityManager->StarMusicPlaying = true;
		}
		if (Engine::GetInstance().scene->showPauseMenu == true) {
			Engine::GetInstance().audio->PauseMusic();
		}
		else {
			Engine::GetInstance().entityManager->starPowerDuration += dt;
		}

		if (Engine::GetInstance().entityManager->starPowerDuration >= 10000.0f) {
			Engine::GetInstance().audio->PlayFx(PowerDown); // Sonido de perder poder
			Engine::GetInstance().entityManager->starPowerDuration = 0;
			Engine::GetInstance().entityManager->isStarPower = false; // Desactivar el poder de estrella
			Engine::GetInstance().entityManager->StarMusicPlaying = false;
			Engine::GetInstance().audio->StopMusic();
			if (Engine::GetInstance().scene->level == 1) {
				Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/GroundTheme.wav");
			}
			else if (Engine::GetInstance().scene->level == 2) {
				Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/World2Theme.wav");
			}
			else if (Engine::GetInstance().scene->level == 3) {
				Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/CastleTheme.wav");
			}
		}
	}
}
