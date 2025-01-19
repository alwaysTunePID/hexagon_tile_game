#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <map>
#include <mutex>
#include "Game.h"
#include "Tile.h"
#include "Tilesprite.h"
#include "WorldObjectSprite.h"

class Graphics
{
private:
    sf::Clock clock;
    std::map<std::pair<WorldObjectType, int>, WorldObjectSprite*> worldObjectSprites;
    std::map<tileType, Tilesprite*> tilesprites;
    sf::Font font;
    displayInput camera;
    sf::RenderTexture m_reflectionSurface;
    sf::RenderTexture m_tileSurface;
    timePoint m_initTime;
    float m_timePastForShader;
    worldPos m_globalLightVec;

public:
    Graphics();
    ~Graphics();

    void update(Game& game, sf::RenderWindow& window, displayInput& zoom, sf::Uint8 callerId, double dt);
    void drawTiles(Game& game, sf::RenderWindow& window, sf::Sprite& reflectionSprite);
    void sortWorldObjects(Game& game, std::vector<WorldObject>& worldObjects);
    void drawReflections(Game& game);
    void drawWorldObjects(Game& game, sf::RenderWindow& window);
    void drawPlayers(Game& game, sf::RenderWindow& window);
    void drawCoordinateSystem(sf::RenderWindow& window);
    void drawPlayerGUI(Game& game, sf::RenderWindow& window, sf::Uint8 callerId);
    void updateCamera(displayInput& input);
    void drawInventory(Player& player, sf::RenderWindow& window);
    WorldObjectSprite* getWorldObjectSprite(WorldObjectType objectType, int value);
    Tilesprite* getTilesprite(Tile& tile);
    void updateGlobalLightVec(double timePast);
    void updateGlobalLightVec2(double timePast);
};

#endif