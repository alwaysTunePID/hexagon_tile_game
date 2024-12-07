#ifndef TILESPRITE_H
#define TILESPRITE_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "Tile.h"

const sf::Color WATER_COLOR{ 48, 117, 239, 255 };

class Tilesprite
{
private:
    std::vector<sf::Texture> textures;
    sf::Texture highlightTexture;
    // TODO: This must be moved to tile class instead
    int frameIdx;
    sf::ConvexShape tileShape;
    sf::RenderTexture m_tileSurface;

    sf::Sprite sprite;
    sf::Sprite highlightSprite;
    tileType type;

    sf::Shader m_shader;

public:
    Tilesprite(Tile& tile);
    Tilesprite();
    ~Tilesprite();

    void setPosFromTileIdxLegacy(TileIdx tileIdx, displayInput& camera);
    void setPosFromTileIdx(TileIdx tileIdx, displayInput& camera);
    void updateFrameIdx();
    void updateWaterSurfaceTexture(Tile& tile, displayInput& camera);
    void updateAndDraw(sf::RenderWindow& window, Tile& tile, displayInput& camera, sf::Sprite& reflectionSprite, sf::RenderTexture& tileSurface, float timePast);
    void updateSprite(Tile& tile, displayInput& camera);
    sf::Sprite& getSprite();
    void draw(sf::RenderWindow& window, Tile& tile);

};

#endif