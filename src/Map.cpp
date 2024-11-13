#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"
#include "Window.h"
#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
    name = "map";
}

// Destructor
Map::~Map()
{

}

// Called before render is available
bool Map::Awake()
{
    name = "map";
    LOG("Loading Map Parser");

    return true;
}
bool Map::Start() {
    return true;
}

bool Map::Update(float dt)
{
    SDL_RenderClear(Engine::GetInstance().render.get()->renderer);
    bool ret = true;

    if (mapLoaded) {
        // L07 TODO 5: Prepare the loop to draw all tiles in a layer + DrawTexture()
        for (const auto& mapLayer : mapData.layers) {
            for (int i = 0; i < mapData.width; i++) {
                for (int j = 0; j < mapData.height; j++) {
                    // L07 TODO 9: Complete the draw function
                    int gid = mapLayer->Get(i, j);
                    SDL_Rect tileRect = mapData.tilesets.front()->GetRect(gid);
                    Vector2D mapCoord = MapToWorld(i, j);
                    Engine::GetInstance().render->DrawTexture(mapData.tilesets.front()->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
                }
            }
        }
    }

    return ret;
}

TileSet* Map::GetTilesetFromTileId(int gid) const
{
    TileSet* set = nullptr;

    for (const auto& tileset : mapData.tilesets) {
        if (gid >= tileset->firstGid && gid < (tileset->firstGid + tileset->tileCount)) {
            set = tileset;
            break;
        }
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");
  

    for (auto& body : Engine::GetInstance().physics->bodiesToDelete) {
        Engine::GetInstance().physics->DeletePhysBody(body);  // Elimina el cuerpo de la simulación física
        delete body;  // Elimina el cuerpo de la memoria
    }
    Engine::GetInstance().physics->bodiesToDelete.clear();  // Limpiar la lista después de eliminar



    for (const auto& tileset : mapData.tilesets) {
        delete tileset;
    }
    mapData.tilesets.clear();

    for (const auto& layer : mapData.layers) {
        delete layer;
    }
    mapData.layers.clear();

    return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
    bool ret = false;

    mapFileName = fileName;
    mapPath = path;
    std::string mapPathName = mapPath + mapFileName;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
        ret = false;
    }
    else {
        // L06: TODO 3: Implement LoadMap to load the map properties
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();

        // L06: TODO 4: Implement the LoadTileSet function to load the tileset properties
        for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset"))
        {
            TileSet* tileSet = new TileSet();
            tileSet->firstGid = tilesetNode.attribute("firstgid").as_int();
            tileSet->name = tilesetNode.attribute("name").as_string();
            tileSet->tileWidth = tilesetNode.attribute("tilewidth").as_int();
            tileSet->tileHeight = tilesetNode.attribute("tileheight").as_int();
            tileSet->spacing = tilesetNode.attribute("spacing").as_int();
            tileSet->margin = tilesetNode.attribute("margin").as_int();
            tileSet->tileCount = tilesetNode.attribute("tilecount").as_int();
            tileSet->columns = tilesetNode.attribute("columns").as_int();

            std::string imgName = tilesetNode.child("image").attribute("source").as_string();
            tileSet->texture = Engine::GetInstance().textures->Load((mapPath + imgName).c_str());

            mapData.tilesets.push_back(tileSet);
        }

        // L07: TODO 3: Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles.push_back(tileNode.attribute("gid").as_int());
            }

            mapData.layers.push_back(mapLayer);
        }

        // L08 TODO 3: Create colliders
        for (pugi::xml_node objectGroupNode = mapFileXML.child("map").child("objectgroup"); objectGroupNode; objectGroupNode = objectGroupNode.next_sibling("objectgroup"))
        {
            std::string objectGroupName = objectGroupNode.attribute("name").as_string();
            std::string layerName = objectGroupNode.attribute("name").as_string();
            if (objectGroupName == "Collisions")
            {
                for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode; objectNode = objectNode.next_sibling("object"))
                {
                    int x = objectNode.attribute("x").as_int();
                    int y = objectNode.attribute("y").as_int();
                    int width = objectNode.attribute("width").as_int();
                    int height = objectNode.attribute("height").as_int();

                    // Crear colisiones para cada objeto
                    PhysBody* collider = Engine::GetInstance().physics->CreateRectangle(x + (width / 2), y + (height / 2), width, height, STATIC);
                    collider->ctype = ColliderType::PLATFORM; //Asignar tipo de colisionador PLATFORM

                    //deadCollisions.push_back(collider);
                   Engine::GetInstance().physics->bodiesToDelete.push_back(collider);

                  
                    LOG("Creating collider at x: %d, y: %d, width: %d, height: %d", x + (width / 2), y + (height / 2), width, height);
                }
            }
            else if (layerName == "Death")
            {
                // Cargar los objetos de muerte (Death)
                for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object"))
                {
                    int x = objectNode.attribute("x").as_int();
                    int y = objectNode.attribute("y").as_int();
                    int width = objectNode.attribute("width").as_int();
                    int height = objectNode.attribute("height").as_int();

                    // Crear colisionador de muerte
                    PhysBody* deathCollider = Engine::GetInstance().physics->CreateRectangle(x + (width / 2), y + (height / 2), width, height, STATIC);
                    deathCollider->ctype = ColliderType::DEATH;  // Asignar tipo de colisionador DEATH
                    //deadCollisions.push_back(deathCollider);
                    Engine::GetInstance().physics->bodiesToDelete.push_back(deathCollider);

                }
            }
            else if (layerName == "Roof")
            {
                //cargar los objetos de Roof
                for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object"))
                {
                    int x = objectNode.attribute("x").as_int();
                    int y = objectNode.attribute("y").as_int();
                    int width = objectNode.attribute("width").as_int();
                    int height = objectNode.attribute("height").as_int();

                    // Crear colisionador de Roof
                    PhysBody* deathCollider = Engine::GetInstance().physics->CreateRectangle(x + (width / 2), y + (height / 2), width, height, STATIC);
                    deathCollider->ctype = ColliderType::ROOF;  // Asignar tipo de colisionador ROOF
                    //deadCollisions.push_back(deathCollider);
                    Engine::GetInstance().physics->bodiesToDelete.push_back(deathCollider);

                }
            }
            else if (objectGroupName == "Wall")
            {
                for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode; objectNode = objectNode.next_sibling("object"))
                {
                    int x = objectNode.attribute("x").as_int();
                    int y = objectNode.attribute("y").as_int();
                    int width = objectNode.attribute("width").as_int();
                    int height = objectNode.attribute("height").as_int();

                    // Crear colisiones para cada objeto
                    PhysBody* collider = Engine::GetInstance().physics->CreateRectangle(x + (width / 2), y + (height / 2), width, height, STATIC);
                    collider->ctype = ColliderType::WALL; //Asignar tipo de colisionador PLATFORM
                    //deadCollisions.push_back(collider);
                    Engine::GetInstance().physics->bodiesToDelete.push_back(collider);


                    LOG("Creating collider at x: %d, y: %d, width: %d, height: %d", x + (width / 2), y + (height / 2), width, height);
                }
            }
            else if (layerName == "Lucky")
            {
                // Cargar los objetos de muerte (Death)
                for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object"))
                {
                    int x = objectNode.attribute("x").as_int();
                    int y = objectNode.attribute("y").as_int();
                    int width = objectNode.attribute("width").as_int();
                    int height = objectNode.attribute("height").as_int();

                    // Crear colisionador de muerte
                    PhysBody* deathCollider = Engine::GetInstance().physics->CreateRectangle(x + (width / 2), y + (height / 2), width, height, STATIC);
                    deathCollider->ctype = ColliderType::LUCKY;
                    //deadCollisions.push_back(deathCollider);
                    Engine::GetInstance().physics->bodiesToDelete.push_back(deathCollider);


                }
            }
        }
        ret = true;

        // L06: TODO 5: LOG all the data loaded iterate all tilesetsand LOG everything
        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", fileName.c_str());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);

            LOG("Tilesets----");

            for (const auto& tileset : mapData.tilesets) {
                LOG("name : %s firstgid : %d", tileset->name.c_str(), tileset->firstGid);
                LOG("tile width : %d tile height : %d", tileset->tileWidth, tileset->tileHeight);
                LOG("spacing : %d margin : %d", tileset->spacing, tileset->margin);
            }

            LOG("Layers----");

            for (const auto& layer : mapData.layers) {
                LOG("id : %d name : %s", layer->id, layer->name.c_str());
                LOG("Layer width : %d Layer height : %d", layer->width, layer->height);
            }
        }
        else {
            LOG("Error while parsing map file: %s", mapPathName.c_str());
        }

        if (mapFileXML) mapFileXML.reset();
    }

    mapLoaded = ret;
    return ret;
}

