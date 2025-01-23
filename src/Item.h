#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include <vector> 

struct SDL_Texture;

class Item : public Entity
{
public:

    Item();
    virtual ~Item();

    bool Awake();

    bool Start();

    bool Update(float dt);

    bool CleanUp();

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }
   
    int GetisPicked();

    void SetIsPicked(bool picked) { isPicked = picked; }

    void SavePickedStateToFile();

    void ResetPosition();

    void SetPosition(const Vector2D& newPosition);

    Vector2D GetPosition() const;
    // Flags
    bool isCoin = false;    
    bool isOneUp = false;   
    bool isBigCoin = false;
    bool isPowerUp = false; 
    bool isFinishFlag = false; 

    // State
    bool apear = true;       

    // Audio Effects
    int pickCoinFxId = 0;
    int CheckPoint = 0;
    int oneUpFxId = 0;
    int BigCoinFxId = 0;
    int PowerUpFxId = -1;
    int isPicked = 0;       

    // Item List
    std::vector<Item*> items;

private:
    // Textures
    SDL_Texture* coinTexture = nullptr;
    SDL_Texture* BigCoinTexture = nullptr;
    SDL_Texture* OneUpTexture = nullptr;
    SDL_Texture* flagTexture = nullptr;
    SDL_Texture* flagpoleTexture = nullptr;
    SDL_Texture* PowerUpTexture = nullptr;
    SDL_Texture* finishFlagTexture = nullptr;
    SDL_Texture* finishFlagpoleTexture = nullptr;

    // Texture Dimensions
    const char* texturePath = nullptr;
    int texW = 0;
    int texH = 0;

    // Animations
    Animation* currentAnimation = &idle;
    Animation idle;

    Animation* currentAnimationFlag = &flag;
    Animation flag;
    Animation LowCheckpointFlag;
    Animation LowestFlag;
    Animation BigCoin;
    Animation UpdateFinishFlag;

    Animation* currentAnimation_flagpole = &flagpole;
    Animation flagpole;

    Animation* currentAnimationFinishFlag = &finishFlag;
    Animation finishFlag;
    Animation* currentAnimationFinishFlagpole = &finish_flagpole;
    Animation finish_flagpole;

    // XML Parameters
    pugi::xml_node parameters;
};