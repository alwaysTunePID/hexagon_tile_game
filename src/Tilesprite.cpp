#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <iostream>
#include "Tile.h"
#include "Tilesprite.h"
#include "Enums.h"
#include "Transformations.h"

Tilesprite::Tilesprite(Tile& tile)
    : textures{}, highlightTexture{}, frameIdx{ 0 }, tileShape{}, m_tileSurface{}, sprite{ highlightTexture }, highlightSprite{ highlightTexture }, type{ tile.getTileType() }, m_shader{}
{
    std::string tileStr{ ToString(type) };
    std::string imagePath{ "../../../resources/textures/Tiles/" + tileStr + "/"};

    std::string completePath{};
    sf::Texture texture{};
    int m{ 0 };

    switch (GetTextureType(type))
    {
    case textureType::directional:
        m = 3; // 3 * 6;
        break;

    default:
        m = 1;
        break;
    }

    for (int j = 0; j < m; j++) {
        completePath = (imagePath + tileStr + std::to_string(j + 1) + ".png");
        texture.loadFromFile(completePath);
        textures.push_back(texture);
    }

    sprite.setTexture(textures[0], true);
    sprite.setOrigin({TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f - 1.f}); // -1 because of tile thickness 
    sprite.setScale(sf::Vector2f(INIT_SCALE, INIT_SCALE));

    // Highlight tile
    std::string imagePath2{ "../../../resources/textures/HighlightTile/HighlightTile.png" };
    highlightTexture.loadFromFile(imagePath2);
    highlightSprite.setTexture(highlightTexture, true);
    highlightSprite.setOrigin({TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f - 1.f}); // -1 because of tile thickness
    highlightSprite.setScale(sf::Vector2f(INIT_SCALE, INIT_SCALE));

    // load only the fragment shader
    if (type == tileType::water)
    {
        if (!m_shader.loadFromFile("../../../shaders/blur.frag", sf::Shader::Type::Fragment))
        {
            std::cout << "ERROR: Couldn't load blur.frag" << std::endl;
        }
        m_shader.setUniform("texturee", sf::Shader::CurrentTexture);
    }

    if (type == tileType::water)
    {
        if (!m_tileSurface.resize({WIDTH2, HEIGHT2}))
            std::cout << "ERROR: Couldn't create tileSurface" << std::endl;

        tileShape.setPointCount(8);
        tileShape.setPoint(0, sf::Vector2f(8.f, 4.f));
        tileShape.setPoint(1, sf::Vector2f(32.f, 0.f));
        tileShape.setPoint(2, sf::Vector2f(48.f, 8.f));
        tileShape.setPoint(3, sf::Vector2f(48.f, 9.f));
        tileShape.setPoint(4, sf::Vector2f(40.f, 20.f));
        tileShape.setPoint(5, sf::Vector2f(16.f, 24.f));
        tileShape.setPoint(6, sf::Vector2f(0.f, 16.f));
        tileShape.setPoint(7, sf::Vector2f(8.f, 5.f));
        tileShape.setFillColor(WATER_COLOR);
        //tileShape.setOrigin(TILE_WIDTH / 2 - 2, TILE_HEIGHT / 2 - 3);
    }

}

Tilesprite::Tilesprite()
    : textures{}, highlightTexture{}, frameIdx{ 0 }, tileShape{}, m_tileSurface{}, sprite{ highlightTexture }, highlightSprite{ highlightTexture }, type{}, m_shader{}
{}

Tilesprite::~Tilesprite()
{}

