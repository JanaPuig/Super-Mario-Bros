#pragma once

#include "Input.h"
#include "Render.h"

enum class EntityType
{
	PLAYER,
	ITEM,
	ENEMY,
	PROJECTILE,
	UNKNOWN
};

enum class EnemyType {
	WALKING,
	FLYING
};

inline bool operator==(EntityType lhs, EnemyType rhs) {
	return static_cast<int>(lhs) == static_cast<int>(rhs);
}

class PhysBody;

class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	void Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) {

	};

	virtual void OnCollisionEnd(PhysBody* physA, PhysBody* physB) {

	};

public:
	bool toBeDestroyed = false;
	std::string name;
	EntityType type;
	bool active = true;

	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	Vector2D position;       
	bool renderable = true;
};