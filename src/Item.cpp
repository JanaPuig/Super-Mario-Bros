#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include <math.h>
#include "EntityManager.h"

Item::Item() : Entity(EntityType::ITEM)
{
    name = "item";
}

Item::~Item() {}

bool Item::Awake() {
    return true;
}

bool Item::Start() {
    // Cargar configuraciones desde XML
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();
    isPicked = parameters.attribute("isPicked").as_int(0);
    isOneUp = parameters.attribute("isOneUp").as_bool(false);
    isCoin = parameters.attribute("isCoin").as_bool(false);
    isBigCoin = parameters.attribute("isBigCoin").as_bool(false);
    isPowerUp = parameters.attribute("isPowerUp").as_bool(false);
    // Inicializar texturas
    coinTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
    OneUpTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("OneUpTexture").as_string());
    flagTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_flag").as_string());
    flagpoleTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_flagpole").as_string());
    BigCoinTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("BigCoinTexture").as_string());
    PowerUpTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("PowerUpTexture").as_string());

    idle.LoadAnimations(parameters.child("animations").child("idle"));
    currentAnimation = &idle;

    flag.LoadAnimations(parameters.child("animations").child("idle_flag"));
    currentAnimation_flag = &flag;

    flagpole.LoadAnimations(parameters.child("animations").child("idle_flagpole"));
    currentAnimation_flagpole = &flagpole;

    lower_flag.LoadAnimations(parameters.child("animations").child("lower_flag"));
    lower_lower_flag.LoadAnimations(parameters.child("animations").child("lower_lower_flag"));

    BigCoin.LoadAnimations(parameters.child("animations").child("idleBigCoin"));

    // Obtener dimensiones de las texturas
    Engine::GetInstance().textures.get()->GetSize(coinTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(BigCoinTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(flagpoleTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(flagTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(PowerUpTexture, texW, texH);

    // Cargar efecto de sonido
    pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Coin.wav"); // Sonido Coin
    CheckPoint = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Checkpoint.wav"); //Sonido CheckPoint
    oneUpFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/1up.wav"); // Sonido 1Up
    BigCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/BigCoin.wav"); // Sonido BigCoin
    PowerUpFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/PowerUp.wav"); // Sonido BigCoin
    return true;
}

bool Item::Update(float dt)
{
    if (Engine::GetInstance().scene.get()->showMainMenu ||
        Engine::GetInstance().scene.get()->showingTransition ||
        Engine::GetInstance().scene.get()->isLoading) {
        return true; // No hacer nada si se está en el menú o en transición
    }

    Player* player = Engine::GetInstance().scene.get()->GetPlayer();
    if (player != nullptr)
    {
        // Comprobar la distancia entre el ítem y el jugador
        float distance = sqrt(pow(position.getX() - player->position.getX(), 2) + pow(position.getY() - player->position.getY(), 2));

        // Si la distancia es menor al umbral, recoger el ítem
        if (distance < 35.0f && isPicked==0) {
            isPicked = 1;
            LOG("Item picked up!");
            Engine::GetInstance().scene->UpdateItem(name, isPicked);
            SavePickedStateToFile();
            // Reproducir el sonido solo si es una moneda
            if (isCoin) {
                Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId); // Reproducir sonido de Coin
            }
            else if (isPowerUp) {
                Engine::GetInstance().audio.get()->PlayFx(PowerUpFxId); // Reproducir sonido de PowerUp
                // Realizar acciones relacionadas con el PowerUp, como otorgar habilidades o poder al jugador
                LOG("PowerUp collected! Power granted.");
            }
            else if (isBigCoin) {
                Engine::GetInstance().audio.get()->PlayFx(BigCoinFxId); // Reproducir sonido de BigCoin
                LOG("Star Coin collected!");
            }
            else if (isOneUp) {
                Engine::GetInstance().entityManager->lives++; // Suma una vida
                Engine::GetInstance().audio.get()->PlayFx(oneUpFxId, 0); // Reproducir sonido de 1UP
                LOG("1UP collected! Extra life granted.");
            }
            else{
            // Cambiar la animación de la flag
            Engine::GetInstance().scene.get()->isFlaged = true;
            Engine::GetInstance().audio.get()->PlayFx(CheckPoint, 0); // Reproducir sonido de checkpoint
            LOG("Collision with Flag");
            currentAnimation_flag = &lower_flag;
            }
        }
        // Cambiar animación a "lower_lower_flag" si la actual termina
        if (currentAnimation_flag == &lower_flag && currentAnimation_flag->HasFinished()) {
            currentAnimation_flag = &lower_lower_flag;
        }
    }
    if (isBigCoin && isPicked == 0) {
        Engine::GetInstance().render.get()->DrawTexture(BigCoinTexture, (int)position.getX(), (int)position.getY(), &BigCoin.GetCurrentFrame());
        BigCoin.Update();
    }
    else if (isPowerUp && isPicked == 0) {
        Engine::GetInstance().render.get()->DrawTexture(PowerUpTexture, (int)position.getX(), (int)position.getY());
    }
    if (isPicked == 0) {
        Engine::GetInstance().render.get()->DrawTexture(isOneUp ? OneUpTexture : coinTexture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
        currentAnimation->Update();

        Engine::GetInstance().render.get()->DrawTexture(flagTexture, (int)position.getX(), (int)position.getY(), &currentAnimation_flag->GetCurrentFrame());
        currentAnimation_flag->Update();
    }
    else {
        Engine::GetInstance().render.get()->DrawTexture(flagTexture, (int)position.getX(), (int)position.getY(), &currentAnimation_flag->GetCurrentFrame());
        currentAnimation_flag->Update();
    }

    // Dibujar el mástil de la bandera
    Engine::GetInstance().render.get()->DrawTexture(flagpoleTexture, (int)position.getX(), (int)position.getY(), &currentAnimation_flagpole->GetCurrentFrame());
    currentAnimation_flagpole->Update();

    return true;
}

void Item::SavePickedStateToFile() {
    pugi::xml_document doc;
    if (!doc.load_file("config.xml")) {
        LOG("Error al cargar config.xml para guardar el estado del ítem.");
        return;
    }

    pugi::xml_node itemNode = doc.child("config").child("scene").child("entities").child("items").child("item");
    itemNode.attribute("isPicked").set_value(isPicked);

    // Guardar el archivo
    doc.save_file("config.xml");
}

int Item::GetisPicked()
{
    return isPicked;
}

bool Item::CleanUp()
{
    Engine::GetInstance().textures.get()->UnLoad(coinTexture);
    Engine::GetInstance().textures.get()->UnLoad(flagpoleTexture);
    Engine::GetInstance().textures.get()->UnLoad(flagTexture);
    Engine::GetInstance().textures.get()->UnLoad(BigCoinTexture);
    Engine::GetInstance().textures.get()->UnLoad(OneUpTexture);
    return true;
}
