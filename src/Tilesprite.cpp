#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <iostream>
#include "Tile.h"
#include "Tilesprite.h"
#include "Enums.h"

Tilesprite::Tilesprite(Tile& tile)
    : sprite{}, highlightSprite{}
{
    type = tile.getTileType();

    std::string tileStr{ ToString(type) };
    std::string imagePath{ "../../../resources/textures/" + tileStr + "/"};

    std::string completePath{};
    sf::Texture texture{};
    int m{ 0 };

    switch (GetTextureType(type))
    {
    case textureType::directional:
        m = 3; // 3 * 6;
        break;

    default:
        m = 3;
        break;
    }

    for (int j = 0; j < m; j++) {
        completePath = (imagePath + tileStr + std::to_string(j + 1) + ".png");
        texture.loadFromFile(completePath);
        textures.push_back(texture);
    }

    sprite.setTexture(textures[0]);
    sprite.setScale(sf::Vector2f(INIT_SCALE, INIT_SCALE));

    // Highlight tile
    std::string imagePath2{ "../../../resources/textures/HighlightTile/HighlightTile.png" };
    highlightTexture.loadFromFile(imagePath2);
    highlightSprite.setTexture(highlightTexture);
    highlightSprite.setScale(sf::Vector2f(INIT_SCALE, INIT_SCALE));
}

Tilesprite::Tilesprite()
{
}

Tilesprite::~Tilesprite()
{
}

void Tilesprite::setPosFromTileIdx(TileIdx tileIdx, displayInput& camera)
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

void Tilesprite::updateFrameIdx()
{
    frameIdx = (frameIdx == 2) ? 0 : frameIdx + 1;
}

void Tilesprite::updateSprite(Tile& tile, displayInput& camera)
{
    directionType direction{ tile.getDirection() };
    bool active{ tile.isActive() };

    setPosFromTileIdx(tile.getTileIdx(), camera);

    int activeIdx {};
    int textureIndex {};
    switch (GetTextureType(type))
    {
    case textureType::directional:
        textureIndex = frameIdx + 3 * tile.getMoveIdx() + 3 * 4 * static_cast<int>(tile.getDirection());
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
    window.draw(sprite);
    if (tile.isHighlighted())
        window.draw(highlightSprite);
}
