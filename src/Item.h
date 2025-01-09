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

public:
    // Audio Fx
    int pickCoinFxId= 0;
    int CheckPoint= 0;
    int oneUpFxId = 0;
    int BigCoinFxId = 0;
    int PowerUpFxId = -1;
    int isPicked;    // Indica si el ítem ha sido recogido

    bool isCoin = false;     // Indica si el ítem es una MONEDA
    bool isOneUp = false;    // Indica si el ítem es una VIDA
    bool isBigCoin = false;
    bool isPowerUp = false;// Indica si el ítem es una MONEDA GRANDE
    bool isFinishFlag = false;
    std::vector<Item*> items;

private:

    SDL_Texture* coinTexture =  NULL;
    SDL_Texture* BigCoinTexture = NULL;
    SDL_Texture* OneUpTexture = NULL;
    SDL_Texture* flagTexture = NULL;
    SDL_Texture* flagpoleTexture = NULL;
    SDL_Texture* PowerUpTexture = NULL;
    SDL_Texture* finish_flagTexture = NULL;
    SDL_Texture* finish_flagpoleTexture = NULL;

    const char* texturePath;
    int texW= 0, texH = 0;

    pugi::xml_node parameters;
    Animation* currentAnimation = &idle;
    Animation idle;

    Animation* currentAnimation_flag = &flag;
    Animation flag;
    Animation lower_flag;
    Animation lower_lower_flag;
    Animation BigCoin;
    Animation update_finish_flag;

    Animation* currentAnimation_flagpole = &flagpole;
    Animation flagpole;


    Animation* currentAnimation_finish_flag = &finish_flag;
    Animation finish_flag;
    Animation* currentAnimation_finish_flagpole = &finish_flagpole;
    Animation finish_flagpole;
};
