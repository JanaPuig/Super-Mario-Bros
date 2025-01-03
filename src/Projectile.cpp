#include "Projectile.h"
#include "Engine.h"
#include "Textures.h"

Projectile::Projectile(Vector2D position, Vector2D direction) : Entity(EntityType::PROJECTILE), direction(direction) {
    // Cargar la textura del proyectil
    texture = Engine::GetInstance().textures.get()->Load("ruta/a/tu/textura/proyectil.png");
    this->position = position;
}

Projectile::~Projectile() {
    Engine::GetInstance().textures.get()->UnLoad(texture);
}

bool Projectile::Update(float dt) {
    // Mover el proyectil
    position += direction * speed * dt;

    // Aquí puedes agregar lógica para detectar colisiones o si el proyectil sale de la pantalla

    return true;
}