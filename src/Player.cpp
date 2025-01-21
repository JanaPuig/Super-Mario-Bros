#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Enemy.h"
#include "EntityManager.h"
#include "tracy/Tracy.hpp"

Player::Player() : Entity(EntityType::PLAYER) {
    name = "Player";
}

Player::~Player() {}

bool Player::Awake() {
    return true;
}

bool Player::Start() {
    // Load FX
    for (int i = 0; i < 8; ++i) {
        std::string path = "Assets/Audio/Fx/MarioVoices/Jump_Random_" + std::to_string(i + 1) + ".wav";
        jumpVoiceIds[i] = Engine::GetInstance().audio.get()->LoadFx(path.c_str());
    }
    jumpFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Jump_Small.wav");
    hereWeGoAgain = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/HereWeGoAgain.wav");
    DeathId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Mario_Death.wav");
    ohNoId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/MarioVoices/Death.wav");
    EnemyDeathSound = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Stomp.wav");
    BowserHit = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/BowserHit.wav");
    PowerDown = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/PowerDown.wav");

    // Load Player Texture
    texturePlayer = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_player").as_string());

    // Player Configuration
    position.setX(parameters.attribute("x").as_float());
    position.setY(parameters.attribute("y").as_float());
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    // Load Animations
    idleRAnimation.LoadAnimations(parameters.child("animations").child("idle"));
    idleLAnimation.LoadAnimations(parameters.child("animations").child("idleL"));
    walkingRAnimation.LoadAnimations(parameters.child("animations").child("walking"));
    walkingLAnimation.LoadAnimations(parameters.child("animations").child("walkingL"));
    jumpRAnimation.LoadAnimations(parameters.child("animations").child("jumping"));
    jumpLAnimation.LoadAnimations(parameters.child("animations").child("jumpingL"));
    deadAnimation.LoadAnimations(parameters.child("animations").child("die"));
    crouchLAnimation.LoadAnimations(parameters.child("animations").child("crouchL"));
    crouchRAnimation.LoadAnimations(parameters.child("animations").child("crouchR"));
    winAnimation.LoadAnimations(parameters.child("animations").child("Win"));
    WalkingStarLAnimation.LoadAnimations(parameters.child("animations").child("walkingLStar"));
    WalkingStarRAnimation.LoadAnimations(parameters.child("animations").child("walkingRStar"));
    IdleStarLAnimation.LoadAnimations(parameters.child("animations").child("idleLStar"));
    IdleStarRAnimation.LoadAnimations(parameters.child("animations").child("idleRStar"));
    JumpStarLAnimation.LoadAnimations(parameters.child("animations").child("jumpingLStar"));
    JumpStarRAnimation.LoadAnimations(parameters.child("animations").child("jumpingRStar"));
    crouchStarRAnimation.LoadAnimations(parameters.child("animations").child("crouchRStar"));
    crouchStarLAnimation.LoadAnimations(parameters.child("animations").child("crouchLStar"));
    DeadStarAnimation.LoadAnimations(parameters.child("animations").child("dieStar"));

    currentAnimation = &idleRAnimation;

    // Add physics
    pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
    pbody->listener = this;
    pbody->ctype = ColliderType::PLAYER;

    if (!parameters.attribute("gravity").as_bool()) {
        pbody->body->SetGravityScale(1);
    }

    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(position.getX()), PIXEL_TO_METERS(position.getY()));
    pbody->body->SetTransform(bodyPos, 0);

    return true;
}

