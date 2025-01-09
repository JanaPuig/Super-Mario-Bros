#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "Item.h"
#include "Enemy.h"
#include "Projectile.h"
#include "tracy/Tracy.hpp"
EntityManager::EntityManager() : Module()
{
	name = "entitymanager";
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake()
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	for (const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Awake();
	}

	return ret;

}
bool EntityManager::Start() {

	bool ret = true;

	//Iterates over the entities and calls Start
	for (const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;

	for (const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->CleanUp();
	}

	entities.clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr;

	// Verifica que se crea correctamente una entidad
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::ENEMY:
		entity = new Enemy();
		break;
	case EntityType::PROJECTILE:

		break;
	default:
		break;
	}

	if (entity != nullptr) {
		entities.push_back(entity);
		LOG("Created entity of type %d at address %p", type, entity);
	}

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		if (*it == entity) {
			(*it)->CleanUp();
			delete* it; // Free the allocated memory
			entities.erase(it); // Remove the entity from the list
			break; // Exit the loop after removing the entity
		}
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if (entity != nullptr) entities.push_back(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;

	for (const auto entity : entities)
	{
		if (!entity->active) continue;

		ret = entity->Update(dt);

		float cameraX = -Engine::GetInstance().render.get()->camera.x;
		if (entity->position.getX() <= cameraX / 260 &&
			entity->position.getX() >= cameraX / 2 - 50) {
			ret = entity->Update(dt); 
		}
	}

	//// Eliminar entidades marcadas para ser destruidas
	//for (auto it = entities.begin(); it != entities.end(); )
	//{
	//	if ((*it)->toBeDestroyed) {
	//		(*it)->CleanUp();
	//		delete* it;
	//		it = entities.erase(it);
	//	}
	//	else {
	//		++it;
	//	}
	//}

	return ret;
}

void EntityManager::RemoveAllItems() {
	LOG("Starting removal of all items.");
	for (auto it = entities.begin(); it != entities.end(); ) {
		if ((*it)->type == EntityType::ITEM) {
			LOG("Destroying item entity at position: (%f, %f)", (*it)->position.getX(), (*it)->position.getY());
			(*it)->CleanUp();
			delete* it;
			it = entities.erase(it);
		}
		else {
			++it;
		}
	}
	LOG("All items removed.");
}

void EntityManager::RemoveAllEnemies() {

	for (auto it = entities.begin(); it != entities.end(); ) {
		if ((*it)->type == EntityType::ENEMY) {
			LOG("Destroying enemy entity at position: (%f, %f)", (*it)->position.getX(), (*it)->position.getY());
			(*it)->CleanUp();
			delete* it;
			it = entities.erase(it);
		}
		else {
			++it;
		}
	}

	LOG("All enemies removed.");
}

Entity* EntityManager::GetEntityByName(std::string name)
{
	for (const auto entity : entities)
	{
		if (entity->name == name) return entity;
	}
	return nullptr;
}

void EntityManager::ResetEnemies()
{
	for (Entity* entity : entities) {
		if (entity->type == EntityType::ENEMY) {
			Enemy* enemy = dynamic_cast<Enemy*>(entity);
			if (enemy != nullptr) {

				enemy->ResetPosition(); // Solo restablece la posición
			}
		}
	}
}

void EntityManager::ResetItems()
{
	for (Entity* entity : entities) {
		if (entity->type == EntityType::ITEM) {
			Item* item = static_cast<Item*>(entity);
			if (item != nullptr) {

				item->ResetPosition(); // Solo restablece la posición
			}
		}
	}
}