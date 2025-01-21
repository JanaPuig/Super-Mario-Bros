#pragma once

#include "Module.h"
#include "Entity.h"
#include <list>

class EntityManager : public Module
{
public:

	EntityManager();

	virtual ~EntityManager();

	bool Awake();

	bool Start();

	bool Update(float dt);

	void RemoveAllItems();

	void RemoveAllEnemies();

	void ResetEnemies();

	void ResetItems();

	bool CleanUp();

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);
	
	Entity* GetEntityByName(std::string name);

	Entity* CreateEntity(EntityType type);

	bool isStarPower = false;
	bool StarMusicPlaying = false;

	float starPowerDuration = 15.0f;
	float puntuation = 0.0f;

	int lives = 3;
	int objects = 0;

	std::list<Entity*> entities;

};
