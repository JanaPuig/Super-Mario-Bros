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


    // Inicializar texturas
    coinTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
    flagTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_flag").as_string());
    flagpoleTexture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture_flagpole").as_string());

    idle.LoadAnimations(parameters.child("animations").child("idle"));
    currentAnimation = &idle;

    flag.LoadAnimations(parameters.child("animations").child("idle_flag"));
    currentAnimation_flag = &flag;

    flagpole.LoadAnimations(parameters.child("animations").child("idle_flagpole"));
    currentAnimation_flagpole = &flagpole;

    lower_flag.LoadAnimations(parameters.child("animations").child("lower_flag"));
    lower_lower_flag.LoadAnimations(parameters.child("animations").child("lower_lower_flag"));

    // Obtener dimensiones de las texturas
    Engine::GetInstance().textures.get()->GetSize(coinTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(flagpoleTexture, texW, texH);
    Engine::GetInstance().textures.get()->GetSize(flagTexture, texW, texH);

    // Cargar efecto de sonido
    pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Coin.wav");
    CheckPoint = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Checkpoint.wav");

    // Establecer si el ítem es una moneda (basado en XML)
    isCoin = parameters.attribute("isCoin").as_bool(false);

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
                Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
            }
            else{
            // Cambiar la animación
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

    // Dibujar el ítem dependiendo de si fue recogido
    if (isPicked==0) {
        Engine::GetInstance().render.get()->DrawTexture(coinTexture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
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
    return true;
}
