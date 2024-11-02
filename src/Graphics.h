#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <map>
#include <mutex>
#include "Game.h"
#include "Tile.h"
#include "Tilesprite.h"
#include "EntitySprite.h"

class Graphics
{
private:
    sf::Clock clock;
    std::map<std::pair<EntityType, int>, EntitySprite> entitySprites;
    std::map<tileType, Tilesprite> tilesprites;
    std::map<int, Tilesprite> playersprites;
    sf::Font font;
    displayInput camera;

public:
    Graphics();
    ~Graphics();

    void update(Game& game, sf::RenderWindow& window, displayInput& zoom, double dt);
    void drawPlayersOnTile(Game& game, sf::RenderWindow& window, Tile& tile);
    void drawPlayers(Game& game, sf::RenderWindow& window);
    void drawCoordinateSystem(sf::RenderWindow& window);
    void drawPlayerGUI(Game& game, sf::RenderWindow& window);
    void updateCamera(displayInput& input);
    void drawInventory(Player& player, sf::RenderWindow& window);
    EntitySprite& getEntitySprite(EntityType entitytype, int value);
    Tilesprite& getTilesprite(Tile& tile);
};

#endif