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

	//L03: TODO 2: Initialize Pl
	// ayer parameters
	position = Vector2D(3, 8);

	return true;
}
bool Player::Start() {
	// Load FX and Textures
	for (int i = 0; i < 8; ++i) {
		std::string path = "Assets/Audio/Fx/MarioVoices/Jump_Random_" + std::to_string(i + 1) + ".wav";
		jumpVoiceIds[i] = Engine::GetInstance().audio.get()->LoadFx(path.c_str());
	}
	//Fx
	jumpFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Jump_Small.wav");
	StartId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/Start.wav");
	DeathId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Mario_Death.wav");
	ohNoId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/Death.wav");

	//Teextures
	walkingTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/mario_walking.png");
	idleTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/player1.png");
	jumpTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/mario_jumping.png");
	die = Engine::GetInstance().textures.get()->Load("Assets/Textures/die.png");
	gameOver = Engine::GetInstance().textures.get()->Load("Assets/Textures/gameOver.png");


	// Get walking texture size and calculate frame dimensions
	Engine::GetInstance().textures.get()->GetSize(walkingTexture, texW, texH);
	frameWidth = texW / 3;  // Dividimos el ancho entre 3 (ya que hay 3 cuadros de animación)
	frameHeight = texH;

	// Initialize player physics
	Engine::GetInstance().textures.get()->GetSize(idleTexture, texW, texH);
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	//Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	//Assign collider type
	pbody->ctype = ColliderType::PLAYER;
	return true;
}
bool Player::Update(float dt) {
	// L08 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	// ANIMATION CODES
	// Death handling
	if (isDead) {
		// Reproducir el sonido de muerte solo una vez
		if (!deathSoundPlayed) {
			Engine::GetInstance().audio.get()->PlayFx(ohNoId, 0);
			Engine::GetInstance().audio.get()->StopMusic(0.2f);  // Detener la música principal
			Engine::GetInstance().audio.get()->PlayFx(DeathId, 0);  // Reproducir el sonido de muerte
			deathSoundPlayed = true;  // Asegúrate de que no se vuelva a reproducir el sonido
		}

		// Mostrar la textura de muerte
		int cameraX = Engine::GetInstance().render.get()->camera.x;
		int cameraY = Engine::GetInstance().render.get()->camera.y;
		Engine::GetInstance().render.get()->DrawTexture(die, (int)position.getX(), (int)position.getY());
		Engine::GetInstance().render.get()->DrawTexture(gameOver, -cameraX + 225, -cameraY);
		// Permitir revivir al presionar la tecla
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
			isDead = false;
			deathSoundPlayed = false;  // Restablecer para futuras muertes
			SetPosition(Vector2D(3, 8.2));
			Engine::GetInstance().audio.get()->PlayFx(StartId, 0);  // Reproducir el sonido de reinicio
			Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/GroundTheme.wav");  // Reanudar la música
		}
		// Detener la ejecución adicional si el jugador está muerto
		return true;
	}

	// Moving Animation (solo si no está muerto)
	if (isMoving && !isJumping) {
		animationTimer += dt;  // Incrementa el temporizador con el delta time
		if (animationTimer >= frameDuration) {
			animationTimer = 0.0f;  // Reiniciar el temporizador
			currentFrame = (currentFrame + 1) % 3;  // Avanza al siguiente cuadro (3 en total)
		}
		SDL_Rect currentClip = { currentFrame * frameWidth, 0, frameWidth, frameHeight };  // Calcular el rectángulo del frame actual
		Engine::GetInstance().render.get()->DrawTexture(walkingTexture, (int)position.getX(), (int)position.getY(), &currentClip);  // Dibujar la animación de caminar
	}

	// Jumping Animation
	else if (isJumping == true) {
		animationTimer += dt;  // Incrementa el temporizador para la animación de salto
		SDL_Rect jumpClip;
		// Primero mostrar el primer frame por un tiempo
		if (animationTimer < jumpFrameDuration) {
			currentFrame = 0;  // Reiniciar a 0 para la animación de salto
			jumpClip = { 0, 0, texW, texH };  // Primer frame (mitad izquierda de la imagen)
		}
		else {
			currentFrame = 1;  // Solo hay dos frames, así que cambia al segundo frame
			jumpClip = { texW, 0, texW, texH };  // Segundo frame (mitad derecha de la imagen)
		}
		Engine::GetInstance().render.get()->DrawTexture(jumpTexture, (int)position.getX(), (int)position.getY(), &jumpClip);
	}

	// Idle Animation
	else {
		Engine::GetInstance().render.get()->DrawTexture(idleTexture, (int)position.getX(), (int)position.getY());  // El jugador está quieto, mostrar la textura estática
	}

	// God Mode toggle
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && !godModeToggle) {
		Player::ToggleGodMode();
		godModeToggle = true;  // Indica que la tecla F10 ha sido presionada
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_UP) {
		godModeToggle = false;  // Marca que la tecla F10 ha sido soltada
	}
	if (godMode) {
		Player::pbody->body->SetGravityScale(0);
		Player::PlayerFlight(dt);
	}
	else {
		Player::pbody->body->SetGravityScale(1);
	}

	// PLAYER MOVEMENT CODE
	float movementSpeed = 0.2f;  // Default walking speed

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
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
		pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);  // Apply an initial upward force
		animationTimer = 0.0f;
		isJumping = true;
		Engine::GetInstance().audio.get()->PlayFx(jumpFxId);
		// Play random sound
		int randomSound = rand() % 8;  // Select random number between 0 and 7
		Engine::GetInstance().audio.get()->PlayFx(jumpVoiceIds[randomSound]);
	}
	// If the player is jumping, use the current velocity produced by the jump
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
	//Unload all textures
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(idleTexture);
	Engine::GetInstance().textures.get()->UnLoad(walkingTexture);
	Engine::GetInstance().textures.get()->UnLoad(jumpTexture);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype) {
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		isJumping = false;	//reset the jump flag when touching the ground
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		// Aquí puedes incrementar un contador de puntos o eliminar el ítem
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
	pbody->body->SetTransform(newPos, 0);  // El segundo parámetro es la rotación

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
	b2Vec2 velocity = pbody->body->GetLinearVelocity(); //Actual Velocity
	velocity.SetZero();//Inicialize velocity at 0

	//Flight Moviment
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) velocity.y = -0.2f * dt;  //up
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) velocity.y = 0.2f * dt;   //down
	pbody->body->SetLinearVelocity(velocity);//Apply velocity to the body
}