#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"

class Projectile : public Entity {
public:
    Projectile(Vector2D position, Vector2D direction);
    virtual ~Projectile();

    bool Update(float dt) override;
    void SetDirection(Vector2D direction);

private:
    Vector2D direction;
    float speed = 500.0f; // Velocidad del proyectil
    SDL_Texture* texture; // Textura del proyectil
};