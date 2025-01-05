#pragma once

#include "Module.h"
#include "Entity.h"
#include <list>

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake();

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	void RemoveAllItems();

	void RemoveAllEnemies();

	void ResetEnemies();

	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(EntityType type);

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);
	
	Entity* GetEntityByName(std::string name);

public:
	bool isFirey = false;
	int lives = 3;
	int objects = 0;
	float puntuation = 00.0f;
	std::list<Entity*> entities;

};