bool Player::Update(float dt) {
    if (Engine::GetInstance().scene.get()->showMainMenu || Engine::GetInstance().scene.get()->isLoading || !isActive || !canMove || Engine::GetInstance().scene->showWinScreen) {
        return true;
    }
    if (Engine::GetInstance().scene.get()->timeUp) {
        isDead = true;
    }

    ManageStarPower(dt);
    int cameraX = Engine::GetInstance().render.get()->camera.x;
    int cameraY = Engine::GetInstance().render.get()->camera.y;

    // Actualize textures to be sure they are Drawn
    Engine::GetInstance().render.get()->DrawTexture(texturePlayer, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
    currentAnimation->Update();
    b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

    // Death handling
    if (isDead) {
        if (!deathSoundPlayed) {
            Engine::GetInstance().audio.get()->PlayFx(ohNoId, 0);
            Engine::GetInstance().audio.get()->StopMusic(0.2f);
            Engine::GetInstance().audio.get()->PlayFx(DeathId, 0);
            deathSoundPlayed = true;
        }

        // Draw Death Texture
        currentAnimation = Engine::GetInstance().entityManager->isStarPower ? &DeadStarAnimation : &deadAnimation;
        Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene->gameOver, -cameraX + 225, -cameraY);

        // Change collision 
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + 16, (int)position.getY() + 16, texW / 2, bodyType::STATIC);
        pbody->listener = this;
        pbody->ctype = ColliderType::PLAYER;

        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
            if (Engine::GetInstance().entityManager->lives <= 0 || Engine::GetInstance().scene->timeUp) {
                // Volver al menú
                Engine::GetInstance().scene.get()->GameOver();
            }
            else {
                // Reiniciar estado del jugador
                Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
                pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
                pbody->listener = this;
                pbody->ctype = ColliderType::PLAYER;
                isDead = false;
                Engine::GetInstance().entityManager->isStarPower = false;
                Engine::GetInstance().entityManager.get()->ResetEnemies();
                Engine::GetInstance().entityManager.get()->ResetItems();

                // Reiniciar posición del jugador
                Vector2D newPosition;
                switch (Engine::GetInstance().scene.get()->level) {
                case 1:
                    newPosition = Engine::GetInstance().scene->isFlaged ? Vector2D(3250, 430) : Vector2D(30, 430);
                    break;
                case 2:
                    newPosition = Engine::GetInstance().scene->isFlaged ? Vector2D(3300, 830) : Vector2D(200, 700);
                    break;
                case 3:
                    newPosition = Engine::GetInstance().scene->isFlaged ? Vector2D(3500, 600) : Vector2D(100, 580);
                    break;
                }
                SetPosition(newPosition);
                Engine::GetInstance().audio.get()->PlayMusic(Engine::GetInstance().scene->level == 1 ? "Assets/Audio/Music/GroundTheme.wav" :
                    Engine::GetInstance().scene->level == 2 ? "Assets/Audio/Music/World2Theme.wav" : "Assets/Audio/Music/CastleTheme.wav");
                Mix_VolumeMusic(Engine::GetInstance().scene.get()->sdlVolume);
                Engine::GetInstance().audio.get()->PlayFx(hereWeGoAgain, 0);
            }
        }
        return true;
    }

    // Crouching animation
    if (iscrouching) {
        currentAnimation = Engine::GetInstance().entityManager->isStarPower ? (facingLeft ? &crouchStarLAnimation : &crouchStarRAnimation) : (facingLeft ? &crouchLAnimation : &crouchRAnimation);
    }
    // Moving Animation
    else if (isMoving && !isJumping) {
        currentAnimation = Engine::GetInstance().entityManager->isStarPower ? (facingLeft ? &WalkingStarLAnimation : &WalkingStarRAnimation) : (facingLeft ? &walkingLAnimation : &walkingRAnimation);
    }
    // Jumping Animation
    else if (isJumping && !iscrouching) {
        currentAnimation = Engine::GetInstance().entityManager->isStarPower ? (facingLeft ? &JumpStarLAnimation : &JumpStarRAnimation) : (facingLeft ? &jumpLAnimation : &jumpRAnimation);
    }
    // Idle animation
    else {
        currentAnimation = Engine::GetInstance().entityManager->isStarPower ? (facingLeft ? &IdleStarLAnimation : &IdleStarRAnimation) : (facingLeft ? &idleLAnimation : &idleRAnimation);
    }

    // DEBUG FUNCTIONS
    // God Mode toggle
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && !godModeToggle) {
        Player::ToggleGodMode();
        godModeToggle = true;
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_UP) {
        godModeToggle = false;
    }
    if (godMode) {
        Player::PlayerFlight(dt);
    }

    // PLAYER MOVEMENT CODE
    movementSpeed = Engine::GetInstance().entityManager->isStarPower ? 5.0f : 3.5f;  // walking speed

    // Sprint
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && !isDead) {
        movementSpeed = Engine::GetInstance().entityManager->isStarPower ? 7.0f : 5.5f;
        isSprinting = true;
        frameDuration = 80.0f;
    }
    else {
        isSprinting = false;
        frameDuration = 130.0f;
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && !isDead) {
        iscrouching = true;
    }
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_UP && !isDead) {
        iscrouching = false;
    }

    // Left movement
    if (!iscrouching && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isDead) {
        velocity.x = -movementSpeed;
        isMoving = true;
        facingLeft = true;
    }
    else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_UP) {
        isMoving = false;
    }

    // Right movement
    if (!iscrouching && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isDead) {
        velocity.x = movementSpeed;
        isMoving = true;
        facingLeft = false;
    }
    else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_UP) {
        isMoving = false;
    }

    // Jump
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && jumpcount < 2) {
        pbody->body->SetLinearVelocity(b2Vec2(0, 0));
        pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
        animationTimer = 0.0f;
        isJumping = true;
        ++jumpcount;
        Engine::GetInstance().audio.get()->PlayFx(jumpFxId);
        // Play random sound
        int randomSound = rand() % 8;
        Engine::GetInstance().audio.get()->PlayFx(jumpVoiceIds[randomSound]);
    }

    if (isJumping) {
        velocity.y = pbody->body->GetLinearVelocity().y;
    }

    // Apply the velocity to the player
    pbody->body->SetLinearVelocity(velocity);

    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
    position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

    return true;
}

