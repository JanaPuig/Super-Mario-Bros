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

public:
    // Audio Fx
    int pickCoinFxId= 0;
    int CheckPoint= 0;
    // Indica si el ítem ha sido recogido
    bool isPicked = false;

    // Indica si el ítem es una moneda
    bool isCoin = false;

    std::vector<Item*> items;

private:

    SDL_Texture* coinTexture;
    SDL_Texture* flagTexture;
    SDL_Texture* flagpoleTexture;

    const char* texturePath;
    int texW= 0, texH = 0;

    pugi::xml_node parameters;
    Animation* currentAnimation = &idle;
    Animation idle;

    Animation* currentAnimation_flag = &flag;
    Animation flag;
    Animation lower_flag;
    Animation lower_lower_flag;

    Animation* currentAnimation_flagpole = &flagpole;
    Animation flagpole;
};
