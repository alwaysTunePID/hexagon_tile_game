#ifndef ENTITYSPRITE_H
#define ENTITYSPRITE_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "WorldObject.h"
#include "Enums.h"

typedef struct animationData {
    int animationIdx;
    int frameIdx;
} animationData;

typedef struct pixelData {
    sf::RectangleShape pixelShape;
    sf::Vector2f startPos;
    float radius;
    float angle;
    std::vector<sf::Vector2f> relPosHistory;
} pixelData;

// For grass
typedef struct woCosmetic {
    int value;
    worldPos pos;
} woCosmetic;

class WorldObjectSprite
{
private:
    WorldObjectType objectType;
    SpellType spellType;
    EffectType effectType;
    VisualType visualType;
    std::vector<sf::Texture> textures;
    sf::Texture m_normalTexture;
    float m_height;
    float m_width;
    bool drawByPixel;
    std::vector<int> animationLengths;

    sf::Sprite sprite;
    sf::Shader m_shader;
    bool m_enableShaders;
    std::map<int, animationData> animationDataMap;
    // Must these two be moved into animationDataMap to handle instances??
    std::vector<pixelData> pixelSprites;
    int pixelAnimationStep;

public:
    WorldObjectSprite(WorldObjectType worldObjectType, int value);
    WorldObjectSprite();
    ~WorldObjectSprite();

    int getTextureIndexDirOffset(directionType dir);
    int dirToTextureIdx(directionType dir);
    float getHFlipFactor(directionType dir);
    sf::Vector2f tileIdxToPos(TileIdx tileIdx, displayInput& camera);
    sf::Vector2f tileIdxAndPosInTileToPos(TileIdx tileIdx, PosInTile& posInTile, displayInput& camera);
    void setPos(castedSpellData& spellData, displayInput& camera);
    sf::Vector2f getPixelPos(unsigned int x, unsigned int y, displayInput& camera);
    void setRot(directionType dir);
    void setScale(castedSpellData& spellData, displayInput& camera);
    void updateFrameIdx();
    void updateSprite(WorldObject& worldObject, displayInput& camera, worldPos& globalLightVec, bool reflected);
    void updateSprite(woCosmetic& cosmetic, displayInput& camera_in, worldPos& globalLightVec, bool reflected);
    void updateSprite(worldPos w_pos, displayInput& camera);
    void addInstance(int id, animationData animationdata);
    animationData& getAnimationData(int id);
    void draw(sf::RenderWindow& window);
    void draw(sf::RenderTexture& surface);
    // Move this to another file?
    void updateTeleportAnimation(castedSpellData& spellData, displayInput& camera);

};

#endif