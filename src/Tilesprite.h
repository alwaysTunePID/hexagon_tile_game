#ifndef TILESPRITE_H
#define TILESPRITE_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "Tile.h"

class Tilesprite
{
private:
    std::vector<sf::Texture> textures {};
    sf::Texture highlightTexture;
    // TODO: This must be moved to tile class instead
    int frameIdx {0};

    sf::Sprite sprite;
    sf::Sprite highlightSprite;
    tileType type;

public:
    Tilesprite(Tile& tile);
    Tilesprite();
    ~Tilesprite();

    void setPosFromTileIdx(TileIdx tileIdx, displayInput& camera);
    void updateFrameIdx();
    void updateSprite(Tile& tile, displayInput& camera);
    sf::Sprite& getSprite();
    void draw(sf::RenderWindow& window, Tile& tile);

};

#endif