// L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
Vector2D Map::MapToWorld(int x, int y) const
{
    Vector2D ret;

    ret.setX(x * mapData.tileWidth);
    ret.setY(y * mapData.tileHeight);

    return ret;
}

Vector2D Map::WorldToMap(int x, int y)
{
    Vector2D mapCoord(0, 0);
        if (mapData.orientation == MapOrientation::ORTOGRAPHIC)
        {
            mapCoord.setX(x / mapData.tileWidth);
            mapCoord.setY(y / mapData.tileHeight);
        }
        else {
            float halfwidth = mapData.tileWidth / 2;
            float halfheight = mapData.tileHeight / 2;
            mapCoord.setX((x / halfwidth + y / halfheight)/2);
            mapCoord.setY((y / halfheight - x / halfwidth) / 2);
        }

    return mapCoord;
}

// L09: TODO 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.push_back(p);
    }

    return ret;
}

MapLayer* Map::GetNavigationLayer() {
    for (const auto& layer : mapData.layers) {
        if (layer->properties.GetProperty("Navigation") != NULL &&
            layer->properties.GetProperty("Navigation")->value) {
            return layer;
        }
    }

    return nullptr;
}
Properties::Property* Properties::GetProperty(const char* name)
{
    for (const auto& property : propertyList) {
        if (property->name == name) {
            return property;
        }
    }

    return nullptr;
}

