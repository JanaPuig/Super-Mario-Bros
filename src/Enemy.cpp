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
        currentAnimation = &idlekoopaLeft;
        // Carga las animaciones de walkingkoopaLeft y deadkoopa

    }
    else if (parameters.attribute("name").as_string() == std::string("goomba")) {
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
    idlekoopaLeft.LoadAnimations(parameters.child("animations").child("idlekoopaL"));
    idlekoopaRight.LoadAnimations(parameters.child("animations").child("idlekoopaR"));
    walkingKoopaRight.LoadAnimations(parameters.child("animations").child("walkingkoopaR"));
    walkingKoopaLeft.LoadAnimations(parameters.child("animations").child("walkingkoopaL"));
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
    else if (enemyName == "goomba") {
        pbody->body->SetGravityScale(1); // Gravedad normal para Goomba
    }
    // Inicializar pathfinding
    pathfinding = new Pathfinding();
    ResetPath();

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
    if (Engine::GetInstance().scene.get()->showMainMenu || Engine::GetInstance().scene.get()->showingTransition || Engine::GetInstance().scene.get()->isLoading) {
        return true; // Si estamos en el menú, no hacer nada
    }
    frameTime += dt;
    if (isDying) {
        deathTimer += dt;

        b2Transform pbodyPos = pbody->body->GetTransform();
        position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
        position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

       if (deathTimer >= 1000.0f) { // Esperar 1 segundo
            isEnemyDead = true;   // Marcar como muerto después del tiempo
            toBeDestroyed = true; // Marcar para eliminación
            return false;         // Detener ejecución
        }

        // Mantén la animación de muerte activa
        if (currentAnimation) {
            currentAnimation->Update();
        }

        // Dibujar la animación de muerte
        SDL_Rect frameRect = currentAnimation->GetCurrentFrame();
        Engine::GetInstance().render.get()->DrawTexture(textureGoomba, (int)position.getX(), (int)position.getY(), &frameRect);
        Engine::GetInstance().render.get()->DrawTexture(textureKoopa, (int)position.getX()+25, (int)position.getY()+25, &frameRect);
        return true; // Continúa ejecutándose mientras muere
    }
   
    // Actualizar animación según el estado
    if (parameters.attribute("name").as_string() == std::string("koopa")) {
        if (hitCount == 1) 
        {
            UpdateColliderSize();
            b2Transform pbodyPos = pbody->body->GetTransform();
            position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
            position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
            pbody->body->SetGravityScale(1);

            currentAnimation = &deadkoopa;

            isDying = true; // Inicia el proceso de muerte
            deathTimer = 0.0f; // Reinicia el temporizador
            return true; // Detener el resto de la lógica de actualización
        }
        else {
            currentAnimation = &idlekoopaLeft;
        }
    }
    else if (parameters.attribute("name").as_string() == std::string("goomba")) {
        if (hitCount >= 1) {
            currentAnimation = &deadGoomba;
            isDying = true; // Inicia el proceso de muerte
            deathTimer = 0.0f; // Reinicia el temporizador
            return true; // Detener el resto de la lógica de actualización
        }
        else {
            currentAnimation = &idleGoomba; 
        }
    }
    if (isEnemyDead) {
        toBeDestroyed = true;
        Engine::GetInstance().scene.get()->SaveState();
        return false;
    }
    if (currentAnimation) {
        currentAnimation->Update();  // Actualizar animación
    }
     
    SDL_Rect frameRect = currentAnimation->GetCurrentFrame();
    Engine::GetInstance().render.get()->DrawTexture(textureGoomba, (int)position.getX(), (int)position.getY(), &frameRect);
    Engine::GetInstance().render.get()->DrawTexture(textureKoopa, (int)position.getX(), (int)position.getY(), &frameRect);

    // Actualizar posición física
    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
    position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);


    //Pathfinding testing inputs
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
            Vector2D pos = GetPosition();
            Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
            pathfinding->ResetPath(tilePos);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_DOWN) {
        pathfinding->PropagateBFS();
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_REPEAT &&
        Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
        pathfinding->PropagateBFS();
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
        pathfinding->PropagateDijkstra();
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_REPEAT &&
        Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
        pathfinding->PropagateDijkstra();
    }
    // L13: TODO 3:	Add the key inputs to propagate the A* algorithm with different heuristics (Manhattan, Euclidean, Squared)

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
        pathfinding->PropagateAStar(MANHATTAN);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_REPEAT &&
        Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
        pathfinding->PropagateAStar(MANHATTAN);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
        pathfinding->PropagateAStar(EUCLIDEAN);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_REPEAT &&
        Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
        pathfinding->PropagateAStar(EUCLIDEAN);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {
        pathfinding->PropagateAStar(SQUARED);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_REPEAT &&
        Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
        pathfinding->PropagateAStar(SQUARED);
    }
    // Dibujar pathfinding
    pathfinding->DrawPath();

    Vector2D pos = GetPosition();

    pbody->body->SetLinearVelocity(MoveEnemy(pos, speed));

    return true;
}

b2Vec2 Enemy::MoveEnemy(Vector2D enemyPosition, float speed)
{
    b2Vec2 movement = { 0,0 };

    if (!pathfinding->pathTiles.empty())
    {
        //Obtener el siguiente punto del camino
        Vector2D nextTileWorld = Engine::GetInstance().map->MapToWorldCenter(pathfinding->pathTiles.back().getX(), pathfinding->pathTiles.back().getY());

        //Calcular dirección hacia el siguiente punto
        Vector2D direction = nextTileWorld - enemyPosition;
        float distance = direction.magnitude();

        //Mover el enemigo en la dirección determinada
        Vector2D determinate = direction * speed;

        if (distance < 6) {
            pathfinding->pathTiles.pop_back();
        }
        else {
            movement.x = direction.normalized().getX() * speed;
            movement.y = direction.normalized().getY() * speed;
        }

    }

    return movement;
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
