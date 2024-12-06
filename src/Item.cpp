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

	// Cargar configuraciones desde XML
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//initilize textures coin
	coinTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	flagTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_flag").as_string());
	flagpoleTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_flagpole").as_string());

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	flag.LoadAnimations(parameters.child("animations").child("idle_flag"));
	currentAnimation_flag = &flag;

	flagpole.LoadAnimations(parameters.child("animations").child("idle_flagpole"));
	currentAnimation_flagpole = &flagpole;


	lower_flag.LoadAnimations(parameters.child("animations").child("lower_flag"));
	lower_lower_flag.LoadAnimations(parameters.child("animations").child("lower_lower_flag"));


	// L08 TODO 4: Add a physics to an item - initialize the physics body
	Engine::GetInstance().textures.get()->GetSize(coinTexture, texW, texH);
	Engine::GetInstance().textures.get()->GetSize(flagpoleTexture, texW, texH);
	Engine::GetInstance().textures.get()->GetSize(flagTexture, texW, texH);
	
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
			currentAnimation_flag = &lower_flag;
		}
		if (currentAnimation_flag == &lower_flag && currentAnimation_flag->HasFinished()) {
			currentAnimation_flag = &lower_lower_flag; // Cambiar la animación a "lower_lower_flag"
		}
	}

	if (!isPicked) {
			Engine::GetInstance().render.get()->DrawTexture(coinTexture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			currentAnimation->Update();

			Engine::GetInstance().render.get()->DrawTexture(flagTexture, (int)position.getX(), (int)position.getY(), &currentAnimation_flag->GetCurrentFrame());
			currentAnimation_flag->Update();
	}
	else {
		Engine::GetInstance().render.get()->DrawTexture(flagTexture, (int)position.getX(), (int)position.getY(), &currentAnimation_flag->GetCurrentFrame());
		currentAnimation_flag->Update();
	}

	Engine::GetInstance().render.get()->DrawTexture(flagpoleTexture, (int)position.getX(), (int)position.getY(), &currentAnimation_flagpole->GetCurrentFrame());
	currentAnimation_flagpole->Update();
	return true;
}

bool Item::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(coinTexture);
	Engine::GetInstance().textures.get()->UnLoad(flagpoleTexture);
	return true;
}