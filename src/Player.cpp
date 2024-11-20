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
	GoombaDeathSound = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Stomp.wav");
	//Load Player Texture
	texturePlayer = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_player").as_string());

	//Player Configuration 
	position.setX(parameters.attribute("x").as_float());
	position.setY(parameters.attribute("y").as_float());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load Animations
	idleTexture.LoadAnimations(parameters.child("animations").child("idle"));
	idleLTexture.LoadAnimations(parameters.child("animations").child("idleL"));
	walkingTexture.LoadAnimations(parameters.child("animations").child("walking"));
	walkingLTexture.LoadAnimations(parameters.child("animations").child("walkingL"));
	jumpTexture.LoadAnimations(parameters.child("animations").child("jumping"));
	jumpLTexture.LoadAnimations(parameters.child("animations").child("jumpingL"));
	deadTexture.LoadAnimations(parameters.child("animations").child("die"));

	currentAnimation = &idleTexture;


	gameOver = Engine::GetInstance().textures.get()->Load("Assets/Textures/gameOver.png");
	
	// Añadir física
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

	if (Engine::GetInstance().scene.get()->showMainMenu) {
		return true; // Si estamos en el menú, no hacer nada más, ni dibujar al jugador
	}
	if (!isActive) return true;

	if (pbody == nullptr) {
		LOG("Error: pbody no inicializado.");
		return true;

	}
	//Actualize textures to be sure they are Drawn
	Engine::GetInstance().render.get()->DrawTexture(texturePlayer, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

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
		currentAnimation = &deadTexture;
		Engine::GetInstance().render.get()->DrawTexture(gameOver, -cameraX + 225, -cameraY);

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
			isDead = false;
			deathSoundPlayed = false;

			if (Engine::GetInstance().scene.get()->level == 1) {
				SetPosition(Vector2D(3, 8.65));
			}
			else if (Engine::GetInstance().scene.get()->level == 2) {
				SetPosition(Vector2D(3, 14.45));
			}
			Engine::GetInstance().audio.get()->PlayFx(hereWeGoAgain, 0);
			Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav");
		}
		return true;
	}
	// Moving Animation
	if (isMoving && !isJumping) {
		if (facingLeft)
		{
			currentAnimation = &walkingLTexture;
		}
		else {
			currentAnimation = &walkingTexture;
		}
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
		if (facingLeft) {
			currentAnimation = &jumpLTexture;
		}
		else {
			currentAnimation = &jumpTexture;
		}
	}

	// Idle Animation
	else {
		if (facingLeft) {
			currentAnimation = &idleLTexture;
		}
		else {
			currentAnimation = &idleTexture;
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
	float movementSpeed = 0.2f;  // walking speed
	// Sprint
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && !isDead) { movementSpeed = 0.35f; isSprinting = true; frameDuration = 80.0f; }
	else {
		isSprinting = false; frameDuration = 130.0f;
	}

	// Left
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isDead)
	{
		velocity.x = -movementSpeed * dt;
		isMoving = true;
		facingLeft = true;
	}
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
		isDead = true;
		break;
	case ColliderType::WALL:
		LOG("Player hit a death zone, resetting position...");
		jumpcount = 1;
		break;
	case ColliderType::ENEMY: {
		LOG("Collision with ENEMY");
		Enemy* enemy = dynamic_cast<Enemy*>(physB->listener);
		float playerBottom = this->position.getY() + this->texH / 2;
		float enemyTop = enemy->GetPosition().getY();

		if (enemy != nullptr && playerBottom < enemyTop) {
			pbody->body->SetLinearVelocity(b2Vec2(0, -7)); // Rebote ligero
			Engine::GetInstance().audio.get()->PlayFx(GoombaDeathSound, 0);
			enemy->hitCount++; // Incrementa el contador de impactos
		}
		else {
			isDead = true; // Si el jugador no está encima, muere
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