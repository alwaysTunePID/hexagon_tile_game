#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include "Graphics.h"
#include "WorldObjectSprite.h"
#include "Transformations.h"

Graphics::Graphics(unsigned seed)
    : clock{}, worldObjectSprites{}, tilesprites{}, tileCosmetics{}, grass{ WorldObjectType::grass, 0},
      font{}, camera{ INIT_SCALE, WIDTH2 / 2, HEIGHT2 / 2 }, m_reflectionSurface{ {WIDTH2, HEIGHT2} },
      m_tileSurface{ {(uint16_t)TILE_WIDTH, (uint16_t)TILE_HEIGHT} }, m_initTime{ Time::now() },
      m_timePastForShader{}, m_globalLightVec{}, generator{ std::default_random_engine{seed} }
{
    //m_reflectionSurface.setSmooth(true); // Should it be smoothed?

    if (!font.openFromFile("../../../resources/fonts/PixCon.ttf"))
        std::cout << "ERROR: Couldn't open font PixCon.ttf" << std::endl;

    if (!sf::Shader::isAvailable())
    {
        std::cout << "ERROR: Shader not available" << std::endl;
    }
}

Graphics::~Graphics()
{
    for (auto& [key, tilesprite_p] : tilesprites)
        delete tilesprite_p;
    for (auto& [key, worldObjectSprite_p] : worldObjectSprites)
        delete worldObjectSprite_p;
}

void Graphics::update(Game& game, sf::RenderWindow& window, displayInput& input, uint8_t callerId, double dt)
{
    window.clear();
    m_reflectionSurface.clear({245, 222, 179, 255}); // sand color

    sf::RectangleShape background(sf::Vector2f(WIDTH2, HEIGHT2));
    sf::Color backgroundColor{ 21, 24, 31 };
    background.setFillColor(backgroundColor);
    window.draw(background);

    updateCamera(input);

    // TODO: Change this to sfml clock?
    timePoint currentTime{ Time::now() };
    timeDuration timePast{ currentTime - m_initTime };
    m_timePastForShader = static_cast<float>(timePast.count());

    updateGlobalLightVec2(timePast.count());

    /*sf::RectangleShape board(sf::Vector2f(COLUMNS * SQ_WIDTH, ROWS * SQ_WIDTH));
    board.setPosition({
        WIDTH2 / 2 - (static_cast<int>(COLUMNS) / 2) * static_cast<int>(SQ_WIDTH),
        HEIGHT2 / 2 - (static_cast<int>(ROWS) / 2) * static_cast<int>(SQ_WIDTH) });
    board.setFillColor(sf::Color::Black);
    window.draw(board);*/

    float lapTime = clock.getElapsedTime().asSeconds();
    bool elapsed = (lapTime > 0.25f) ? true : false;
    if (elapsed)
    {
        clock.restart();

        /*for (auto& [pairShit, worldObjectSprite] : worldObjectSprites)
            worldObjectSprite.updateFrameIdx();

        for (auto& [tiletype, tilesprite] : tilesprites)
            tilesprite.updateFrameIdx();*/
    }

    drawReflections(game);
    m_reflectionSurface.display();

    sf::Sprite reflectionSprite{ m_reflectionSurface.getTexture() };

    drawTiles(game, window, reflectionSprite);

    drawWorldObjects(game, window);

    for (castedSpellData& castedSpell : game.getCastedSpells())
    {
        if (castedSpell.spellType == SpellType::teleport) {
            continue;
        }
        else {
            int value{ static_cast<int>(castedSpell.spellType) };
            WorldObjectSprite* worldObjectSprite_p{ getWorldObjectSprite(WorldObjectType::spell, value) };
            //worldObjectSprite_p->updateSprite(castedSpell, camera);
            //worldObjectSprite_p->draw(window);
        }
    }

    //window.draw(sf::Sprite(m_reflectionSurface.getTexture()));

    if (input.showCoordinateSystem)
        drawCoordinateSystem(window);

    drawPlayerGUI(game, window, callerId);

    window.display();
}