bool Player::CleanUp() {
    LOG("Cleanup player");
    Engine::GetInstance().textures.get()->UnLoad(texturePlayer);
    return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
    if (physA != nullptr || physB != nullptr) {
        switch (physB->ctype) {
        case ColliderType::PLATFORM:
            LOG("Collision PLATFORM");
            isJumping = false; // reset the jump flag when touching the ground
            jumpcount = 0;
            break;
        case ColliderType::ITEM:
            LOG("Collision ITEM");
            break;
        case ColliderType::DEATH:
            LOG("Player hit a death zone, resetting position...");
            LoseLife();
            break;
        case ColliderType::WALL:
            LOG("Player collision with WALL...");
            jumpcount = 1;
            break;
        case ColliderType::ENEMY: {
            LOG("Collision with ENEMY");
            Enemy* enemy = dynamic_cast<Enemy*>(physB->listener);
            if (enemy != nullptr) {
                float playerBottom = this->position.getY() + this->texH;
                float enemyTop = enemy->GetPosition().getY();

                if (Engine::GetInstance().entityManager->isStarPower) {
                    // Handle enemy death
                    b2Filter filter = enemy->pbody->body->GetFixtureList()->GetFilterData();
                    filter.maskBits = 0x0000;
                    enemy->pbody->body->GetFixtureList()->SetFilterData(filter);
                    if (enemy->name == "bowser") {
                        enemy->hitCount += 3;
                        Engine::GetInstance().scene->UpdateEnemyHitCount(enemy->name, enemy->hitCount);
                    }
                    else {
                        if (enemy->name == "koopa" || enemy->name == "koopa2") {
                            Engine::GetInstance().entityManager->puntuation += 650.50;
                        }
                        if (enemy->name == "goomba" || enemy->name == "goomba2") {
                            Engine::GetInstance().entityManager->puntuation += 300.0;
                        }
                        enemy->isDying = true;
                        b2Vec2 bounceVelocity(0.0f, -6.0f); // Bounce up
                        enemy->pbody->body->SetLinearVelocity(bounceVelocity);
                    }
                    Engine::GetInstance().audio.get()->PlayFx(EnemyDeathSound, 0);
                }
                else if (playerBottom <= enemyTop + 5.0f) {
                    pbody->body->SetLinearVelocity(b2Vec2(0, -7)); // Bounce up
                    if (enemy->name == "bowser" && !enemy->isAttacking) {
                        Engine::GetInstance().audio.get()->PlayFx(BowserHit);
                    }
                    Engine::GetInstance().audio.get()->PlayFx(EnemyDeathSound, 0);
                    enemy->hitCount++;
                    Engine::GetInstance().scene->UpdateEnemyHitCount(enemy->name, enemy->hitCount);
                }
                else {
                    LoseLife();
                }
            }
            break;
        }
        default:
            break;
        }
    }
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::PLATFORM:
        LOG("End Collision PLATFORM");
        jumpcount = 1;
        break;
    case ColliderType::ITEM:
        LOG("End Collision ITEM");
        break;
    case ColliderType::DEATH:
        LOG("Position Reset");
        break;
    case ColliderType::WALL:
        LOG("Player hit a death zone, resetting position...");
        jumpcount = 1;
        break;
    case ColliderType::ENEMY:
        break;
    default:
        break;
    }
}

