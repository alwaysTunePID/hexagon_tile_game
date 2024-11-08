#ifndef ENTITYSPRITE_H
#define ENTITYSPRITE_H

#include <vector>
#include <SFML/Graphics.hpp>
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

class EntitySprite
{
private:
    EntityType entityType;
    SpellType spellType;
    EffectType effectType;
    VisualType visualType;
    std::vector<sf::Texture> textures {};
    float height_;
    float width_;
    bool drawByPixel;
    std::vector<int> animationLengths;

    sf::Sprite sprite;
    std::map<int, animationData> animationDataMap;
    // Must these two be moved into animationDataMap to handle instances??
    std::vector<pixelData> pixelSprites;
    int pixelAnimationStep;

public:
    EntitySprite(EntityType entityType, int value);
    EntitySprite();
    ~EntitySprite();

    int getTextureIndexDirOffset(directionType dir);
    sf::Vector2f tileIdxToPos(TileIdx tileIdx, displayInput& camera);
    sf::Vector2f tileIdxAndPosInTileToPos(TileIdx tileIdx, PosInTile& posInTile, displayInput& camera);
    void setPos(castedSpellData& spellData, displayInput& camera);
    sf::Vector2f getPixelPos(unsigned int x, unsigned int y, displayInput& camera);
    void setRot(directionType dir);
    void setScale(castedSpellData& spellData, displayInput& camera);
    void updateFrameIdx();
    void updateSprite(int id, TileIdx tileIdx, PosInTile pos, directionType dir, displayInput& camera);
    void updateSprite(int id, worldPos w_pos, directionType dir, displayInput& camera); // New, with worldPos
    void updateSprite(worldPos w_pos, displayInput& camera);
    void updateSprite(int id, effectData& effect, TileIdx tileIdx, displayInput& camera);
    void updateSprite(castedSpellData& spellData, displayInput& camera);
    void addInstance(int id, animationData animationdata);
    animationData& getAnimationData(int id);
    void draw(sf::RenderWindow& window);
    // Move this to another file?
    void updateTeleportAnimation(castedSpellData& spellData, displayInput& camera);

};

#endif