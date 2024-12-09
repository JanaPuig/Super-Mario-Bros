#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "EntityManager.h"
#include "Physics.h"

Enemy::Enemy() : Entity(EntityType::ENEMY) {}

Enemy::~Enemy() {
    delete pathfinding;
}

bool Enemy::Awake() {
    return true;
}

bool Enemy::Start() {
    // Inicialización de texturas
    if (parameters.attribute("name").as_string() == std::string("koopa")) {
        textureKoopa = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_koopa").as_string());
        currentAnimation = &flyingkoopaLeft;
    }
    else if (parameters.attribute("name").as_string() == std::string("koopa2")) {
        textureKoopa = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_koopa").as_string());
        currentAnimation = &flyingkoopaLeft;
    }
    else if (parameters.attribute("name").as_string() == std::string("goomba")) {
        textureGoomba = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
        currentAnimation = &idleGoomba;
    }
    else if (parameters.attribute("name").as_string() == std::string("goomba2")) {
        textureGoomba = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
        currentAnimation = &idleGoomba;
    }

    //asigna nombre al enemigo
    name = parameters.attribute("name").as_string();

    // Configuración inicial
    position.setX(parameters.attribute("x").as_int());
    position.setY(parameters.attribute("y").as_int());
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();
    hitCount = parameters.attribute("hitcount").as_int();

    // Cargar animaciones
    idleGoomba.LoadAnimations(parameters.child("animations").child("idle"));
    deadGoomba.LoadAnimations(parameters.child("animations").child("dead"));
    flyingkoopaLeft.LoadAnimations(parameters.child("animations").child("idlekoopaL"));
    flyingkoopaRight.LoadAnimations(parameters.child("animations").child("idlekoopaR"));
    deadkoopa.LoadAnimations(parameters.child("animations").child("deadkoopa"));

    // Añadir física
    pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

    pbody->listener = this;
    //Assign collider type
    pbody->ctype = ColliderType::ENEMY;
    std::string enemyName = parameters.attribute("name").as_string();
    if (enemyName == "koopa") {
        pbody->body->SetGravityScale(0); // Sin gravedad para Koopa
    }
    else if (enemyName == "koopa2") {
        pbody->body->SetGravityScale(0); // Sin gravedad para Koopa
    }
    else if (enemyName == "goomba") {
        pbody->body->SetGravityScale(10); // Gravedad normal para Goomba
    }
    else if (enemyName == "goomba2") {
        pbody->body->SetGravityScale(10); // Gravedad normal para Goomba
    }
    // Inicializar pathfinding
    pathfinding = new Pathfinding();

    return true;
}
int Enemy::GetHitCount() 
{
   return hitCount;
}
void Enemy::UpdateColliderSize() {
    // Elimina el colisionador anterior
    Engine::GetInstance().physics.get()->DeletePhysBody(pbody);

    // Nuevo tamaño para el colisionador
    int newWidth = 32;
    int newHeight = 32;

    // Crea un nuevo colisionador de tipo círculo con el nuevo tamaño
    pbody = Engine::GetInstance().physics.get()->CreateCircle(
        (int)position.getX() + newWidth / 2,
        (int)position.getY() + newHeight / 2,
        newWidth / 2,
        bodyType::DYNAMIC
    );
    // Asigna el nuevo listener y tipo de colisión
    pbody->listener = this;
    pbody->ctype = ColliderType::ENEMY;
}
bool Enemy::Update(float dt) {
    if (Engine::GetInstance().scene.get()->showMainMenu ||
        Engine::GetInstance().scene.get()->showingTransition ||
        Engine::GetInstance().scene.get()->isLoading) {
        return true;
    }

    frameTime += dt;

    if (isDying) {
        deathTimer += dt;
        b2Transform pbodyPos = pbody->body->GetTransform();
        position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
        position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

        if (deathTimer >= 1000.0f) {
            isEnemyDead = true;
            toBeDestroyed = true;
            return false;
        }

        if (currentAnimation) currentAnimation->Update();

        SDL_Rect frameRect = currentAnimation->GetCurrentFrame();
        Engine::GetInstance().render.get()->DrawTexture(textureGoomba, (int)position.getX(), (int)position.getY(), &frameRect);
        Engine::GetInstance().render.get()->DrawTexture(textureKoopa, (int)position.getX() + 25, (int)position.getY() + 25, &frameRect);
        return true;
    }

    // Lógica de muerte
    if ((parameters.attribute("name").as_string() == std::string("koopa") || parameters.attribute("name").as_string() == std::string("koopa2")) && hitCount == 1) {
        UpdateColliderSize();
        pbody->body->SetGravityScale(1);
        currentAnimation = &deadkoopa;
        isDying = true;
        deathTimer = 0.0f;
        return true;
    }
    else if ((parameters.attribute("name").as_string() == std::string("goomba") || parameters.attribute("name").as_string() == std::string("goomba2")) && hitCount >= 1) {
        currentAnimation = &deadGoomba;
        isDying = true;
        deathTimer = 0.0f;
        return true;
    }

    if (isEnemyDead) {
        toBeDestroyed = true;
        Engine::GetInstance().scene.get()->SaveState();
        return false;
    }

    if (currentAnimation) currentAnimation->Update();

    SDL_Rect frameRect = currentAnimation->GetCurrentFrame();
    Engine::GetInstance().render.get()->DrawTexture(textureGoomba, (int)position.getX(), (int)position.getY(), &frameRect);
    Engine::GetInstance().render.get()->DrawTexture(textureKoopa, (int)position.getX(), (int)position.getY(), &frameRect);

    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
    position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

    // Mostrar u ocultar path al presionar F9
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
        showPath = !showPath;
    }

    // Dibujar el path si está habilitado
    if (showPath) {
        pathfinding->DrawPath();
    }

    // Detección del jugador
    Vector2D playerPosition = Engine::GetInstance().scene.get()->GetPlayerPosition();
    Vector2D playerTile = Engine::GetInstance().map.get()->WorldToMap(playerPosition.getX(), playerPosition.getY());
    Vector2D enemyPosition = GetPosition();
    Vector2D enemyPositionTiles = Engine::GetInstance().map.get()->WorldToMap(enemyPosition.getX(), enemyPosition.getY());
    float distanceToPlayer = (playerPosition - enemyPosition).magnitude();

    if (distanceToPlayer <= detectionRange) {
        ResetPath();

        int steps = 0;  // Número de pasos máximo del pathfinding
        int maxSteps = 100; // -> Ajustar según les parezca
        while (pathfinding->pathTiles.empty() && steps < maxSteps) {
            pathfinding->PropagateAStar(SQUARED);
            steps++;
        }

        int i = 0;
        for (const auto& pos : pathfinding->pathTiles) {
            if (i == pathfinding->pathTiles.size() - 2) {
                nextPos = pos;
            }
            i++;
        }

        // Modificar solo el movimiento en el eje Y para Koopas
        if (parameters.attribute("name").as_string() == std::string("koopa") || parameters.attribute("name").as_string() == std::string("koopa2")) {
            velocity = b2Vec2(0, 0); // Resetear la velocidad en X
            if (nextPos.getX() > enemyPositionTiles.getX()) {
                velocity = b2Vec2(2, 0);
            }
            else if (nextPos.getX() < enemyPositionTiles.getX()) {
                velocity = b2Vec2(-2, 0);
            }
            else if (nextPos.getY() > enemyPositionTiles.getY()) {
                velocity = b2Vec2(0, 2); // Movimiento solo en el eje Y
            }
            else if (nextPos.getY() < enemyPositionTiles.getY()) {
                velocity = b2Vec2(0, -2); // Movimiento solo en el eje Y
            }

            // Actualizar animación en base a dirección Y (solo para Koopas)
            if (velocity.y > 0) {
                currentAnimation = &flyingkoopaLeft; // O animación hacia abajo
            }
            else if (velocity.y < 0) {
                currentAnimation = &flyingkoopaRight; // O animación hacia arriba
            }

        }
        else {
            // Comportamiento normal para otros enemigos (no Koopa)
            if (nextPos.getX() > enemyPositionTiles.getX()) {
                velocity = b2Vec2(2, 0);
            }
            else if (nextPos.getX() < enemyPositionTiles.getX()) {
                velocity = b2Vec2(-2, 0);
            }
          
        }

        pbody->body->SetLinearVelocity(velocity);
    }
    else {
        pbody->body->SetLinearVelocity(b2Vec2(0, 0));
    }
    if (parameters.attribute("name").as_string() == std::string("goomba") || parameters.attribute("name").as_string() == std::string("goomba2")) {
        if (GetPosition().getY() > 490)
        {
            //currentAnimation = &deadGoomba;
            isEnemyDead = true;
            toBeDestroyed = true;
        }
    }
    return true;
}

