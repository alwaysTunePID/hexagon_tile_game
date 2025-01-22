#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <iostream>
#include <math.h>
#include "WorldObjectSprite.h"
#include "Enums.h"
#include "Transformations.h"

WorldObjectSprite::WorldObjectSprite(WorldObjectType worldObjectType, int value)
    : objectType{ worldObjectType }, spellType{ SpellType::last }, effectType{ EffectType::last },
      visualType{ VisualType::last }, textures{}, m_normalTexture{},
      m_height{ static_cast<float>(GetSize(worldObjectType, value).h) },
      m_width{ static_cast<float>(GetSize(worldObjectType, value).w) },
      drawByPixel{ false }, animationLengths{}, sprite{}, m_shader{}, m_enableShaders{}, animationDataMap{}, pixelSprites{},
      pixelAnimationStep{ -1 }
{
    animationLengths.push_back(1);

    if (objectType == WorldObjectType::spell)
        spellType = static_cast<SpellType>(value);
    else if (objectType == WorldObjectType::visual)
        visualType = static_cast<VisualType>(value);

    std::string folderAndFile{ GetFolderAndFile(worldObjectType, value) };
    std::vector<std::string> dirStrings{ "" };
    if (GetTextureType(worldObjectType) == textureType::directional)
        dirStrings = {"1", "2", "3", "4"};
    else if (worldObjectType == WorldObjectType::grass)
        dirStrings = {"A", "B", "C"};

    sf::Texture texture{};
    for (auto& dirString : dirStrings)
    {
        // E.g. "Players/PlayerA/PlayerA1.png"
        std::string completePath{ "../../../resources/textures/" + folderAndFile + dirString + ".png" };
        texture.loadFromFile(completePath);
        textures.push_back(texture);

        if (worldObjectType == WorldObjectType::mountain)
        {
            completePath = "../../../resources/textures/" + folderAndFile + dirString + "Normal.png";
            m_normalTexture.loadFromFile(completePath);

            if (!m_shader.loadFromFile("../../../shaders/object.frag", sf::Shader::Fragment))
                std::cout << "ERROR: Couldn't load object.frag" << std::endl;
        }
    }

    sprite.setTexture(textures[0]);
    // TODO: How do we fix this?
    if (worldObjectType == WorldObjectType::player)
    {
        sprite.setOrigin(m_width / 2, m_height / 2 + 12); // Is this hardcoding ok? 
    }
    else if (worldObjectType == WorldObjectType::grass)
    {
        sprite.setOrigin(m_width / 2, m_height / 2 + 2); // Is this hardcoding ok? 
    }
    else if (worldObjectType == WorldObjectType::mountain)
    {
        sprite.setOrigin(m_width / 2, m_height - 12.5); // Is this hardcoding ok? 
    }
    else
    {
        sprite.setOrigin(m_width / 2, m_height / 2); // Is this hardcoding ok? 
    }
    sprite.setScale(sf::Vector2f(INIT_SCALE, INIT_SCALE));
}

WorldObjectSprite::WorldObjectSprite()
{
}

WorldObjectSprite::~WorldObjectSprite()
{
}

int WorldObjectSprite::getTextureIndexDirOffset(directionType dir)
{
    // TODO: This is hardcoded with the assumption that the animation is 3 frames
    switch (dir)
    {
    case directionType::up:
        return 0;
    case directionType::upLeft:
        return 3;
    case directionType::downLeft:
        return 6;
    case directionType::down:
        return 9;
    case directionType::downRight:
        return 6;
    case directionType::upRight:
        return 3;
    default:
        return 0;
    }
}

int WorldObjectSprite::dirToTextureIdx(directionType dir)
{
    switch (dir)
    {
    case directionType::up:
        return 0;
    case directionType::upLeft:
        return 1;
    case directionType::downLeft:
        return 2;
    case directionType::down:
        return 3;
    case directionType::downRight:
        return 2;
    case directionType::upRight:
        return 1;
    default:
        return 0;
    }
}

float WorldObjectSprite::getHFlipFactor(directionType dir)
{
    return (dir == directionType::downRight || dir == directionType::upRight) ? -1.f : 1.f;
}

