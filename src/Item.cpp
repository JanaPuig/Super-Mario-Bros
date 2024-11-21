#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name = "item";
}

Item::~Item() {}

bool Item::Awake() {
	return true;
}

bool Item::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());

	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;
	// L08 TODO 4: Add a physics to an item - initialize the physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	
	//audio effect
	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Coin.wav");
	return true;
}


bool Item::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showMainMenu|| Engine::GetInstance().scene.get()->showingTransition|| Engine::GetInstance().scene.get()->isLoading) {
		return true; // Si estamos en el menú, no hacer nada más, ni dibujar al jugador
	}
	Player* player = Engine::GetInstance().scene.get()->GetPlayer(); 
	if (player != nullptr)
	{
		// Check the distance between the item and the player
		float distance = sqrt(pow(position.getX() - player->position.getX(), 2) + pow(position.getY() - player->position.getY(), 2));

		// If the distance is less than a threshold (e.g. 30 pixels), the player touches the item
		if (distance < 30.0f && !isPicked) {
			isPicked = true;
			LOG("Item picked up!");
			Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
			// update the score
		}
	}

	if (!isPicked) {
		if (currentAnimation != nullptr) {
			Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			currentAnimation->Update();
		}
		else {
			LOG("Warning: currentAnimation is null.");
		}
	}

	return true;
}

bool Item::CleanUp()
{
	return true;
}