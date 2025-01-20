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
    name = parameters.attribute("name").as_string();

    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();
    isPicked = parameters.attribute("isPicked").as_int(0);
    isOneUp = parameters.attribute("isOneUp").as_bool(false);
    isCoin = parameters.attribute("isCoin").as_bool(false);
    isBigCoin = parameters.attribute("isBigCoin").as_bool(false);
    isPowerUp = parameters.attribute("isPowerUp").as_bool(false);
    isFinishFlag = parameters.attribute("isFinishFlag").as_bool(false);

    // Inicializar texturas
    coinTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
    OneUpTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("OneUpTexture").as_string());
    flagTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_flag").as_string());
    flagpoleTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_flagpole").as_string());

    finish_flagTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_finish_flag").as_string());
    finish_flagpoleTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_finish_flagpole").as_string());

    BigCoinTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("BigCoinTexture").as_string());
    PowerUpTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("PowerUpTexture").as_string());

    idle.LoadAnimations(parameters.child("animations").child("idle"));
    currentAnimation = &idle;

    flag.LoadAnimations(parameters.child("animations").child("idle_flag"));
    currentAnimation_flag = &flag;

    flagpole.LoadAnimations(parameters.child("animations").child("idle_flagpole"));
    currentAnimation_finish_flagpole = &flagpole;

    finish_flag.LoadAnimations(parameters.child("animations").child("idle_finish_flag"));
    currentAnimation_finish_flag = &finish_flag;

    finish_flagpole.LoadAnimations(parameters.child("animations").child("idle_finish_flagpole"));
    currentAnimation_finish_flagpole = &finish_flagpole;

    lower_flag.LoadAnimations(parameters.child("animations").child("lower_flag"));
    lower_lower_flag.LoadAnimations(parameters.child("animations").child("lower_lower_flag"));
    update_finish_flag.LoadAnimations(parameters.child("animations").child("update_finish_flag"));

    BigCoin.LoadAnimations(parameters.child("animations").child("idleBigCoin"));

    // Obtener dimensiones de las texturas
    Engine::GetInstance().textures.get()->GetSize(coinTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(BigCoinTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(flagpoleTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(flagTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(PowerUpTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(finish_flagpoleTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(finish_flagTexture, texW, texH);

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

    if (apear) {
        Player* player = Engine::GetInstance().scene.get()->GetPlayer();
        if (player != nullptr)
        {
            // Comprobar la distancia entre el ítem y el jugador
            float distance = sqrt(pow(position.getX() - player->position.getX(), 2) + pow(position.getY() - player->position.getY(), 2));

            // Si el ítem es BigCoin, la distancia mínima es 50.0f
            if (isBigCoin && distance < 50.0f && isPicked == 0) {
                isPicked = 1;
                LOG("BigCoin picked up!");
                Engine::GetInstance().scene->UpdateItem(name, isPicked);
                SavePickedStateToFile();
                Engine::GetInstance().entityManager->puntuation += 250;
                Engine::GetInstance().entityManager->objects++;
                Engine::GetInstance().audio.get()->PlayFx(BigCoinFxId); // Reproducir sonido de BigCoin
                LOG("Star Coin collected!");
            }
            // Para los otros ítems, la distancia mínima sigue siendo 35.0f
            else if (!isBigCoin && distance < 35.0f && isPicked == 0) {
                isPicked = 1;
                LOG("Item picked up!");
                Engine::GetInstance().scene->UpdateItem(name, isPicked);
                SavePickedStateToFile();

                // Reproducir el sonido solo si es una moneda
                if (isCoin) {
                    Engine::GetInstance().entityManager->objects++;
                    Engine::GetInstance().entityManager->puntuation += 250;
                    Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId); // Reproducir sonido de Coin
                }
                else if (isPowerUp) {
                    Engine::GetInstance().entityManager->puntuation += 300;
                    Engine::GetInstance().audio.get()->PlayFx(PowerUpFxId); // Reproducir sonido de PowerUp
                    Engine::GetInstance().entityManager.get()->isStarPower = true;; // Reproducir sonido de PowerU
                    LOG("PowerUp collected! Power granted.");
                }
                else if (isOneUp) {
                    Engine::GetInstance().entityManager->puntuation += 300;
                    Engine::GetInstance().entityManager->lives++; // Suma una vida
                    Engine::GetInstance().audio.get()->PlayFx(oneUpFxId, 0); // Reproducir sonido de 1UP
                    LOG("1UP collected! Extra life granted.");
                }
                else if (isFinishFlag) {
                    Engine::GetInstance().entityManager->puntuation += 400;
                    LOG("Collision with Finish Flag");
                    currentAnimation_finish_flag = &update_finish_flag;
                    float targetY = position.getY() + 248.0f;
                    float moveSpeed = 3.0f;
                    if (position.getY() < targetY) {
                        position.setY(position.getY() + 248.0f);
                    }
                }
                else {
                    // Cambiar la animación de la flag
                    Engine::GetInstance().entityManager->puntuation += 400;
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
            if (isFinishFlag && currentAnimation_finish_flag != nullptr) {
                // Actualizar y dibujar animación de Finish Flag
                currentAnimation_finish_flag->Update();
                Engine::GetInstance().render.get()->DrawTexture(
                    finish_flagTexture,
                    (int)position.getX(),
                    (int)position.getY(),
                    &currentAnimation_finish_flag->GetCurrentFrame()
                );

                // Cambiar de animación si ha terminado
                if (currentAnimation_finish_flag == &update_finish_flag && currentAnimation_finish_flag->HasFinished()) {
                    LOG("Finish flag animation completed.");
                    currentAnimation_finish_flag = &finish_flag;
                }
            }
        }
        if (isBigCoin && isPicked == 0) {
            Engine::GetInstance().render.get()->DrawTexture(BigCoinTexture, (int)position.getX()-10, (int)position.getY(), &BigCoin.GetCurrentFrame());
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

        Engine::GetInstance().render.get()->DrawTexture(finish_flagpoleTexture, (int)position.getX(), (int)position.getY(), &currentAnimation_finish_flagpole->GetCurrentFrame());
        currentAnimation_finish_flagpole->Update();

        Engine::GetInstance().render.get()->DrawTexture( finish_flagTexture, (int)position.getX(), (int)position.getY(), &currentAnimation_finish_flag->GetCurrentFrame());
    }
    return true;
}
void Item::SavePickedStateToFile() {
    pugi::xml_document doc;
    if (!doc.load_file("config.xml")) {
        LOG("Error al cargar config.xml para guardar el estado del ítem.");
        return;
    }
    pugi::xml_node itemNode = doc.child("config").child("scene").child("entities").child("items").child("coin");
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
    Engine::GetInstance().textures.get()->UnLoad(finish_flagpoleTexture);
    Engine::GetInstance().textures.get()->UnLoad(finish_flagTexture);
    Engine::GetInstance().textures.get()->UnLoad(BigCoinTexture);
    Engine::GetInstance().textures.get()->UnLoad(OneUpTexture);
    return true;
}
void Item::ResetPosition()
{
    idle.LoadAnimations(parameters.child("animations").child("idle"));
    currentAnimation = &idle;

    flag.LoadAnimations(parameters.child("animations").child("idle_flag"));
    currentAnimation_flag = &flag;

    flagpole.LoadAnimations(parameters.child("animations").child("idle_flagpole"));
    currentAnimation_finish_flagpole = &flagpole;

    finish_flag.LoadAnimations(parameters.child("animations").child("idle_finish_flag"));
    currentAnimation_finish_flag = &finish_flag;

    finish_flagpole.LoadAnimations(parameters.child("animations").child("idle_finish_flagpole"));
    currentAnimation_finish_flagpole = &finish_flagpole;

    lower_flag.LoadAnimations(parameters.child("animations").child("lower_flag"));
    lower_lower_flag.LoadAnimations(parameters.child("animations").child("lower_lower_flag"));
    update_finish_flag.LoadAnimations(parameters.child("animations").child("update_finish_flag"));

    BigCoin.LoadAnimations(parameters.child("animations").child("idleBigCoin"));

    isPicked = 0;
    currentAnimation = &idle;
    currentAnimation_flag = &flag;
    currentAnimation_finish_flag = &finish_flag;
}
void Item::SetPosition(const Vector2D& newPosition) {
    position = newPosition; // Asigna la nueva posición
}

Vector2D Item::GetPosition() const {
    return position; // Devuelve la posición actual
}