// TODO: This is very much a copy of the function in Tilesprite. How do we solve this???
sf::Vector2f WorldObjectSprite::tileIdxToPos(TileIdx tileIdx, displayInput& camera)
{
    double spacing{ camera.tileSpacing ? 1.0 : 0.0 };

    double board_width{ 
        static_cast<double>(tileIdx.first) * (8 + spacing) * camera.zoom +
        static_cast<double>(tileIdx.second) * (40 + spacing) * camera.zoom };
    double board_height{
        static_cast<double>(tileIdx.first) * (20 + spacing) * camera.zoom +
        static_cast<double>(tileIdx.second) * (4 + spacing) * camera.zoom };

    return {
        static_cast<float>(camera.horizontal + board_width),
        static_cast<float>(camera.vertical + board_height) };
}

sf::Vector2f WorldObjectSprite::tileIdxAndPosInTileToPos(TileIdx tileIdx, PosInTile& posInTile, displayInput& camera)
{
    sf::Vector2f pos{ tileIdxToPos(tileIdx, camera) };

    pos.x += posInTile.first * camera.zoom;
    pos.y += posInTile.second * camera.zoom;

    return pos;
}

void WorldObjectSprite::setPos(castedSpellData& spellData, displayInput& camera)
{
    sf::Vector2f fromPos{ tileIdxToPos(spellData.fromTileIdx, camera) };
    sf::Vector2f toPos{ tileIdxToPos(spellData.toTileIdx, camera) };
    sf::Vector2f distDiff{ toPos - fromPos };
    sf::Vector2f traveledDiff{
        static_cast<float>(distDiff.x * spellData.traveledPerc / 100.0 ),
        static_cast<float>(distDiff.y * spellData.traveledPerc / 100.0) };

    sprite.setPosition(fromPos + traveledDiff );
    // TODO: Should this really be here?
    if (spellData.spellType != SpellType::teleport)
        sprite.setRotation(atan2(distDiff.y, distDiff.x) * 180 / PI + 90.0);
}

sf::Vector2f WorldObjectSprite::getPixelPos(unsigned int x, unsigned int y, displayInput& camera)
{
    // TODO: This implementation is strange. Can it be done better?
    sf::Vector2f spritePos{ sprite.getPosition() };
    spritePos.x -= sprite.getOrigin().x * camera.zoom;
    spritePos.y -= sprite.getOrigin().y * camera.zoom;
    sf::Vector2f pos{spritePos.x + static_cast<float>(x) * camera.zoom, spritePos.y + static_cast<float>(y) * camera.zoom};
    return pos;
}

void WorldObjectSprite::setRot(directionType dir)
{
    float angle{};
    switch (dir)
    {
    case directionType::up:
        angle = 0.0;
        break;
    case directionType::upLeft:
        angle = 315.0;
        break;
    case directionType::downLeft:
        angle = 225.0;
        break;
    case directionType::down:
        angle = 180.0;
        break;
    case directionType::downRight:
        angle = 135.0;
        break;
    case directionType::upRight:
        angle = 45.0;
        break;
    default:
        angle = 0.0;
        break;
    }
    sprite.setRotation(angle);
}

void WorldObjectSprite::setScale(castedSpellData& spellData, displayInput& camera)
{
    float scale{ 0.0 };
    if (spellData.traveledPerc < 20) {
        scale = static_cast<float>(1.0 - spellData.traveledPerc * 0.05);
        std::cout << "Scale:" << scale << std::endl; 
    }
    else if (spellData.traveledPerc > 80) {
        scale = static_cast<float>((spellData.traveledPerc - 80.0) * 0.05);
        std::cout << "Scale:" << scale << std::endl;
    }
    sprite.setScale(sf::Vector2f(scale * camera.zoom, scale * camera.zoom));
}

void WorldObjectSprite::updateFrameIdx()
{
    for (auto& [id, animationdata] : animationDataMap)
    {
        int animationLength{ animationLengths.at(animationdata.animationIdx) };
        bool isEndOfAnimation{ animationdata.frameIdx == (animationLength - 1) };

        // TODO: THIS SHIT doesn't work because Game isn't updated that the effect is deactivated
        // Deactivate effect?? that is active
        if (effectType == EffectType::spawn && animationdata.animationIdx && isEndOfAnimation)
        {
            animationdata.animationIdx = 0;
            animationdata.frameIdx = 0;
        }
        else
        {
            animationdata.frameIdx = isEndOfAnimation ? 0 : animationdata.frameIdx + 1;
        }
    }
}