void Player::SetPosition(const Vector2D& newPosition) {
    if (pbody == nullptr || pbody->body == nullptr) {
        LOG("Error: pbody or pbody->body are not initialized.");
        return;
    }
    b2Vec2 newPos = b2Vec2(PIXEL_TO_METERS(newPosition.getX()), PIXEL_TO_METERS(newPosition.getY()));
    pbody->body->SetTransform(newPos, 0);
    position = newPosition;
    LOG("SetPosition called: X: %f, Y: %f", position.getX(), position.getY());
}

Vector2D Player::GetPosition() {
    if (!pbody || !pbody->body) {
        std::cerr << "Error: pbody or pbody->body is null!" << std::endl;
        return Vector2D(0, 0); // Return a default value or handle the error
    }

    b2Vec2 bodyPos = pbody->body->GetTransform().p;
    return Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
}

void Player::ToggleGodMode() {
    godMode = !godMode;
    LOG(godMode ? "GOD MODE ACTIVATED" : "GOD MODE DEACTIVATED");
    b2Filter filter = pbody->body->GetFixtureList()->GetFilterData();
    filter.maskBits = godMode ? 0x0000 : 0xFFFF;
    pbody->body->GetFixtureList()->SetFilterData(filter);
    pbody->body->SetGravityScale(godMode ? 0 : 1);

    if (godMode) pbody->body->SetLinearVelocity(b2Vec2(0, 0));
}

void Player::PlayerFlight(float dt) {
    b2Vec2 velocity = pbody->body->GetLinearVelocity();
    velocity.SetZero(); // Initialize velocity at 0

    // Flight Movement
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) velocity.y = -0.2f * dt;  // up
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) velocity.y = 0.2f * dt;   // down
    pbody->body->SetLinearVelocity(velocity); // Apply velocity to the body
}

void Player::LoseLife() {
    if (!isDead) {
        isDead = true;
        Engine::GetInstance().entityManager->lives--;
    }
}

void Player::StopMovement() {
    canMove = false;
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texW / 2, bodyType::STATIC);
        pbody->listener = this;
        pbody->ctype = ColliderType::PLAYER;
    }
}

void Player::ResumeMovement() {
    canMove = true;
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texW / 2, bodyType::DYNAMIC);
        pbody->listener = this;
        pbody->ctype = ColliderType::PLAYER;
        if (godMode) {
            b2Filter filter = pbody->body->GetFixtureList()->GetFilterData();
            filter.maskBits = godMode ? 0x0000 : 0xFFFF;
            pbody->body->GetFixtureList()->SetFilterData(filter);
            pbody->body->SetGravityScale(0);
        }
        else {
            pbody->body->SetGravityScale(1);
        }
    }
}

void Player::ManageStarPower(float dt) {
    if (Engine::GetInstance().entityManager->isStarPower) {
        // Play Star Power music
        if (!Engine::GetInstance().entityManager->StarMusicPlaying) {
            Engine::GetInstance().audio->StopMusic();
            Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/StarPower.wav");
            Engine::GetInstance().entityManager->StarMusicPlaying = true;
        }
        if (Engine::GetInstance().scene->showPauseMenu) {
            Engine::GetInstance().audio->PauseMusic();
        }
        else {
            Engine::GetInstance().entityManager->starPowerDuration += dt;
        }

        if (Engine::GetInstance().entityManager->starPowerDuration >= 10000.0f) {
            Engine::GetInstance().audio->PlayFx(PowerDown); // Sound of losing power
            Engine::GetInstance().entityManager->starPowerDuration = 0;
            Engine::GetInstance().entityManager->isStarPower = false; // Disable star power
            Engine::GetInstance().entityManager->StarMusicPlaying = false;
            Engine::GetInstance().audio->StopMusic();
            Engine::GetInstance().audio->PlayMusic(Engine::GetInstance().scene->level == 1 ? "Assets/Audio/Music/GroundTheme.wav" :
                Engine::GetInstance().scene->level == 2 ? "Assets/Audio/Music/World2Theme.wav" :
                "Assets/Audio/Music/CastleTheme.wav");
        }
    }
}