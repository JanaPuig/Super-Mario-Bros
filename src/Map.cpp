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

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

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
            // Verifica si la capa es de colisiones
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
                    collider->ctype = ColliderType::PLATFORM;

                  
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
                    deathCollider->ctype = ColliderType::DEATH;  // Asignar tipo de colisionador como DEATH
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

                    // Crear colisionador de muerte
                    PhysBody* deathCollider = Engine::GetInstance().physics->CreateRectangle(x + (width / 2), y + (height / 2), width, height, STATIC);
                    deathCollider->ctype = ColliderType::ROOF;  // Asignar tipo de colisionador como ROOF
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

