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
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Coin.png");
	
	// L08 TODO 4: Add a physics to an item - initialize the physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	
	//audio effect
	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Coin.wav");
	return true;
}


bool Item::Update(float dt)
{
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
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY());
	}

	return true;
}

bool Item::CleanUp()
{
	return true;
}