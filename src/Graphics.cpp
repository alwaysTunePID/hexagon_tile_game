#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "Graphics.h"
#include "WorldObjectSprite.h"
#include "Transformations.h"

Graphics::Graphics()
    : clock{}, worldObjectSprites{}, tilesprites{},
      font{}, camera{ INIT_SCALE, WIDTH2 / 2, HEIGHT2 / 2 }, m_reflectionSurface{}, m_tileSurface{}, m_initTime{ Time::now() },
      m_timePastForShader{}, m_globalLightVec{}
{
    if (!m_reflectionSurface.create(WIDTH2, HEIGHT2))
        std::cout << "ERROR: Couldn't create reflectionSurface" << std::endl;
    if (!m_tileSurface.create(TILE_WIDTH, TILE_HEIGHT))
        std::cout << "ERROR: Couldn't create tileSurface" << std::endl;
    //m_reflectionSurface.setSmooth(true); // Should it be smoothed?

    font.loadFromFile("../../../resources/fonts/PixCon.ttf");

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

void Graphics::update(Game& game, sf::RenderWindow& window, displayInput& input, double dt)
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

    drawPlayerGUI(game, window);

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
                        tilesprite_p->updateAndDraw(window, tile, camera, reflectionSprite, m_tileSurface, m_timePastForShader);
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
        WorldObjectSprite* worldObjectSprite_p{ getWorldObjectSprite(worldObject.getType(), worldObject.getId()) };
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
        WorldObjectSprite* worldObjectSprite_p{ getWorldObjectSprite(worldObject.getType(), worldObject.getId()) };
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

void Graphics::drawPlayerGUI(Game& game, sf::RenderWindow& window)
{
    for (auto& [id, player] : game.getPlayers())
    {
        // Need a box / canvas to draw text to ??
        std::string hi{ "Hello" };
        sf::Text texttt{ hi, font };
        // set the character size
        texttt.setCharacterSize(24); // in pixels, not points!
        float turnTimer{ player.getTurnTime() };
        std::string turnTimerStr{ std::to_string(static_cast<int>(turnTimer)) };
        std::string completeStr{};
        float xText{ WIDTH2 - 200 };
        float yText{ HEIGHT2 / 2 };

        if (id == 0)
        {
            yText -= SQ_WIDTH * 2;
        }
        else
        {
            yText += SQ_WIDTH;
        }

        completeStr += player.getName();
        completeStr += ("\n WINS: " + std::to_string(player.getPoints()));
        completeStr += ("\n TIME: " + turnTimerStr);
        texttt.setString(completeStr);

        texttt.setPosition(sf::Vector2f(xText, yText));
        // set the color
        sf::Color gray{ 255, 255, 255, 100 };
        sf::Color textColor{ player.isCurrentPlayer() ? sf::Color::White : gray };
        texttt.setColor(textColor);

        window.draw(texttt);

        //TODO: This will not work when multiplayer
        if (player.isCurrentPlayer())
        {
            // Draw aim arrow
            if (player.isLJoyMode(LJoyMode::aim))
            {
                TileIdx tileIdx{ player.getAimTileIdx() };
                directionType direction{ player.getSelectedSpellDir() };

                int value1{ static_cast<int>(VisualType::aim) };
                WorldObjectSprite* worldObjectSprite1_p{ getWorldObjectSprite(WorldObjectType::visual, value1) };
                //worldObjectSprite1_p->updateSprite(0, tileIdx, player.getAimPos(), direction, camera);
                //worldObjectSprite1_p->draw(window);

                if (IsDirectional(player.getSelectedSpell()))
                {
                    int value{ static_cast<int>(VisualType::aimDir) };
                    WorldObjectSprite* worldObjectSprite_p{ getWorldObjectSprite(WorldObjectType::visual, value) };
                    //worldObjectSprite_p->updateSprite(0, tileIdx, {0.f, 0.f}, direction, camera);
                    //worldObjectSprite_p->draw(window);
                }
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
        std::string hi{ "Hello" };
        sf::Text texttt{ hi, font };
        // set the character size
        texttt.setCharacterSize(24); // in pixels, not points!
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
            texttt.setColor(textColor);
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