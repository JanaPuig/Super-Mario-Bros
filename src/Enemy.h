#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

// Forward declaration
struct SDL_Texture;

class Enemy : public Entity
{
public:
    Enemy();
    virtual ~Enemy();

    // Lifecycle methods
    bool Awake();
    bool Start();
    bool Update(float dt);
    bool CleanUp();

    // Position management
    void SetParameters(const pugi::xml_node& params, bool useXMLPosition = true) {
        parameters = params;
        if (useXMLPosition) {
            position.setX(parameters.attribute("x").as_float());
            position.setY(parameters.attribute("y").as_float());
        }
    }

    void SetPosition(const Vector2D& pos) {
        position = pos;
    }

    void Load_SetPosition(Vector2D pos);
    Vector2D GetPosition();
    void ResetPosition();
    void ResetPath();
    void UpdateColliderSize();
    int GetHitCount();

    // Movement control
    void StopMovement();
    void ResumeMovement();

public:
    // Textures and animations
    SDL_Texture* textureGoomba;
    SDL_Texture* textureKoopa;
    SDL_Texture* textureBowser;
    const char* texturePath;

    int texW = 0, texH = 0;
    pugi::xml_node parameters;
    Animation* currentAnimation = nullptr;

    // Bowser animations
    Animation idleBowserL;
    Animation idleBowserR;
    Animation attackBowser;
    Animation deadBowserL;
    Animation deadBowserR;
    Animation walkingBowserL;
    Animation walkingBowserR;

    // Other enemy animations
    Animation idleGoomba;
    Animation deadGoomba;
    Animation flyingkoopaLeft;
    Animation flyingkoopaRight;
    Animation deadkoopa;

    // Physics and pathfinding
    b2Vec2 velocity = b2Vec2(0, 0);
    Vector2D nextPos;
    Vector2D lastPlayerTile;
    PhysBody* pbody = nullptr;
    Pathfinding* pathfinding = nullptr;

    // Behavior control variables
    int hitCount = 0;
    int currentFrame = 0;
    int totalFrames = 3;

    // Bowser sound effects
    int BowserStep = 1;
    int BowserDeath = 1;
    int BowserAttack = 1;
    int drawY = 0;

    b2Vec2 attackVelocity = b2Vec2(8.0f, 0.0f); // Attack velocity of Bowser

    // State flags
    bool isAttacking = false; // Attack state
    bool isEnemyDead = false;
    bool isDying = false;
    bool movingRight = true;
    bool movingLeft = false;
    bool showPath = false;
    bool isLookingRight = false;
    bool isAlive = true; // Initialize isAlive

    // Time and range parameters
    float currentX = 0;
    float deathTimer = 0.0f;
    float targetX = 6300.0f;
    float ReturnSpeed = 4.0f;
    float frameTime = 0;
    float frameDuration = 200.0f;
    float lastAttackTime = 0.0f;
    float minAttackInterval = 2000.0f;
    float attackTimer = 0.0f;
    float attackDuration = 6000.0f; // Attack duration in milliseconds (6 seconds)
    float detectionRange = 500.0f;
    float speed = 3.0f;
    float lastStepTime = 0.0f;
    float stepInterval = 600.0f; // Seconds between step sounds

    bool visible = true;
};