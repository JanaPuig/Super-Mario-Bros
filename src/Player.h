#pragma once
#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"

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

    // Player-specific functionality
    Vector2D GetPosition() const { return position; }
    void SetPosition(const Vector2D& newPosition);
    void ToggleGodMode();
    void PlayerFlight(float dt);

    // State management
    void SetActive(bool active) { isActive = active; }
    bool IsActive() const { return isActive; }

private:
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

    // Textures
    SDL_Texture* idleTexture = nullptr;
    SDL_Texture* idleLTexture = nullptr;
    SDL_Texture* jumpTexture = nullptr;
    SDL_Texture* jumpLTexture = nullptr;
    SDL_Texture* walkingTexture = nullptr;
    SDL_Texture* walkingLTexture = nullptr;
    SDL_Texture* die = nullptr;
    SDL_Texture* gameOver = nullptr;

    // Audio FX
    int jumpFxId = 0;
    int jumpVoiceIds[8] = { 0 };
    int DeathId = 0;
    int StartId = 0;
    int ohNoId = 0;

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
