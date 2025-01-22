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
#include "tracy/Tracy.hpp"

Enemy::Enemy()
    : Entity(EntityType::ENEMY), hitCount(0), isAlive(true) {
}
Enemy::~Enemy() {
    delete pathfinding;
}

bool Enemy::Awake() {
    return true;
}

bool Enemy::Start() {
    // Inicialización de texturas
    if (parameters.attribute("name").as_string() == std::string("koopa") || parameters.attribute("name").as_string() == std::string("koopa2")) {
        textureKoopa = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_koopa").as_string());
        currentAnimation = &flyingkoopaLeft;
    }
    else if (parameters.attribute("name").as_string() == std::string("goomba") || parameters.attribute("name").as_string() == std::string("goomba2")) {
        textureGoomba = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
        currentAnimation = &idleGoomba;
    }
    else if (parameters.attribute("name").as_string() == std::string("bowser")) {
        textureBowser = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_bowser").as_string());
        currentAnimation = &idleBowserL;
        detectionRange = 1000.0f;
    }

    //asigna nombre al enemigo
    name = parameters.attribute("name").as_string();

    // Configuración inicial
    if (position.getX() == 0 && position.getY() == 0) {
        position.setX(parameters.attribute("x").as_float());
        position.setY(parameters.attribute("y").as_float());
    }
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();
    hitCount = parameters.attribute("hitcount").as_int();

    // Cargar animaciones
    idleGoomba.LoadAnimations(parameters.child("animations").child("idle"));
    deadGoomba.LoadAnimations(parameters.child("animations").child("dead"));
    flyingkoopaLeft.LoadAnimations(parameters.child("animations").child("idlekoopaL"));
    flyingkoopaRight.LoadAnimations(parameters.child("animations").child("idlekoopaR"));
    deadkoopa.LoadAnimations(parameters.child("animations").child("deadkoopa"));
    idleBowserL.LoadAnimations(parameters.child("animations").child("idleBowserL"));
    idleBowserR.LoadAnimations(parameters.child("animations").child("idleBowserR"));
    deadBowserL.LoadAnimations(parameters.child("animations").child("deadBowserL"));
    deadBowserR.LoadAnimations(parameters.child("animations").child("deadBowserR"));
    attackBowser.LoadAnimations(parameters.child("animations").child("attack"));
    walkingBowserL.LoadAnimations(parameters.child("animations").child("walkBowserL"));
    walkingBowserR.LoadAnimations(parameters.child("animations").child("walkBowserR"));

    //Cargar Fx
    BowserDeath = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/BowserDeath.wav");
    BowserAttack = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/BowserAttack.wav");
    BowserStep = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/BowserStep.wav");
    CastelThem = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/CastleTheme.wav");
    // Añadir física
    pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

    pbody->listener = this;
    //Assign collider type
    pbody->ctype = ColliderType::ENEMY;
    std::string enemyName = parameters.attribute("name").as_string();
    if (enemyName == "koopa") {
        pbody->body->SetGravityScale(1); // Sin gravedad para Koopa
    }
    else if (enemyName == "koopa2") {
        pbody->body->SetGravityScale(1); // Sin gravedad para Koopa
    }
    else if (enemyName == "goomba") {
        pbody->body->SetGravityScale(1); // Gravedad normal para Goomba
    }
    else if (enemyName == "goomba2") {
        pbody->body->SetGravityScale(1); // Gravedad normal para Goomba
    }
    else if (enemyName == "bowser") {
        pbody->body->SetGravityScale(5); // Gravedad normal para Bowser
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


    if (name == "koopa" || name == "koopa2") {
        newWidth = 32;
        newHeight = 32;
        pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + newWidth / 2, (int)position.getY() + newHeight / 2 + 20, newWidth / 2, bodyType::DYNAMIC);
    }
    else if (name == "bowser") {
        if (isAttacking) {
            newWidth = 84;
            newHeight = 55;
            pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + newWidth / 2, (int)position.getY() + newHeight / 2 + 60, newWidth, newHeight, bodyType::DYNAMIC);
        }
        else {
            newWidth = 102;
            newHeight = 117;
            pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
        }
    }
    // Asigna el nuevo listener y tipo de colisión
    pbody->listener = this;
    pbody->ctype = ColliderType::ENEMY;
}
bool Enemy::Update(float dt) {
    if (Engine::GetInstance().scene.get()->showMainMenu ||
        Engine::GetInstance().scene.get()->showingTransition ||
        Engine::GetInstance().scene.get()->isLoading || !visible || Engine::GetInstance().scene.get()->showWinScreen) {
        return true;
    }

    if (visible) {
        Vector2D playerPosition = Engine::GetInstance().scene.get()->GetPlayerPosition();
        Vector2D playerTile = Engine::GetInstance().map.get()->WorldToMap(playerPosition.getX(), playerPosition.getY());
        Vector2D enemyPosition = GetPosition();
        Vector2D enemyPositionTiles = Engine::GetInstance().map.get()->WorldToMap(enemyPosition.getX(), enemyPosition.getY());
        float distanceToPlayer = (playerPosition - enemyPosition).magnitude();
        float currentTime = SDL_GetTicks();
        frameTime += dt;

        if (isDying) {
            deathTimer += dt;
            b2Transform pbodyPos = pbody->body->GetTransform();
            position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
            position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

            if (parameters.attribute("name").as_string() == std::string("bowser")) {
                if (deathTimer >= 2000.0f) {
                    isEnemyDead = true;
                    toBeDestroyed = true;
                    Engine::GetInstance().scene.get()->endFinalBoss = true;
                    return true;
                }
            }
            else {
                if (deathTimer >= 1000.0f) {
                    isEnemyDead = true;
                    toBeDestroyed = true;
                    return true;
                }
            }

            if (currentAnimation) currentAnimation->Update();

            SDL_Rect frameRect = currentAnimation->GetCurrentFrame();
            Engine::GetInstance().render.get()->DrawTexture(textureGoomba, (int)position.getX(), (int)position.getY(), &frameRect);
            Engine::GetInstance().render.get()->DrawTexture(textureKoopa, (int)position.getX(), (int)position.getY() + 15, &frameRect);
            Engine::GetInstance().render.get()->DrawTexture(textureBowser, (int)position.getX(), (int)position.getY(), &frameRect);
            return true;
        }

        if (parameters.attribute("name").as_string() == std::string("goomba") || parameters.attribute("name").as_string() == std::string("goomba2")) {
            if (hitCount >= 1) {
                LOG("Goomba is Dead");
                Engine::GetInstance().entityManager->puntuation += 300.50;
                currentAnimation = &deadGoomba;
                isDying = true;
                return true;
            }
        }

        if (parameters.attribute("name").as_string() == std::string("koopa") || parameters.attribute("name").as_string() == std::string("koopa2")) {
            if (hitCount >= 1) {
                LOG("Koopa is Dead");
                Engine::GetInstance().entityManager->puntuation += 650.50;
                UpdateColliderSize();
                pbody->body->SetGravityScale(1);
                b2Filter filter = pbody->body->GetFixtureList()->GetFilterData();
                filter.maskBits = 0x0000;
                pbody->body->GetFixtureList()->SetFilterData(filter);
                currentAnimation = &deadkoopa;
                isDying = true;
                return true;
            }
            if (distanceToPlayer > detectionRange) {
                pbody->body->SetGravityScale(0);
            }
            else {
                pbody->body->SetGravityScale(1);
            }
            if (velocity.x < 0) {
                currentAnimation = &flyingkoopaLeft;
            }
            else if (velocity.x > 0) {
                currentAnimation = &flyingkoopaRight;
            }
        }
        if (parameters.attribute("name").as_string() == std::string("bowser")) {
            static bool isReturningToStart = false;

            // Si Bowser está en proceso de regresar a 6300
            if (position.getX() <= 4500 || isReturningToStart) {
                isReturningToStart = true;
                currentX = position.getX();

                if (currentX < targetX) {
                    currentX += ReturnSpeed;
                    if (currentX > targetX) {
                        currentX = targetX;
                    }
                    if (isAttacking) {
                        currentAnimation = &attackBowser;
                    }
                    else {
                        currentAnimation = &walkingBowserR;
                    }
                    currentAnimation->Update();
                    SDL_Rect frameRect = currentAnimation->GetCurrentFrame();
                    drawY = (int)position.getY() + (isAttacking ? 23 : 0);
                    Engine::GetInstance().render.get()->DrawTexture(textureBowser, (int)position.getX() - 35, drawY, &frameRect);
                }

                if (currentX >= targetX || playerPosition.getX() >= 5100) {
                    isReturningToStart = false; // Deja de regresar
                }
                // Actualizar posición de Bowser
                position.setX(currentX);
                pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(currentX), pbody->body->GetTransform().p.y), 0);
                pbody->body->SetLinearVelocity(b2Vec2(0, 0)); // Detener movimiento lineal

                return true; // Termina la actualización de Bowser
            }
            if (hitCount >= 3) {
                Engine::GetInstance().entityManager->puntuation += 100000;
                currentAnimation = velocity.x > 0 ? &deadBowserR : &deadBowserL;
                isDying = true;
                b2Filter filter = pbody->body->GetFixtureList()->GetFilterData();
                filter.maskBits = 0x0000;
                pbody->body->GetFixtureList()->SetFilterData(filter);
                deathTimer = 0.0f;
                Engine::GetInstance().audio->StopMusic(1.0F);
                Engine::GetInstance().audio.get()->PlayFx(BowserDeath);
                LOG("Bowser is dead");
                Engine::GetInstance().audio.get()->PlayFx(CastelThem);
                return true;
            }
            if (isAttacking) {
                attackTimer += dt;
                if (attackTimer >= attackDuration) {
                    isAttacking = false;
                    attackTimer = 0.0f;
                    UpdateColliderSize();
                    currentAnimation = velocity.x > 0 ? &idleBowserR : &idleBowserL;
                }
            }
            else {
                if (distanceToPlayer <= detectionRange && currentTime - lastAttackTime >= minAttackInterval) {
                    if (rand() % 200 < 1) {
                        LOG("Bowser ATTACKS!");
                        isAttacking = true;
                        UpdateColliderSize();
                        attackTimer = 0.0f;
                        lastAttackTime = currentTime;
                        Engine::GetInstance().audio.get()->PlayFx(BowserAttack);
                        minAttackInterval = 10000.0f + rand() % 10;
                        currentAnimation = &attackBowser;
                        currentAnimation->Reset();
                    }
                }
                if (!isAttacking) {
                    if (velocity.x < 0 && distanceToPlayer <= detectionRange) {
                        currentAnimation = &walkingBowserL;
                    }
                    else if (velocity.x > 0 && distanceToPlayer <= detectionRange) {
                        currentAnimation = &walkingBowserR;
                    }
                    else {
                        currentAnimation = velocity.x > 0 ? &idleBowserR : &idleBowserL;
                    }
                    if ((velocity.x != 0 || velocity.y != 0) && currentAnimation == &walkingBowserR || currentAnimation == &walkingBowserL) {

                        if (currentTime - lastStepTime > stepInterval) {
                            Engine::GetInstance().audio.get()->PlayFx(BowserStep, 0);
                            lastStepTime = currentTime;
                        }
                    }
                }
            }
            if (!isAttacking) {
                pbody->body->SetLinearVelocity(velocity);
            }
        }

        if (isEnemyDead) {
            toBeDestroyed = true;
            return true;
        }
        if (currentAnimation) {
            currentAnimation->Update();
            SDL_Rect frameRect = currentAnimation->GetCurrentFrame();
            Engine::GetInstance().render.get()->DrawTexture(textureGoomba, (int)position.getX(), (int)position.getY(), &frameRect);
            Engine::GetInstance().render.get()->DrawTexture(textureKoopa, (int)position.getX(), (int)position.getY() + 15, &frameRect);
            int drawY = (int)position.getY() + (isAttacking ? 23 : 0);
            Engine::GetInstance().render.get()->DrawTexture(textureBowser, (int)position.getX(), drawY, &frameRect);
        }
        if (pbody != NULL) {
            b2Transform pbodyPos = pbody->body->GetTransform();
            position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
            position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

            if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
                showPath = !showPath;
            }

            if (showPath) {
                pathfinding->DrawPath();
            }

            if (distanceToPlayer <= detectionRange) {
                ResetPath();

                int steps = 0;
                int maxSteps = 100;
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

                if (parameters.attribute("name").as_string() == std::string("koopa") || parameters.attribute("name").as_string() == std::string("koopa2")) {
                    velocity = b2Vec2(0, 0);
                    if (nextPos.getX() > enemyPositionTiles.getX()) {
                        velocity = b2Vec2(2.5, 0);
                    }
                    else if (nextPos.getX() < enemyPositionTiles.getX()) {
                        velocity = b2Vec2(-2.5, 0);
                    }
                    else if (nextPos.getY() > enemyPositionTiles.getY()) {
                        velocity = b2Vec2(0, 2.5);
                    }
                    else if (nextPos.getY() < enemyPositionTiles.getY()) {
                        velocity = b2Vec2(0, -2.5);
                    }
                }
                else if (parameters.attribute("name").as_string() == std::string("bowser") && isAttacking == true) {
                    if (nextPos.getX() > enemyPositionTiles.getX()) {
                        velocity = b2Vec2(5.5, 0);
                    }
                    else if (nextPos.getX() < enemyPositionTiles.getX()) {
                        velocity = b2Vec2(-5.5, 0);
                    }
                }
                else if (parameters.attribute("name").as_string() == std::string("bowser") && isAttacking == false) {
                    if (nextPos.getX() > enemyPositionTiles.getX()) {
                        velocity = b2Vec2(2.5, 0);
                    }
                    else if (nextPos.getX() < enemyPositionTiles.getX()) {
                        velocity = b2Vec2(-2.5, 0);
                    }
                }
                else {
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

            if ((parameters.attribute("name").as_string() == std::string("goomba") || parameters.attribute("name").as_string() == std::string("goomba2")) && Engine::GetInstance().scene->level == 1) {
                if (GetPosition().getY() > 490) {
                    isEnemyDead = true;
                    toBeDestroyed = true;
                }
            }
            else if ((parameters.attribute("name").as_string() == std::string("goomba") || parameters.attribute("name").as_string() == std::string("goomba2")) && Engine::GetInstance().scene->level == 3) {
                if (GetPosition().getY() > 790) {
                    isEnemyDead = true;
                    toBeDestroyed = true;
                }
            }

            if (pbody != NULL) {
                b2Transform pbodyPos = pbody->body->GetTransform();
                position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
                position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
            }
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
    Engine::GetInstance().textures.get()->UnLoad(textureBowser);
    return true;
}

void Enemy::Load_SetPosition(Vector2D pos) {
    pos.setX(pos.getX() + texW / 2);
    pos.setY(pos.getY() + texH / 2);
    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
    pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
    if (pbody == nullptr) {
        Start();
    }
    if (pbody != nullptr) {
        b2Vec2 bodyPos = pbody->body->GetTransform().p;
        Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
        return pos;
    }
}
void Enemy::ResetPath() {
    Vector2D pos = GetPosition();
    Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
    pathfinding->ResetPath(tilePos);
}
void Enemy::ResetPosition() {
    currentAnimation = (name == "koopa" || name == "koopa2") ? &flyingkoopaLeft :
        (name == "goomba" || name == "goomba2") ? &idleGoomba : nullptr;

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

    if (name == "koopa" || name == "koopa2" || name == "goomba" || name == "goomba2") {
        pbody->body->SetGravityScale(1);
    }
    else if (name == "bowser") {
        pbody->body->SetGravityScale(5);
    }
}

void Enemy::StopMovement() {
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        if (name == "koopa" || name == "koopa2") {
            pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - 2 + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::STATIC);
            pbody->listener = this;
            pbody->ctype = ColliderType::ENEMY;
        }
        else if (name == "bowser") {
            if (isAttacking) {
                newWidth = 84;
                newHeight = 55;
                pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + newWidth / 2, (int)position.getY() + newHeight / 2 + 30, newWidth, newHeight, bodyType::STATIC);
                pbody->listener = this;
                pbody->ctype = ColliderType::ENEMY;
            }
            else {
                pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::STATIC);
                pbody->listener = this;
                pbody->ctype = ColliderType::ENEMY;
            }
        }
        else if (name == "goomba" || name == "goomba2") {
            pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::STATIC);
            pbody->listener = this;
            pbody->ctype = ColliderType::ENEMY;
        }

    }
}
void Enemy::ResumeMovement() {
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        if (name == "koopa" || name == "koopa2") {
            pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - 2 + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
            pbody->body->SetGravityScale(1);
        }
        else if (name == "goomba" || name == "goomba2") {
            pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
            pbody->body->SetGravityScale(1);
        }
        else if (name == "bowser") {
            if (isAttacking) {
                newWidth = 84;
                newHeight = 55;
                pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + newWidth / 2, (int)position.getY() + newHeight / 2 + 30, newWidth, newHeight, bodyType::DYNAMIC);
            }
            else {
                pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
            }
            pbody->body->SetGravityScale(5); // Gravedad normal para Bowser
        }
        pbody->listener = this;
        pbody->ctype = ColliderType::ENEMY;
    }
}