#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Graphics.h"
#include "WorldObjectSprite.h"
#include "Transformations.h"

Graphics::Graphics(unsigned seed)
    : clock{}, worldObjectSprites{}, tilesprites{}, tileCosmetics{}, grass{ WorldObjectType::grass, 0},
      font{}, camera{ INIT_SCALE, WIDTH2 / 2, HEIGHT2 / 2 }, m_reflectionSurface{ {WIDTH2, HEIGHT2} },
      m_tileSurface{ {(uint16_t)TILE_WIDTH, (uint16_t)TILE_HEIGHT} }, m_initTime{ Time::now() },
      m_timePastForShader{}, m_globalLightVec{}, generator{ std::default_random_engine{seed} }, particleSimOngoing{ false },
      particleBuffer{}, fadeProgram{}, computeProgram{}, renderProgram{}, vao{}, prevPosTexture{}, trailTexture{}, psTexture{}, quadVAO{}, quadVBO{},
      trailTextureRes{}, trailTexturePos{}, cOrigin{}, circle{}
{
    //m_reflectionSurface.setSmooth(true); // Should it be smoothed?

    if (!font.openFromFile("../../../resources/fonts/PixCon.ttf"))
        std::cout << "ERROR: Couldn't open font PixCon.ttf" << std::endl;

    if (!sf::Shader::isAvailable())
    {
        std::cout << "ERROR: Shader not available" << std::endl;
    }

    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;
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

    // Only for debugging
    //window.draw(sf::Sprite(m_reflectionSurface.getTexture()));

    if (input.showCoordinateSystem)
        drawCoordinateSystem(window);

    drawPlayerGUI(game, window, callerId);

    updateAndDrawParticleSimulation(window, input, dt);
    //window.resetGLStates();

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
        sf::Color textColor{ game.getCurrentPlayerId() == id ? sf::Color::White : gray };
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
            if (player.isLJoyMode(LJoyMode::aim) && (int)callerId == game.getCurrentPlayerId())
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

void Graphics::InitQuad() {
    float quadVertices[] = {
        // positions         // texCoords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // Top-left
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // Bottom-right

        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // Top-left
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f  // Top-right
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

// Initialize OpenGL & create buffer
void Graphics::InitParticles()
{
    // Generate random particle positions
    std::vector<Particle> particles(NUM_PARTICLES);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.5f, 0.5f);

    // TODO: Don't think this init is used any more
    // should be removed because it is inefficient
    for (auto& p : particles) {
        p.x_n = dist(gen);
        p.y_n = dist(gen);
        p.z_n = dist(gen);
        p.w_n = 1.0f;
        p.x = p.y = p.z = p.w = 0.0f;
        p.vx = p.vy = p.vz = p.vw = 0.0f;
        p.lifeTime = 1.0f + dist(gen);
        p.totalLifeTime = p.lifeTime;
        p.activated = -1.0f;
        p.p3 = 0.0f;
    }

    // Create OpenGL buffer for particles
    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW); // nullptr instead of particles.data()?
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);

    // Create VAO for rendering
    glGenVertexArrays(1, &vao);

    // Create and initialize the textures
    glGenTextures(1, &prevPosTexture);
    glBindTexture(GL_TEXTURE_2D, prevPosTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, trailTextureRes.x, trailTextureRes.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &trailTexture);
    glBindTexture(GL_TEXTURE_2D, trailTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, trailTextureRes.x, trailTextureRes.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &psTexture);
    glBindTexture(GL_TEXTURE_2D, psTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, trailTextureRes.x, trailTextureRes.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    InitQuad();
}

void Graphics::updateAndDrawParticleSimulation(sf::RenderWindow& window, displayInput& input, double dt)
{
    if (input.enableShaders)
    {
        // TODO: Things are assumed around this texture size
        trailTextureRes = sf::Vector2f(256.f, 256.f);
        cOrigin = sf::Vector2f(200.f, 200.f);

        float cRadius{ trailTextureRes.x / 10.f };
    
        circle.setRadius(cRadius);
        circle.setOrigin({cRadius, cRadius});
        circle.setFillColor(sf::Color::Black);
        circle.setPosition(cOrigin);
        window.draw(circle);

        window.setActive(true);

        if (!particleSimOngoing)
        {
            // Compile shaders
            fadeProgram = CreateComputeShader("../../../shaders/fadeTrail.comp");
            computeProgram = CreateComputeShader("../../../shaders/partSim.comp");
            renderProgram = CreateProgram(
                LoadShader("../../../shaders/partSim.vert", GL_VERTEX_SHADER),
                LoadShader("../../../shaders/partSim.frag", GL_FRAGMENT_SHADER));
            InitParticles();
            particleSimOngoing = true;
            std::cout << "Init of Particles complete" << std::endl;
        }

        FadeParticleTrail(dt);

        UpdateParticles(dt);

        //glClear(GL_COLOR_BUFFER_BIT);
        RenderParticles();
    }
}

// Update particles using compute shader
void Graphics::FadeParticleTrail(float dt) {
    glUseProgram(fadeProgram);
    // Bind the texture to an image unit before dispatching the compute shader
    glBindImageTexture(0, trailTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    glUniform1f(glGetUniformLocation(fadeProgram, "deltaTime"), dt);

    // TODO: Hardcoded to the texture size.
    // workgroup size * num invocations = texture size => 16 * 16 = 256, in both x and y
    glDispatchCompute(16, 16, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Unbind the image texture
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glUseProgram(0);
}

void Graphics::UpdateParticles(float dt) {
    // TODO: Temp, use coord from object
    // Treat this as the coordinates you set to SFML objects. So screen coordinates.
    float cRadius{ trailTextureRes.x / 10.f };
    // Make the texture position fit the particle system
    trailTexturePos = sf::Vector2f( cOrigin.x - trailTextureRes.x / 2.0, 
                                    cOrigin.y - trailTextureRes.y + 50.0);

    glUseProgram(computeProgram);
    // Bind the texture to an image unit before dispatching the compute shader
    glBindImageTexture(0, prevPosTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, trailTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(2, psTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glUniform1f(glGetUniformLocation(computeProgram, "deltaTime"), dt);
    // TODO: These needs to be dynamic (WIDTH2, HEIGHT2)
    glUniform2f(glGetUniformLocation(computeProgram, "uResolution"), (float)WIDTH2, (float)HEIGHT2);
    glUniform2f(glGetUniformLocation(computeProgram, "trailTexRes"), trailTextureRes.x, trailTextureRes.y);
    glUniform2f(glGetUniformLocation(computeProgram, "trailTexPos"), trailTexturePos.x, trailTexturePos.y);
    glUniform2f(glGetUniformLocation(computeProgram, "circleOrigin"), cOrigin.x, cOrigin.y);
    glUniform1f(glGetUniformLocation(computeProgram, "circleRadius"), cRadius);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
    // Maybe change to this
    //int numWorkGroups = (NUM_PARTICLES + 255) / 256;
    glDispatchCompute(NUM_PARTICLES / 256, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearTexImage(prevPosTexture, 0, GL_RGBA, GL_FLOAT, clearColor);

    // Unbind the image texture
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void Graphics::RenderParticles() {
    glUseProgram(renderProgram);

    // This was so hard to get right. Texture transformation took as long as writing the compute shader code...
    float texScale = 1.f;
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-(((float)WIDTH2  - trailTextureRes.x * texScale) / 2.f) / ((float)WIDTH2 / 2.f)  + (trailTexturePos.x * texScale) / ((float)WIDTH2 / 2.f), 
                                             (((float)HEIGHT2 - trailTextureRes.y * texScale) / 2.f) / ((float)HEIGHT2 / 2.f) - (trailTexturePos.y * texScale) / ((float)HEIGHT2 / 2.f),
                                             0.0f));
    //trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3((trailTextureRes.x / (float)WIDTH2) * texScale,
                                        (trailTextureRes.y / (float)HEIGHT2) * texScale,
                                        1.f));

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, trailTexture);
    glUniform1i(glGetUniformLocation(renderProgram, "psTexture"), 0);
    glUniformMatrix4fv(glGetUniformLocation(renderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

    // Bind the quad VAO and draw the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Unbind the texture and reset the active texture unit
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(0);

    // Set psTexture as prevPosTexture for next frame
    std::swap(prevPosTexture, psTexture);
}

GLuint Graphics::LoadShader(const std::string& filePath, GLenum type)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source{ buffer.str() };

    GLuint shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compilation error: " << log << "\n";
    }

    return shader;
}

GLuint Graphics::CreateComputeShader(const std::string& filePath) {
    GLuint computeShader = LoadShader(filePath, GL_COMPUTE_SHADER);
    GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);

    // Check for linking errors
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program linking error: " << log << "\n";
    }

    glDeleteShader(computeShader);
    return program;
}

GLuint Graphics::CreateProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program linking error: " << log << "\n";
    }

    // Validate program
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program validation error: " << log << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}