bool Enemy::CleanUp() {
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        pbody = nullptr;
    }
    Engine::GetInstance().textures.get()->UnLoad(textureKoopa);
    Engine::GetInstance().textures.get()->UnLoad(textureGoomba);
    return true;
}

void Enemy::SetPosition(Vector2D pos) {
    pos.setX(pos.getX() + texW / 2);
    pos.setY(pos.getY() + texH / 2);
    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
    pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
 
    b2Vec2 bodyPos = pbody->body->GetTransform().p;
    Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
    return pos;
    
}
void Enemy::ResetPath() {
    Vector2D pos = GetPosition();
    Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
    pathfinding->ResetPath(tilePos);
}
void Enemy::ResetPosition() {
    // Restablece el estado de la animación
    if (parameters.attribute("name").as_string() == std::string("koopa") || parameters.attribute("name").as_string() == std::string("koopa2")) {
        currentAnimation = &flyingkoopaLeft;
    }
    else if (parameters.attribute("name").as_string() == std::string("goomba") || parameters.attribute("name").as_string() == std::string("goomba2")) {
        currentAnimation = &idleGoomba;
    }
    if (parameters.attribute("name").as_string() != std::string("goomba2"))
    {
        // Restablece la posición
        position.setX(parameters.attribute("x").as_int() + 32);
        position.setY(parameters.attribute("y").as_int() + 32);
    }
    else {
        // Restablece la posición
        position.setX(parameters.attribute("x").as_int() + 16);
        position.setY(parameters.attribute("y").as_int() + 32);
    }
    // Restablecer otras variables
    isDying = false;
    isEnemyDead = false;
    toBeDestroyed = false;
    hitCount = 0;
    if (pbody) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        pbody = nullptr;
    }
    pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
    pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.getX()), PIXEL_TO_METERS(position.getY())), 0);
    pbody->listener = this;
    pbody->ctype = ColliderType::ENEMY;
    //Gravedad de los enemigos
    std::string enemyName = parameters.attribute("name").as_string();
    if (enemyName == "koopa" || enemyName == "koopa2") {
        pbody->body->SetGravityScale(0);
    }
    else if (enemyName == "goomba" || enemyName == "goomba2") {
        pbody->body->SetGravityScale(1);
    }

}