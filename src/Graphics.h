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

// Configure number of particles in the system
const int NUM_PARTICLES = 10*256; // Needs to be even work group size

// Particle data structure
struct Particle {
    float x_n, y_n, z_n, w_n; // Position [-1, 1] + padding
    float x, y, z, w;         // Simulation local position + padding
    float vx, vy, vz, vw;     // Simulation local Velocity + padding
    float lifeTime, totalLifeTime, activated, p3;
};

class Graphics
{
private:
    sf::Clock clock;
    std::map<std::pair<WorldObjectType, int>, WorldObjectSprite*> worldObjectSprites;
    std::map<tileType, Tilesprite*> tilesprites;
    std::map<int, std::vector<woCosmetic>> tileCosmetics;
    WorldObjectSprite grass;
    sf::Font font;
    displayInput camera;
    sf::RenderTexture m_reflectionSurface;
    sf::RenderTexture m_tileSurface;
    timePoint m_initTime;
    float m_timePastForShader;
    worldPos m_globalLightVec;
    std::default_random_engine generator;
    bool particleSimOngoing;
    GLuint particleBuffer;
    GLuint fadeProgram;
    GLuint computeProgram;
    GLuint renderProgram;
    GLuint vao;
    GLuint prevPosTexture;
    GLuint trailTexture;
    GLuint psTexture;
    GLuint quadVAO;
    GLuint quadVBO;
    sf::Vector2f trailTextureRes;
    sf::Vector2f trailTexturePos;
    sf::Vector2f cOrigin;
    sf::CircleShape circle;

public:
    Graphics(unsigned seed);
    ~Graphics();

    void update(Game& game, sf::RenderWindow& window, displayInput& zoom, uint8_t callerId, double dt);
    void drawTiles(Game& game, sf::RenderWindow& window, sf::Sprite& reflectionSprite);
    void sortWorldObjects(Game& game, std::vector<WorldObject>& worldObjects);
    void drawReflections(Game& game);
    void drawWorldObjects(Game& game, sf::RenderWindow& window);
    void drawPlayers(Game& game, sf::RenderWindow& window);
    void drawCoordinateSystem(sf::RenderWindow& window);
    void drawPlayerGUI(Game& game, sf::RenderWindow& window, uint8_t callerId);
    void updateCamera(displayInput& input);
    void drawInventory(Player& player, sf::RenderWindow& window);
    WorldObjectSprite* getWorldObjectSprite(WorldObjectType objectType, int value);
    Tilesprite* getTilesprite(Tile& tile);
    std::vector<woCosmetic>* getTileCosmetics(Tile& tile);
    void updateGlobalLightVec(double timePast);
    void updateGlobalLightVec2(double timePast);
    //void createParticleSimulation();
    void InitQuad();
    void InitParticles();
    void updateAndDrawParticleSimulation(sf::RenderWindow& window, displayInput& input, double dt);
    // Temp here
    void FadeParticleTrail(float deltaTime);
    void UpdateParticles(float deltaTime);
    void RenderParticles();
    // Shader loading utility
    GLuint LoadShader(const std::string& filePath, GLenum type);
    GLuint CreateComputeShader(const std::string& filePath);
    GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader);
};

#endif