void Graphics::drawTiles(Game& game, sf::RenderWindow& window, sf::Sprite& reflectionSprite)
{
    bool isFirstWaterTile{ true };
    //TODO: FIX THIS HARDCODED 
    int boardSize = 8;

    for (int i = 0; i <= boardSize; i++)
    {
        int r{ i - boardSize / 2 };

        for (int ii = 0; ii <= boardSize; ii++)
        {
            int c{ ii - boardSize / 2 };

            TileIdx tileIdx{ r, c };
            if (game.isOutOfBounds(tileIdx))
                continue;

            // Optimize this shit!!
            for (auto& [tileId, tile] : game.getTiles())
            {
                if (tile.getTileIdx().first == r && tile.getTileIdx().second == c)
                {
                    Tilesprite* tilesprite_p{ getTilesprite(tile) };
                    if (tile.getTileType() == tileType::water)
                    {
                        if (isFirstWaterTile)
                        {
                            tilesprite_p->updateWaterSurfaceTexture(tile, camera);
                            isFirstWaterTile = false;
                        }
                        tilesprite_p->updateAndDraw(window, tile, camera, m_globalLightVec, reflectionSprite, m_tileSurface, m_timePastForShader);
                    }
                    else if (tile.getTileType() == tileType::grass)
                    {
                        tilesprite_p->updateSprite(tile, camera);
                        tilesprite_p->draw(window, tile);
                        for (auto& cosmetic : *getTileCosmetics(tile))
                        {
                            grass.updateSprite(cosmetic, camera, m_globalLightVec, false);
                            grass.draw(window);
                        }
                    }
                    else
                    {
                        tilesprite_p->updateSprite(tile, camera);
                        tilesprite_p->draw(window, tile);
                    }
                    

                    // Draw effects
                    /*
                    for (effectData& effect : tile.getProperties())
                    {
                        if (HasTexture(effect.type))
                        {
                            int value{ static_cast<int>(effect.type) };
                            WorldObjectSprite& worldObjectSprite{ getWorldObjectSprite(WorldObjectType::effect, value) };
                            worldObjectSprite.updateSprite(tile.getId(), effect, tile.getTileIdx(), camera);
                            worldObjectSprite.draw(window);
                        }
                        // Is this really the way to do it?
                        if (effect.type == EffectType::spawn)
                            effect.active = false;
                    }*/
                }
            }
        }
    }
}

// TODO: Optimize this sorting shit
void Graphics::sortWorldObjects(Game& game, std::vector<WorldObject>& worldObjects)
{
    for (auto& [id, worldObject] : game.getWorldObjects())
    {
        //TileIdx tileIdx{ WorldPosToTileIdx(worldObject.getPos()) };
        //std::cout << "id: " << worldObject.getId() << " tileIdx: " << tileIdx.first << " " << tileIdx.second << std::endl;
        worldObjects.push_back(worldObject);
    }

    displayInput camera_cpy{ camera };
    // TODO: Don't think this lambda is correct when introducing z coordinate
    // defining custom comparator as lambda expression
    auto comp = [camera_cpy] (WorldObject a, WorldObject b)
    {
        screenPos s_pos_a{ WorldToScreenPos(a.getPos(), camera_cpy) };
        screenPos s_pos_b{ WorldToScreenPos(b.getPos(), camera_cpy) };
        
        return s_pos_a.y < s_pos_b.y;
    };

    std::sort(worldObjects.begin(), worldObjects.end(), comp);
}

