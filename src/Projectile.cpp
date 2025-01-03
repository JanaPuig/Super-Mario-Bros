#include "Projectile.h"
#include "Engine.h"
#include "Textures.h"
#include "Log.h"

Projectile::Projectile(Vector2D position, Vector2D direction) : Entity(EntityType::PROJECTILE), direction(direction) {
    // Cargar la textura del proyectil
    texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Projectile.png");
    this->position = position;

    if (texture == nullptr) {
        LOG("Error loading projectile texture: Assets/Textures/Projectile.png");
    }
}

Projectile::~Projectile() {
    Engine::GetInstance().textures.get()->UnLoad(texture);
}

bool Projectile::Update(float dt) {
    // Mover el proyectil
    position = position + (direction * speed * dt);

    int cameraX = Engine::GetInstance().render.get()->camera.x;
    int cameraY = Engine::GetInstance().render.get()->camera.y;
    // Obtener las dimensiones de la pantalla


    // Verificar si el proyectil está fuera de los límites de la pantalla
    if (position.getX() < 0 || position.getX() > cameraX || position.getY() < 0 || position.getY() > cameraY) {
        toBeDestroyed = true; // Marcar el proyectil para ser destruido
    }
    SDL_Rect destRect = {
    (int)(position.getX() - cameraX),
    (int)(position.getY() - cameraY),
    32, 32
    }; // Ajusta el tamaño según tu textura
    Engine::GetInstance().render.get()->DrawTexture(texture, destRect.x, destRect.y, nullptr);
    return true;
}

void Projectile::Draw() {
    SDL_Rect destRect = { (int)position.getX(), (int)position.getY(), 32, 32 }; // Ajusta el tamaño según tu textura
    Engine::GetInstance().render.get()->DrawTexture(texture, destRect.x, destRect.y, nullptr);
}

void Projectile::SetDirection(Vector2D newDirection) {
    direction = newDirection;
}
