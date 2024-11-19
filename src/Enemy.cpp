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

Enemy::Enemy() : Entity(EntityType::ENEMY) {}

Enemy::~Enemy() {
    delete pathfinding;
}

bool Enemy::Awake() {
    return true;
}

bool Enemy::Start() {
    if (parameters.attribute("name").as_string() == std::string("koopa")) {
        textureKoopaFlying = Engine::GetInstance().textures.get()->Load(parameters.attribute("koopa_flying").as_string());
        textureKoopaDead = Engine::GetInstance().textures.get()->Load(parameters.attribute("dead_koopa").as_string());
        currentAnimation = &idlekoopaLeft;
    }
    else if (parameters.attribute("name").as_string() == std::string("goomba")) {
        textureGoomba = Engine::GetInstance().textures.get()->Load(parameters.attribute("goombaSprites").as_string());
        currentAnimation = &idleGoomba;
    }

    position.setX(parameters.attribute("x").as_int());
    position.setY(parameters.attribute("y").as_int());
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    idleGoomba.LoadAnimations(parameters.child("animations").child("idle"));
    deadGoomba.LoadAnimations(parameters.child("animations").child("dead"));
    idlekoopaLeft.LoadAnimations(parameters.child("animations").child("idlekoopaL"));
    deadkoopa.LoadAnimations(parameters.child("animations").child("deadkoopa"));
    walkingKoopa.LoadAnimations(parameters.child("animations").child("walkingkoopa"));

    leftBoundary = position.getX() - 500;
    rightBoundary = position.getX() + 500;

    pbody = Engine::GetInstance().physics.get()->CreateCircle(
        (int)position.getX() + texH / 2,
        (int)position.getY() + texH / 2,
        texH / 2,
        bodyType::DYNAMIC
    );
    pbody->listener = this;
    pbody->ctype = ColliderType::ENEMY;

    if (!parameters.attribute("gravity").as_bool()) {
        pbody->body->SetGravityScale(0);
    }

    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(position.getX()), PIXEL_TO_METERS(position.getY()));
    pbody->body->SetTransform(bodyPos, 0);

    pathfinding = new Pathfinding();
    Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(position.getX(), position.getY());
    pathfinding->ResetPath(tilePos);

    return true;
}

bool Enemy::Update(float dt) {
    frameTime += dt;

    if (Engine::GetInstance().scene.get()->showMainMenu || Engine::GetInstance().scene.get()->showingTransition) {
        return true;
    }

    if (parameters.attribute("name").as_string() == std::string("koopa")) {
        currentAnimation = isDead ? &deadkoopa : &idlekoopaLeft;
    }
    else if (parameters.attribute("name").as_string() == std::string("goomba")) {
        currentAnimation = isDead ? &deadGoomba : &idleGoomba;
    }
    if (currentAnimation) {
        currentAnimation->Update();
    }

    SDL_Rect frameRect = currentAnimation->GetCurrentFrame();
    SDL_Texture* currentTexture = (parameters.attribute("name").as_string() == std::string("koopa"))
        ? (isDead ? textureKoopaDead : textureKoopaFlying)
        : textureGoomba;

    Engine::GetInstance().render.get()->DrawTexture(currentTexture, (int)position.getX(), (int)position.getY(), &frameRect);

    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
    position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
        Vector2D pos = GetPosition();
        Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
        pathfinding->ResetPath(tilePos);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_DOWN) {
        pathfinding->PropagateBFS();
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
        pathfinding->PropagateDijkstra();
    }

    pathfinding->DrawPath();

    return true;
}

bool Enemy::CleanUp() {
    return true;
}

void Enemy::SetPosition(Vector2D pos) {
    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
    pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
    b2Vec2 bodyPos = pbody->body->GetTransform().p;
    return Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
}