void Graphics::drawWorldObjects(Game& game, sf::RenderWindow& window)
{
    std::vector<WorldObject> worldObjects;
    sortWorldObjects(game, worldObjects);
    bool reflected{ false };

    for (WorldObject& worldObject : worldObjects)
    {
        int value{ worldObject.getId() };
        // TODO: Temp for visual
        if (worldObject.getType() == WorldObjectType::visual)
            continue;
        else if (worldObject.getType() == WorldObjectType::player)
            value = game.getPlayerIdToWOId(worldObject.getId());

        WorldObjectSprite* worldObjectSprite_p{ getWorldObjectSprite(worldObject.getType(), value) };
        worldObjectSprite_p->updateSprite(worldObject, camera, m_globalLightVec, reflected);
        worldObjectSprite_p->draw(window);
    }
}


void Graphics::drawReflections(Game& game)
{
    std::vector<WorldObject> worldObjects;
    sortWorldObjects(game, worldObjects);
    bool reflected{ true };

    // Draw objects in reflected perspective
    for (WorldObject& worldObject : worldObjects)
    {
        int value{ worldObject.getId() };
        // TODO: Temp for visual
        if (worldObject.getType() == WorldObjectType::visual)
            continue;
        else if (worldObject.getType() == WorldObjectType::player)
            value = game.getPlayerIdToWOId(worldObject.getId());

        WorldObjectSprite* worldObjectSprite_p{ getWorldObjectSprite(worldObject.getType(), value) };
        worldObjectSprite_p->updateSprite(worldObject, camera, m_globalLightVec, reflected);
        worldObjectSprite_p->draw(m_reflectionSurface);
    }
}

void Graphics::drawCoordinateSystem(sf::RenderWindow& window)
{
    worldPos unitWorldPos{ 0, 0, 0 };
    int value{ static_cast<int>(VisualType::dot) };
    WorldObjectSprite* dotSprite_p{ getWorldObjectSprite(WorldObjectType::visual, value) };

    for (int x{-4}; x <= 4; x++)
    {
        for (int y{-4}; y <= 4; y++)
        {
            unitWorldPos.x = x;
            unitWorldPos.y = y;

            dotSprite_p->updateSprite(unitWorldPos, camera);
            dotSprite_p->draw(window);
        }
    }

    // TODO: Move this to Enum.h and then to its own helper file
    // 1   / tan (60) = 0.57735;
    // 0.5 * tan (30) = 0.28868;
    double a{ 0.57735 };
    double b{ 0.28868 };

    worldPos vertex0{ a,    0, 0}; // x, y, z
    worldPos vertex1{ b, -0.5, 0};
    worldPos vertex2{-b, -0.5, 0};
    worldPos vertex3{-a,    0, 0};
    worldPos vertex4{-b,  0.5, 0};
    worldPos vertex5{ b,  0.5, 0};

    std::vector<worldPos> boundVertices{ vertex0, vertex1, vertex2, vertex3, vertex4, vertex5 };
    for (worldPos& boundVertex : boundVertices)
    {
        dotSprite_p->updateSprite(boundVertex, camera);
        dotSprite_p->draw(window);
    }
}

