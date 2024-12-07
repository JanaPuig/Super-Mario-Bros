#pragma once
#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"

// Forward declaration
struct SDL_Texture;

class Player : public Entity
{
public:
    Player();
    virtual ~Player();

    // Lifecycle methods
    bool Awake();
    bool Start();
    bool Update(float dt);
    bool CleanUp();

    // Collision methods
    void OnCollision(PhysBody* physA, PhysBody* physB) override;
    void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

    void SetPosition(const Vector2D& newPosition);


    // Player-specific functionality
    Vector2D GetPosition() const { return position; }
    void ToggleGodMode();
    void PlayerFlight(float dt);

    // State management
    void SetActive(bool active) { isActive = active; }
    bool IsActive() const { return isActive; }

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

    Vector2D GetPosition();

private:

    SDL_Texture* texturePlayer;
    pugi::xml_node parameters;
    Animation* currentAnimation = nullptr;

    // Player parameters
    float speed = 5.0f;
    float jumpForce = 2.8f;
    const float jumpFrameDuration = 120.0f;

    // Animation parameters
    int texW = 0, texH = 0;
    int frameWidth = 0, frameHeight = 0;
    int currentFrame = 0;
    float animationTimer = 0.0f;
    float frameDuration = 130.0f;

    // Physics body
    PhysBody* pbody = nullptr;

    // Animations
    Animation idleRAnimation;
    Animation idleLAnimation;
    Animation jumpRAnimation;
    Animation jumpLAnimation;
    Animation walkingRAnimation;
    Animation walkingLAnimation;
    Animation deadAnimation;


    // Audio FX
    int pickCoinFxId= 0;
    int jumpFxId = 0;
    int jumpVoiceIds[8] = { 0 };
    int DeathId = 0;
    int hereWeGoAgain = 0;
    int ohNoId = 0;
    int EnemyDeathSound = 0;

    // Player state
    bool isMoving = false;
    bool isSprinting = false;
    bool isDead = false;
    bool isJumping = false;
    bool deathSoundPlayed = false;
    bool facingLeft = false;
    bool godMode = false;
    bool godModeToggle = false;
    int jumpcount = 0;

    // Internal state
    bool isActive = true;
};