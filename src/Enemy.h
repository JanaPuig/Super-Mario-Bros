#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

    Enemy();
    virtual ~Enemy();

    bool Awake();

    bool Start();

    bool Update(float dt);

    bool CleanUp();

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

    void ResetPath();

    void UpdateColliderSize();

    int GetHitCount();

    void ResetPosition();

    void StopMovement();

    void ResumeMovement();

    Vector2D GetPosition();

    // Variables p�blicas relacionadas con texturas y animaciones
    SDL_Texture* textureGoomba;
    SDL_Texture* textureKoopa;
    SDL_Texture* textureBowser;
    const char* texturePath;
    int texW, texH;
    pugi::xml_node parameters;
    Animation* currentAnimation = nullptr;

    // Animaciones espec�ficas de Bowser
    Animation idleBowserL;
    Animation idleBowserR;
    Animation attackBowser;
    Animation deadBowserL;
    Animation deadBowserR;
    Animation walkingBowserL;
    Animation walkingBowserR;

    // Animaciones de otros enemigos
    Animation idleGoomba;
    Animation deadGoomba;
    Animation flyingkoopaLeft;
    Animation flyingkoopaRight;
    Animation deadkoopa;

    // Variables f�sicas y de pathfinding
    b2Vec2 velocity = b2Vec2(0, 0);
    Vector2D nextPos;
    Vector2D lastPlayerTile;
    PhysBody* pbody;
    Pathfinding* pathfinding;

    // Variables de control de comportamiento
    int hitCount = 0;
    int currentFrame = 0;
    int totalFrames = 3;
    int newWidth = 0;
    int newHeight = 0;

    //BowserSoundEffects
    int BowserStep = 1;
    int BowserDeath = 1;
    int BowserAttack = 1;
    int drawY = 0;
    int CastelThem = 1;
    b2Vec2 attackVelocity = b2Vec2(8.0f, 0.0f); //Bowser's Attack velocity

    bool isAttacking = false; 
    bool isEnemyDead = false;
    bool isDying = false;
    bool movingRight = true;
    bool movingLeft = false;
    bool showPath = false;
    bool isLookingRight = false;
    bool isAlive;

    // Par�metros relacionados con tiempo y rango
    float currentX=0;
    float deathTimer = 0.0f;
    float targetX = 6300.0f;
    float ReturnSpeed = 4.0f;
    float frameTime = 0;
    float frameDuration = 200.0f;
    float lastAttackTime = 0.0f;
    float minAttackInterval = 2000.0f;
    float attackTimer = 0.0f;
    float attackDuration = 6000.0f; 
    float detectionRange = 500.0f;
    float speed = 3.0f;
    float lastStepTime = 0.0f;
    float stepInterval = 600.0f; 

    bool visible = true;

};