void Graphics::drawPlayerGUI(Game& game, sf::RenderWindow& window, uint8_t callerId)
{
    for (auto& [id, player] : game.getPlayers())
    {
        sf::Text texttt( font );
        texttt.setCharacterSize(24);
        float turnTimer{ player.getTurnTime() };
        std::string turnTimerStr{ std::to_string(static_cast<int>(turnTimer)) };
        std::string completeStr{};
        float xText{ WIDTH2 - 240 };
        float yText{ HEIGHT2 / 2 };

        if (id == 0)
        {
            yText -= SQ_WIDTH * 3.f;
        }
        else
        {
            yText += SQ_WIDTH;
        }

        playerStats stats{ player.getStats() };

        completeStr += player.getName();
        completeStr += ("\n DEATHS: " + std::to_string(stats.deaths));
        completeStr += ("\n KILLS: " + std::to_string(stats.kills));
        completeStr += ("\n TIME: " + turnTimerStr);
        texttt.setString(completeStr);

        texttt.setPosition(sf::Vector2f(xText, yText));
        // set the color
        sf::Color gray{ 255, 255, 255, 100 };
        sf::Color textColor{ player.isCurrentPlayer() ? sf::Color::White : gray };
        texttt.setFillColor(textColor);

        window.draw(texttt);

        // TODO: This shouldn't be here
        if (id == (int)callerId)
        {
            texttt.setCharacterSize(12);
            worldPos pos{ game.getWorldObject(player.getWorldObjectIds()).getPos() };
            TileIdx tileIdx{ WorldPosToTileIdx(pos) };

            std::string xStr{ std::to_string(std::round(pos.x * 100.0) / 100.0) };
            std::string yStr{ std::to_string(std::round(pos.y * 100.0) / 100.0) };
            xStr = (xStr.at(0) == '-') ? xStr.substr(0,5) : " " + xStr.substr(0,4);
            yStr = (yStr.at(0) == '-') ? yStr.substr(0,5) : " " + yStr.substr(0,4); 
            completeStr = "x: " + xStr + ", y: " + yStr + "\nTile: "
                                + std::to_string(tileIdx.first) + ", " + std::to_string(tileIdx.second);
            texttt.setString(completeStr);
            texttt.setPosition(sf::Vector2f(10, 16));
            window.draw(texttt);


            // Draw aim arrow
            if (player.isLJoyMode(LJoyMode::aim))
            {
                WorldObject& worldObject{ game.getWorldObject(player.getAimId()) };
                bool reflected{ false };
                int value1{ static_cast<int>(VisualType::aim) };
                WorldObjectSprite* worldObjectSprite_p{ getWorldObjectSprite(worldObject.getType(), value1) };
                worldObjectSprite_p->updateSprite(worldObject, camera, m_globalLightVec, reflected);
                worldObjectSprite_p->draw(window);

                //if (IsDirectional(player.getSelectedSpell()))
                //{
                //    directionType direction{ player.getSelectedSpellDir() };
                //    int value{ static_cast<int>(VisualType::aimDir) };
                //    WorldObjectSprite* worldObjectSprite_p{ getWorldObjectSprite(WorldObjectType::visual, value) };
                //    //worldObjectSprite_p->updateSprite(0, tileIdx, {0.f, 0.f}, direction, camera);
                //    //worldObjectSprite_p->draw(window);
                //}
            }
            drawInventory(player, window);
        }
    }
}



void Graphics::updateCamera(displayInput& input)
{
    if (std::abs(input.zoom) > 1.0)
    {
        camera.zoom += input.zoom / 500.0;
        if (camera.zoom > 20.0)
            camera.zoom = 20.0;
        else if (camera.zoom < 1.0)
            camera.zoom = 1.0;
    }
    if (std::abs(input.horizontal) > 5.0)
    {
        camera.horizontal -= input.horizontal / 2.0;
        if (camera.horizontal > 2000.0 * camera.zoom)
            camera.horizontal = 2000.0 * camera.zoom;
        else if (camera.horizontal < 0.0)
            camera.horizontal = 0.0;
    }
    if (std::abs(input.vertical) > 5.0)
    {
        camera.vertical -= input.vertical / 2.0;
        if (camera.vertical > 1100.0 * camera.zoom)
            camera.vertical = 1100.0 * camera.zoom;
        else if (camera.vertical < 0.0)
            camera.vertical = 0.0;
    }

    camera.enableShaders = input.enableShaders;

    // Toggle tile spacing
    camera.tileSpacing = (input.tileSpacing) ? !camera.tileSpacing : camera.tileSpacing;
}

