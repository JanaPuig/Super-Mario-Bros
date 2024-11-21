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

Enemy::Enemy() : Entity(EntityType::ENEMY) {}

Enemy::~Enemy() {
    delete pathfinding;
}

bool Enemy::Awake() {
    return true;
}

bool Enemy::Start() {
    // Inicializaci�n de texturas
    if (parameters.attribute("name").as_string() == std::string("koopa")) {
        textureGoomba = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_koopa").as_string());
        currentAnimation = &idlekoopaLeft;

        // Carga las animaciones de walkingkoopaLeft y deadkoopa

    }
    else if (parameters.attribute("name").as_string() == std::string("goomba")) {
        textureGoomba = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
        currentAnimation = &idleGoomba;
    }

    // Configuraci�n inicial
    position.setX(parameters.attribute("x").as_int());
    position.setY(parameters.attribute("y").as_int());
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    // Cargar animaciones
    idleGoomba.LoadAnimations(parameters.child("animations").child("idle"));
    deadGoomba.LoadAnimations(parameters.child("animations").child("dead"));
    idlekoopaLeft.LoadAnimations(parameters.child("animations").child("idlekoopaL"));
    idlekoopaRight.LoadAnimations(parameters.child("animations").child("idlekoopaR"));
    walkingKoopaRight.LoadAnimations(parameters.child("animations").child("walkingkoopaR"));
    walkingKoopaLeft.LoadAnimations(parameters.child("animations").child("walkingkoopaL"));
    deadkoopa.LoadAnimations(parameters.child("animations").child("deadkoopa"));

    // L�mite para caminar
    leftBoundary = position.getX() - 500;
    rightBoundary = position.getX() + 500;

    // A�adir f�sica
    pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

    pbody->listener = this;
    //Assign collider type
    pbody->ctype = ColliderType::ENEMY;

    if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);
 

    // Inicializar pathfinding
    pathfinding = new Pathfinding();

    ResetPath();

    return true;
}

bool Enemy::Update(float dt) {
    frameTime += dt;
    if (Engine::GetInstance().scene.get()->showMainMenu || Engine::GetInstance().scene.get()->showingTransition|| Engine::GetInstance().scene.get()->isLoading) {
        return true; // Si estamos en el men�, no hacer nada
    }
    // Actualizar animaci�n seg�n el estado
    if (parameters.attribute("name").as_string() == std::string("koopa")) {
        if (hitCount == 1)
        {
            currentAnimation = &walkingKoopaLeft;
        }
        else if (hitCount >=2) {
            currentAnimation = &deadkoopa;
            //Engine::GetInstance().entityManager->DestroyEntity();
        }
        else {
            currentAnimation = &idlekoopaLeft;
        }
    }
    if (parameters.attribute("name").as_string() == std::string("goomba")) {
        if (hitCount >=1) {
            currentAnimation = &deadGoomba;
            //Engine::GetInstance().entityManager->DestroyEntity();
        }
        else {
            currentAnimation = &idleGoomba;
        }
    }

    if (currentAnimation) {
        currentAnimation->Update();  // Actualizar animaci�n
    }

    SDL_Rect frameRect = currentAnimation->GetCurrentFrame();
    Engine::GetInstance().render.get()->DrawTexture(textureGoomba, (int)position.getX(), (int)position.getY(), &frameRect);

    // Actualizar posici�n f�sica
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

    return true;
}

bool Enemy::CleanUp() {
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
