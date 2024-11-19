#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Enemy : public Entity {
public:
    Enemy();
    virtual ~Enemy();

    bool Awake();
    bool Start();
    bool Update(float dt);
    bool CleanUp();

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

    void SetPosition(Vector2D pos);
    Vector2D GetPosition();

    void UpdateColliderToFrame();

public:
    bool isDead = false;

private:
    SDL_Texture* textureGoomba = nullptr;
    SDL_Texture* textureKoopaFlying = nullptr;
    SDL_Texture* textureKoopaDead = nullptr;

    int texW = 0, texH = 0;
    pugi::xml_node parameters;

    Animation* currentAnimation = nullptr;

    Animation idleGoomba;
    Animation deadGoomba;
    Animation idlekoopaLeft;
    Animation walkingKoopa;
    Animation deadkoopa;

    PhysBody* pbody = nullptr;
    Pathfinding* pathfinding = nullptr;

    float frameTime = 0.0f;
    float leftBoundary = 0.0f;
    float rightBoundary = 0.0f;
};