void Tilesprite::setPosFromTileIdxLegacy(TileIdx tileIdx, displayInput& camera)
{
    double spacing{ camera.tileSpacing ? 1.0 : 0.0 };

    double board_width{ 
        static_cast<double>(tileIdx.first) * (SQ_WIDTH + spacing) * camera.zoom  * 0.75 +
        static_cast<double>(tileIdx.second) * (SQ_WIDTH + spacing) * camera.zoom * 0.75 };
    double board_height{
        static_cast<double>(tileIdx.first) * (SQ_HEIGHT + spacing) * camera.zoom / 2.0 -
        static_cast<double>(tileIdx.second) * (SQ_HEIGHT + spacing) * camera.zoom / 2.0 };

    double tile_width{};
    double tile_height{};

    tile_width  = - (SQ_WIDTH + spacing) * camera.zoom / 2.0;
    tile_height = - (SQ_HEIGHT + spacing) * camera.zoom / 2.0 - 12.0 * camera.zoom;

    sprite.setPosition({
        static_cast<float>(camera.horizontal + board_width + tile_width),
        static_cast<float>(camera.vertical + board_height + tile_height) });

    highlightSprite.setPosition(sprite.getPosition());
}

void Tilesprite::setPosFromTileIdx(TileIdx tileIdx, displayInput& camera)
{
    screenPos s_pos = WorldToScreenPos(TileIdxToWorldPos(tileIdx), camera);
    // Temp to lower water
    if (type == tileType::water)
        s_pos.y += 1.0 * camera.zoom;

    sprite.setPosition({s_pos.x, s_pos.y});

    highlightSprite.setPosition(sprite.getPosition());

    // shader.setUniform("myvar", 5.f);
    // shader.setUniform("wave_phase", time);
    // shader.setUniform("wave_amplitude", sf::Vector2f(x * 40, y * 40));
    // shader.setUniform("blur_radius", (x + y) * 0.008f);
}

void Tilesprite::updateFrameIdx()
{
    frameIdx = (frameIdx == 2) ? 0 : frameIdx + 1;
}

void Tilesprite::updateWaterSurfaceTexture(Tile& tile, displayInput& camera)
{
    if (camera.enableShaders == false)
        return;

    m_tileSurface.clear({0,0,0,0});
    //sf::View view(sf::FloatRect(0.f, 0.f, TILE_WIDTH * camera.zoom, TILE_HEIGHT * camera.zoom));
    //m_tileSurface.setView(view);
    // Change these hardcoded to correct values
    tileShape.setPosition({0.f, 1.f * camera.zoom});
    tileShape.setScale(sf::Vector2f(camera.zoom, camera.zoom));
    m_tileSurface.draw(tileShape);
    m_tileSurface.display();

    if (textures.size() >= 2)
        textures.pop_back();
    textures.push_back(m_tileSurface.getTexture());
}