void WorldObjectSprite::updateSprite(WorldObject& worldObject, displayInput& camera_in, worldPos& globalLightVec, bool reflected)
{
    directionType dir{ worldObject.getDir() };
    int textureIndex { dirToTextureIdx(dir) };
    sprite.setTexture(textures[textureIndex]);
    displayInput camera { reflected ? CAMERA_0 : camera_in };
    screenPos s_pos{ WorldToScreenPos(worldObject.getPos(), camera) };
    sprite.setPosition(s_pos.x, s_pos.y);
    float reflectionFlip{ reflected ? -1.f : 1.f };
    sprite.setScale(sf::Vector2f(getHFlipFactor(dir) * camera.zoom, reflectionFlip * camera.zoom));
    float rotation{ reflected ? -16.f : 0.f };
    sprite.setRotation(rotation);

    if (objectType == WorldObjectType::mountain)
    {
        sf::Vector3f negScreenLightVec{
            -static_cast<float>(globalLightVec.x),
            -static_cast<float>(globalLightVec.y),
            -static_cast<float>(globalLightVec.z)
        };

        m_shader.setUniform("texturee", sf::Shader::CurrentTexture);
        m_shader.setUniform("normalMap", m_normalTexture);
        m_shader.setUniform("negScreenLightDir", negScreenLightVec);
        m_enableShaders = camera_in.enableShaders;
    }
}

// TODO: Too much of a copy of the function above
void WorldObjectSprite::updateSprite(woCosmetic& cosmetic, displayInput& camera_in, worldPos& globalLightVec, bool reflected)
{
    sprite.setTexture(textures[cosmetic.value]);
    displayInput camera { reflected ? CAMERA_0 : camera_in };
    screenPos s_pos{ WorldToScreenPos(cosmetic.pos, camera) };
    sprite.setPosition(s_pos.x, s_pos.y);
    float reflectionFlip{ reflected ? -1.f : 1.f };
    sprite.setScale(sf::Vector2f(camera.zoom, reflectionFlip * camera.zoom));
    float rotation{ reflected ? -16.f : 0.f };
    sprite.setRotation(rotation);

    if (objectType == WorldObjectType::mountain)
    {
        sf::Vector3f negScreenLightVec{
            -static_cast<float>(globalLightVec.x),
            -static_cast<float>(globalLightVec.y),
            -static_cast<float>(globalLightVec.z)
        };

        m_shader.setUniform("texturee", sf::Shader::CurrentTexture);
        m_shader.setUniform("normalMap", m_normalTexture);
        m_shader.setUniform("negScreenLightDir", negScreenLightVec);
        m_enableShaders = camera_in.enableShaders;
    }
}

void WorldObjectSprite::updateSprite(worldPos w_pos, displayInput& camera)
{
    sprite.setTexture(textures[0]);
    sprite.setOrigin(m_width / 2, m_height / 2);

    screenPos s_pos{ WorldToScreenPos(w_pos, camera) };
    sprite.setPosition(s_pos.x, s_pos.y);
    sprite.setScale(sf::Vector2f(camera.zoom, camera.zoom));
}

// TODO: Remove elements from animationDataMap at some point???
/* void WorldObjectSprite::updateSprite(int id, TileIdx tileIdx, PosInTile pos, directionType dir, displayInput& camera)
{
    animationData& animationdata{ getAnimationData(id) };

    // Begin active animation
    if ( effect.active && (animationdata.animationIdx != 1) )
    {
        animationdata.animationIdx = 1;
        animationdata.frameIdx = 0;
    }
    if (spellData.spellType == SpellType::teleport) {
        updateTeleportAnimation(spellData, camera);
    }
    // TODO: This will not work when having more than 2 animations
    int offset{ animationdata.animationIdx ? animationLengths.at(0) : 0 };
    int dirOffset { getTextureIndexDirOffset(dir) };
    int textureIndex{ offset + dirOffset + animationdata.frameIdx };
    sprite.setTexture(textures[textureIndex]);

    sprite.setOrigin(m_width / 2, m_height / 2);

    if (worldObjectType == WorldObjectType::player)
        pos.second -= 14.f;

    sprite.setPosition(tileIdxAndPosInTileToPos(tileIdx, pos, camera));
    // TODO: You must add a way to differentiate between the two directional types.
    if (worldObjectType != WorldObjectType::player)
        setRot(dir);
    int horizontalFlip{ (dir == directionType::downRight || dir == directionType::upRight) ? -1 : 1 };
    sprite.setScale(sf::Vector2f(horizontalFlip * camera.zoom, camera.zoom));
    pixelSprites.clear();
    pixelAnimationStep = -1;
    drawByPixel = false;
}*/

void WorldObjectSprite::addInstance(int id, animationData animationdata)
{
    animationDataMap.insert({id, animationdata});
}

