#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include <vector>

// Forward declaration
struct SDL_Texture;

class Item : public Entity
{
public:
    Item();
    virtual ~Item();

    // Lifecycle methods
    bool Awake();
    bool Start();
    bool Update(float dt);
    bool CleanUp();

    // Item parameters
    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

    // Item state management
    void SetIsPicked(bool picked) { isPicked = picked; }
    void SavePickedStateToFile();
    int GetisPicked();
    void ResetPosition();
    void SetPosition(const Vector2D& newPosition);
    Vector2D GetPosition() const;

public:
    // Audio FX
    int pickCoinFxId = 0;
    int checkPointFxId = 0;
    int oneUpFxId = 0;
    int bigCoinFxId = 0;
    int powerUpFxId = -1;

    // Item type flags
    bool isPicked = false;    
    bool isCoin = false;     
    bool isOneUp = false;
    bool isBigCoin = false;   
    bool isPowerUp = false;    
    bool isFinishFlag = false;   
    bool appear = true;         

    std::vector<Item*> items;   // Collection of items

private:
    // Textures
    SDL_Texture* coinTexture = nullptr;
    SDL_Texture* bigCoinTexture = nullptr;
    SDL_Texture* OneUpTexture = nullptr;
    SDL_Texture* flagTexture = nullptr;
    SDL_Texture* flagpoleTexture = nullptr;
    SDL_Texture* powerUpTexture = nullptr;
    SDL_Texture* finishFlagTexture = nullptr;
    SDL_Texture* finishFlagpoleTexture = nullptr;

    const char* texturePath = nullptr;
    int texW = 0, texH = 0;

    // Animation parameters
    pugi::xml_node parameters;
    Animation* currentAnimation = &idle;
    Animation idle;

    Animation* currentAnimationFlag = &flag;
    Animation flag;
    Animation lowerFlag;
    Animation lowerLowerFlag;
    Animation bigCoin;
    Animation updateFinishFlag;

    Animation* currentAnimationFlagpole = &flagpole;
    Animation flagpole;

    Animation* currentAnimationFinishFlag = &finishFlag;
    Animation finishFlag;
    Animation* currentAnimationFinishFlagpole = &finishFlagpole;
    Animation finishFlagpole;
};