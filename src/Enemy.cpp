#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
Enemy::Enemy() : Entity(EntityType::ENEMY)
{

}

Enemy::~Enemy() {
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {


	//initilize textures
	texture_fly = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_fly").as_string());

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();


	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	dead.LoadAnimations(parameters.child("animations").child("dead"));
	currentAnimation = &idle; // Por defecto, el enemigo inicia con la animaci�n idle

	idlekoopa.LoadAnimations(parameters.child("animations").child("idlekoopa"));
	deadkoopa.LoadAnimations(parameters.child("animations").child("deadkoopa"));
	currentAnimation = &idlekoopa;

	leftBoundary = 500;
	rightBoundary = position.getX() + 500;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->listener =  this;
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(position.getX()), PIXEL_TO_METERS(position.getY()));
	pbody->body->SetTransform(bodyPos, 0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();

	Vector2D pos = GetPosition();

	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
	return true;
}

bool Enemy::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showMainMenu || Engine::GetInstance().scene.get()->showingTransition) {
		return true; // Si estamos en el men�, no hacer nada
	}

	if (isDead) {
		currentAnimation = &dead; // Cambiar a la animaci�n de muerte
		SDL_Rect frameRect = currentAnimation->GetCurrentFrame();

		Engine::GetInstance().render.get()->DrawTexture(
			texture,
			(int)position.getX(),
			(int)position.getY(),
			&frameRect
		);

		// Detener el movimiento del enemigo
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		return true; // Salir de la funci�n para evitar dibujar otra animaci�n
	}

	// Si no est� muerto, animaci�n de caminar
	frameTime += dt;
	if (frameTime >= frameDuration) {
		currentFrame = (currentFrame + 1) % totalFrames;
		frameTime = 0.0f;
	}
	SDL_Rect frameRect = { currentFrame * texW, 0, texW, texH };
	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &frameRect);

		//Animation walking
		
		//b2Vec2 velocity = pbody->body->GetLinearVelocity();
		//if (movingRight) {
		//	velocity.x = speed;  // Right movement
		//	if (position.getX() >= rightBoundary)
		//	{
		//		movingRight = false;
		//		movingLeft = true;
		//	}

		//}
		//if (movingLeft)
		//{
		//	velocity.x = -speed;  //  Left movement

		//	if (position.getX() <= leftBoundary)
		//	{
		//		movingRight = true;
		//		movingLeft = false;
		//	}
		//}
		//pbody->body->SetLinearVelocity(velocity);		
	
	if (type == EnemyType::FLYING)
	{
		currentAnimation = &deadkoopa; 
		frameRect = currentAnimation->GetCurrentFrame();

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &frameRect);

		pbody->body->SetLinearVelocity(b2Vec2(0, 0)); 
		return true;
	}


	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	//Vector2D pos = GetPosition();
	//Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	//pathfinding->ResetPath(tilePos);

	

	// Pathfinding testing inputs
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		Vector2D pos = GetPosition();
		Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
		pathfinding->ResetPath(tilePos);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_DOWN) {
		std::cout << "Tecla J presionada" << std::endl;

		pathfinding->PropagateBFS();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateBFS();
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
		pathfinding->PropagateDijkstra();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateDijkstra();
	}


	// Draw pathfinding 
	pathfinding->DrawPath();

	return true;
}

bool Enemy::CleanUp()
{

	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	//pos.setX(pos.getX() + texW / 2);
	//pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}