animationData& WorldObjectSprite::getAnimationData(int id)
{
    if (animationDataMap.find(id) == animationDataMap.end()) {
        // Couldn't find it so add it
        animationDataMap.insert({ id, {0, 0} });
    }
    return animationDataMap[id];
}

void WorldObjectSprite::draw(sf::RenderWindow& window)
{
    if (drawByPixel) {
        for (auto& pixel : pixelSprites)
        {
            window.draw(pixel.pixelShape);
        }
        return;
    }

    if (m_enableShaders)
        window.draw(sprite, &m_shader);
    else
        window.draw(sprite);
}

void WorldObjectSprite::draw(sf::RenderTexture& surface)
{
    surface.draw(sprite, &m_shader);
}

// Move this to a new file??
void WorldObjectSprite::updateTeleportAnimation(castedSpellData& spellData, displayInput& camera)
{
    if (pixelSprites.empty())
    {
        sf::Image image = sprite.getTexture()->copyToImage();
        for (unsigned int y = 0; y < m_height; y++)
        {
            for (unsigned int x = 0; x < m_width; x++)
            {
                sf::Color color = image.getPixel(x,y);
                // Skip transparent pixels
                if (color.a == 0)
                    continue;
                sf::RectangleShape pixelShape(sf::Vector2f(1, 1));
                pixelShape.setFillColor(color);
                //pixelShape.setOrigin(0.5, 0.5);
                pixelShape.setPosition(getPixelPos(x, y, camera));
                pixelShape.setScale(sf::Vector2f(camera.zoom, camera.zoom));

                sf::Vector2f originDiff{ pixelShape.getPosition() - sprite.getPosition() };
                float radius{ static_cast<float>(std::sqrt(std::pow(originDiff.x, 2) + std::pow(originDiff.y, 2))) };
                float angle{ static_cast<float>(atan2(originDiff.y, originDiff.x)) };
                std::vector<sf::Vector2f> relPosHistory{ originDiff };

                //std::cout << "radius: " << radius << "angle: " << angle << std::endl;

                pixelData pixel{pixelShape, pixelShape.getPosition(), radius, angle, relPosHistory};
                pixelSprites.push_back(pixel);
            }   
        }
        pixelAnimationStep = 0;
    }
    else
    {
        //std::cout << "Traveled perc: " << spellData.traveledPerc << std::endl;

        sf::Vector2f origin{ sprite.getPosition() };
        float r_max{ 18.f * camera.zoom };
        float r_largest{ 0.f };
        bool printOnce{ true };
        for (auto& pixel : pixelSprites)
        {
            sf::Color color{ pixel.pixelShape.getFillColor() };
            sf::Vector2f relPos{};

            switch (pixelAnimationStep)
            {
            case 0:
                pixel.angle = pixel.angle + (r_max - pixel.radius) * 0.003f;
                pixel.radius = pixel.radius - (r_max - pixel.radius) / 40.f;
                if (pixel.radius < 0.f)
                {
                    pixel.radius = 0.f;
                    pixel.angle = PI/2.f;
                    color.a = 0;
                    pixel.pixelShape.setFillColor(color);
                }
                if (pixel.radius > r_largest)
                    r_largest = pixel.radius; 
                
                relPos = {
                    pixel.radius * std::cos(pixel.angle),
                    pixel.radius * std::sin(pixel.angle)
                };

                pixel.relPosHistory.push_back(relPos);
                drawByPixel = true;
                break;
            
            case 1:
                relPos = pixel.relPosHistory.back();
                pixel.relPosHistory.pop_back();

                if (printOnce)
                {
                    std::cout << "x: " << origin.x + relPos.x << " " << origin.y + relPos.y << std::endl;
                    printOnce = false;
                }

                if (pixel.relPosHistory.empty())
                    pixelAnimationStep = -1;

                color.a = 255;
                pixel.pixelShape.setFillColor(color);
                drawByPixel = true;
                break;
            
            default:
                drawByPixel = false;
                break;
            }

            float new_x{ origin.x + relPos.x };
            float new_y{ origin.y + relPos.y };

            pixel.pixelShape.setPosition(new_x, new_y);
            pixel.pixelShape.setScale(sf::Vector2f(camera.zoom, camera.zoom));
        }
        // State switch condition
        if (r_largest <= 0.f )
        {
            std::cout << "To Tile: " << spellData.toTileIdx.first << " " << spellData.toTileIdx.first << std::endl;
            sprite.setPosition(tileIdxToPos(spellData.toTileIdx, camera));
            pixelAnimationStep = 1;
        }
    }
}