void Tilesprite::updateAndDraw(sf::RenderWindow& window, Tile& tile, displayInput& camera, worldPos& globalLightVec, sf::Sprite& reflectionSprite, sf::RenderTexture& reflectionSurface, float timePastForShader)
{
    // https://www.sfml-dev.org/tutorials/2.6/graphics-shape.php
    // https://thebookofshaders.com/13/

    bool resetRect{ true };
    
    if (camera.enableShaders)
    {
        screenPos s_pos{ WorldToScreenPos(TileIdxToWorldPos(tile.getTileIdx()), CAMERA_0) };

        int rectLeft{ static_cast<int>(s_pos.x - TILE_WIDTH * CAMERA_0.zoom / 2.0) };
        int rectTop{ static_cast<int>(s_pos.y - (TILE_HEIGHT / 2.0 - 1) * CAMERA_0.zoom) };
        reflectionSprite.setTextureRect(sf::IntRect(
            {rectLeft, rectTop},
            {static_cast<int>(round(TILE_WIDTH * CAMERA_0.zoom)),
             static_cast<int>(round(TILE_HEIGHT * CAMERA_0.zoom))}));

        reflectionSurface.clear({0,0,0,0});
        reflectionSurface.draw(reflectionSprite);
        reflectionSurface.display();

        sprite.setOrigin({(TILE_WIDTH / 2) * camera.zoom, (TILE_HEIGHT / 2 - 1) * camera.zoom}); // -1 because of tile thickness
        sprite.setScale(sf::Vector2f(1.f, 1.f));
        setPosFromTileIdx(tile.getTileIdx(), camera);
        sprite.setTexture(textures[1], resetRect);
        sprite.setTextureRect(sf::IntRect(
            {0, 0},
            {static_cast<int>(round(TILE_WIDTH * camera.zoom)),
             static_cast<int>(round(TILE_HEIGHT * camera.zoom))}));

        worldPos globalLightVecScreen{ NormalToScreenVec(globalLightVec) };
        sf::Vector3f negScreenLightVec{
            -static_cast<float>(globalLightVecScreen.x),
            -static_cast<float>(globalLightVecScreen.y),
            -static_cast<float>(globalLightVecScreen.z)
        };

        m_shader.setUniform("texturee", sf::Shader::CurrentTexture);
        m_shader.setUniform("reflection", reflectionSurface.getTexture());
        m_shader.setUniform("negScreenLightDir", negScreenLightVec);
        m_shader.setUniform("x_origin", sprite.getPosition().x - camera.horizontal);
        m_shader.setUniform("y_origin", sprite.getPosition().y - camera.vertical);
        m_shader.setUniform("WATER_COLOR", static_cast<sf::Glsl::Vec4>(WATER_COLOR));
        m_shader.setUniform("WIDTH2", static_cast<float>(WIDTH2));
        m_shader.setUniform("TILE_WIDTH", static_cast<float>(TILE_WIDTH * camera.zoom));
        m_shader.setUniform("ZOOM", camera.zoom);
        m_shader.setUniform("timee", timePastForShader);

        m_shader.setUniform("TILE_SURFACE_WITDH", static_cast<float>(WIDTH2));
        m_shader.setUniform("TILE_SURFACE_HEIGHT", static_cast<float>(HEIGHT2));
        m_shader.setUniform("REFLECTION_WIDTH", static_cast<float>(TILE_WIDTH * camera.zoom));
        m_shader.setUniform("REFLECTION_HEIGHT", static_cast<float>(TILE_HEIGHT * camera.zoom));

        window.draw(sprite, &m_shader);
    }
    else
    {
        //bool active{ tile.isActive() };
        sprite.setOrigin({TILE_WIDTH / 2, TILE_HEIGHT / 2 - 1}); // -1 because of tile thickness 
        sprite.setScale(sf::Vector2f(camera.zoom, camera.zoom));
        setPosFromTileIdx(tile.getTileIdx(), camera);
        sprite.setTexture(textures[0], resetRect);
        

        window.draw(sprite);
    }
    
    if (tile.isHighlighted())
    {
        highlightSprite.setTexture(highlightTexture);
        highlightSprite.setScale(sf::Vector2f(camera.zoom, camera.zoom));
        window.draw(highlightSprite);
    }
}

void Tilesprite::updateSprite(Tile& tile, displayInput& camera)
{
    if (type == tileType::water)
        return;

    directionType direction{ tile.getDirection() };
    bool active{ tile.isActive() };

    setPosFromTileIdx(tile.getTileIdx(), camera);

    int activeIdx {};
    int textureIndex {};
    switch (GetTextureType(type))
    {
    case textureType::directional:
        textureIndex = frameIdx + 3 * 4 * static_cast<int>(tile.getDirection());
        break;

    case textureType::normal:
        textureIndex = frameIdx;
        break;

    default:
        break;
    }

    sprite.setTexture(textures[textureIndex]);
    sprite.setScale(sf::Vector2f(camera.zoom, camera.zoom));

    highlightSprite.setTexture(highlightTexture);
    highlightSprite.setScale(sf::Vector2f(camera.zoom, camera.zoom));
}

sf::Sprite& Tilesprite::getSprite()
{
    return sprite;
}

void Tilesprite::draw(sf::RenderWindow& window, Tile& tile)
{
    //window.draw(sprite);
    window.draw(sprite, &m_shader);
    if (tile.isHighlighted())
        window.draw(highlightSprite);
}