void Graphics::drawInventory(Player& player, sf::RenderWindow& window)
{
    if (player.isLJoyMode(LJoyMode::aim))
    {
        sf::Text texttt( font );
        texttt.setCharacterSize(24);
        sf::Color gray{ 255, 255, 255, 100 };
        std::string completeStr{};
        float xText{ 20 };
        float yText{ 7 * HEIGHT2 / 8 };
        int idx{ 0 };

        for (SpellType spell : player.getSelectionSpells())
        {
            completeStr = ToString(spell);
            texttt.setString(completeStr);
            texttt.setPosition(sf::Vector2f(xText, yText));
            sf::Color textColor{ (player.getSelectedSpellIdx() == idx) ? sf::Color::White : gray };
            texttt.setFillColor(textColor);
            window.draw(texttt);
            yText += 30;
            idx++;
        }
    }
}

WorldObjectSprite* Graphics::getWorldObjectSprite(WorldObjectType objectType, int value)
{
    std::pair<WorldObjectType, int> key{ objectType, value };
    if (worldObjectSprites.find(key) == worldObjectSprites.end()) {
        // Couldn't find sprite so add it
        worldObjectSprites.insert({ key, new WorldObjectSprite(objectType, value) });
    }
    return worldObjectSprites[key];
}

// For some reason I couldn't get this to build if I didn't make it a pointer to the Tilesprite
Tilesprite* Graphics::getTilesprite(Tile& tile)
{
    if (tilesprites.find(tile.getTileType()) == tilesprites.end()) {
        // Couldn't find sprite so add it
        tilesprites.insert({tile.getTileType(), new Tilesprite(tile)});
    }
    return tilesprites[tile.getTileType()];
}

std::vector<woCosmetic>* Graphics::getTileCosmetics(Tile& tile)
{
    if (tileCosmetics.find(tile.getId()) == tileCosmetics.end()) {
        // Couldn't find sprite so add it
        double inc{ 0.2 };
        std::uniform_real_distribution<double> dist(0.0, 0.15);
        std::vector<woCosmetic> cosmeticVec;
        worldPos c_pos{ TileIdxToWorldPos(tile.getTileIdx()) };
        for (int i{0}; i < 4; i++)
        {
            for (int ii{0}; ii < 4; ii++)
            {
                worldPos w_pos_diff{
                    (inc * i - 0.42 + dist(generator)),
                    (inc * ii - 0.42 + dist(generator)),
                    0
                };
                w_pos_diff = CartesianToWorldPos(w_pos_diff);
                worldPos w_pos{ 
                    c_pos.x + w_pos_diff.x,
                    c_pos.y + w_pos_diff.y
                };
                woCosmetic grass{ (i+ii)%3, w_pos };
                cosmeticVec.push_back(grass);
            }
        }
        tileCosmetics.insert({tile.getId(), cosmeticVec});
    }
    return &tileCosmetics.at(tile.getId());
}

void Graphics::updateGlobalLightVec(double timePast)
{
    worldPos v{};
    double r_sun{ 1.0 };
    double dayCycleFactor{ 0.1 };
    v.x = 2.0;

    double angle{ std::fmod(dayCycleFactor * timePast, PI) };
    v.y = - r_sun * std::cos(angle);
    v.z =   r_sun * std::sin(angle);

    v = WorldToNormalVec(v);

    double vLength{ std::sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2)) };

    v.x = -v.x / vLength;
    v.y = -v.y / vLength;
    v.z = -v.z / vLength;

    m_globalLightVec = v;
}

// This calculate global light vector in normal map coordinate system
void Graphics::updateGlobalLightVec2(double timePast)
{
    worldPos v{};
    double r_sun{ 1.0 };
    double dayCycleFactor{ 0.1 };
    v.z = 2.0;

    double angle{ std::fmod(dayCycleFactor * timePast, PI) };
    v.x = - r_sun * std::cos(angle);
    v.y =   r_sun * std::sin(angle);


    double vLength{ std::sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2)) };

    v.x = -v.x / vLength;
    v.y = -v.y / vLength;
    v.z = -v.z / vLength;

    m_globalLightVec = v;
}