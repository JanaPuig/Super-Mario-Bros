#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}
Player::~Player() {
}
bool Player::Awake() {
	position = Vector2D(3, 8);
	return true;
}
bool Player::Start() {
	// Load FX and Textures
	for (int i = 0; i < 8; ++i) 
	{ std::string path = "Assets/Audio/Fx/MarioVoices/Jump_Random_" + std::to_string(i + 1) + ".wav";
	jumpVoiceIds[i] = Engine::GetInstance().audio.get()->LoadFx(path.c_str()); }

	jumpFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Jump_Small.wav");
	StartId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/Start.wav");
	DeathId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Mario_Death.wav");
	ohNoId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/Death.wav");

	//Teextures
	walkingTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/mario_walking.png");
	walkingLTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/mario_walkingL.png");
	idleTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/player1.png");
	idleLTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/playerL.png");
	jumpTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/mario_jumping.png");
	jumpLTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/mario_jumpingL.png");
	die = Engine::GetInstance().textures.get()->Load("Assets/Textures/die.png");
	gameOver = Engine::GetInstance().textures.get()->Load("Assets/Textures/gameOver.png");

	Engine::GetInstance().textures.get()->GetSize(walkingTexture, texW, texH);
	frameWidth = texW / 3; 
	frameHeight = texH;
	Engine::GetInstance().textures.get()->GetSize(idleTexture, texW, texH);

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	return true;
}
bool Player::Update(float dt) {
	// L08 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	// ANIMATION CODES
	// Death handling
	if (isDead) {
		if (!deathSoundPlayed) {
			Engine::GetInstance().audio.get()->PlayFx(ohNoId, 0);
			Engine::GetInstance().audio.get()->StopMusic(0.2f);  
			Engine::GetInstance().audio.get()->PlayFx(DeathId, 0); 
			deathSoundPlayed = true;
		}
		//Drawn Death Texture
		int cameraX = Engine::GetInstance().render.get()->camera.x;
		int cameraY = Engine::GetInstance().render.get()->camera.y;
		Engine::GetInstance().render.get()->DrawTexture(die, (int)position.getX(), (int)position.getY());
		Engine::GetInstance().render.get()->DrawTexture(gameOver, -cameraX + 225, -cameraY);
	
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
			isDead = false;
			deathSoundPlayed = false;  
			SetPosition(Vector2D(3, 8.3));
			Engine::GetInstance().audio.get()->PlayFx(StartId, 0); 
			Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav"); 
		}	
		return true;
	}
	// Moving Animation
	if (isMoving && !isJumping) {
		animationTimer += dt;  // Incrementa el temporizador con el delta time
		if (animationTimer >= frameDuration) {
			animationTimer = 0.0f;
			currentFrame = (currentFrame + 1) % 3;
		}
		SDL_Rect currentClip = { currentFrame * frameWidth, 0, frameWidth, frameHeight };
		Engine::GetInstance().render.get()->DrawTexture(facingLeft ? walkingLTexture : walkingTexture, (int)position.getX(), (int)position.getY(), &currentClip);
	}

	// Jumping Animation
	else if (isJumping == true) {
		animationTimer += dt;  // Incrementa el temporizador para la animación de salto
		SDL_Rect jumpClip;
		if (animationTimer < jumpFrameDuration) {
			currentFrame = 0;
			jumpClip = { 0, 0, texW, texH };
		}
		else {
			currentFrame = 1; 
			jumpClip = { texW, 0, texW, texH };
		}
		Engine::GetInstance().render.get()->DrawTexture(facingLeft ? jumpLTexture : jumpTexture, (int)position.getX(), (int)position.getY(), &jumpClip);
	}

	// Idle Animation
	else {
		Engine::GetInstance().render.get()->DrawTexture(facingLeft ? idleLTexture : idleTexture, (int)position.getX(), (int)position.getY());
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
	float movementSpeed = 0.2f;  // walking speed
	// Sprint
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && !isDead) { movementSpeed = 0.35f; isSprinting = true; frameDuration = 80.0f; }
	else { isSprinting = false; frameDuration = 130.0f;
	}

	// Left
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isDead) velocity.x = -movementSpeed * dt, isMoving = true, facingLeft = true;
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_UP) isMoving = false;

	// Right
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isDead) velocity.x = movementSpeed * dt, isMoving = true, facingLeft = false;
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_UP) isMoving = false;

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
	if (isJumping) velocity.y = pbody->body->GetLinearVelocity().y;

	// Apply the velocity to the player
	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(idleTexture);
	Engine::GetInstance().textures.get()->UnLoad(walkingTexture);
	Engine::GetInstance().textures.get()->UnLoad(jumpTexture);
	Engine::GetInstance().textures.get()->UnLoad(idleLTexture);
	Engine::GetInstance().textures.get()->UnLoad(walkingLTexture);
	Engine::GetInstance().textures.get()->UnLoad(jumpLTexture);
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
		isDead = true;
		break;
	default:
		break;
	}
}
void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype) {
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::DEATH:
		LOG("Position Reset");
		break;
	default:
		break;
	}
}

void Player::SetPosition(const Vector2D& newPosition)
{
	b2Vec2 newPos = b2Vec2(newPosition.getX(), newPosition.getY());
	pbody->body->SetTransform(newPos, 0);  
	// Actualizar la posición visual del jugador
	position = newPosition;
	LOG("SetPosition called: X: %f, Y: %f", position.getX(), position.getY());
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