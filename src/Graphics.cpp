#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "Graphics.h"

Graphics::Graphics()
    : clock{}, entitySprites{}, tilesprites{}, playersprites{},
      font{}, camera{ INIT_SCALE, WIDTH2 / 2, HEIGHT2 / 2 }
{
    font.loadFromFile("../../../resources/fonts/PixCon.ttf");
}

Graphics::~Graphics()
{
}

void Graphics::update(Game& game, sf::RenderWindow& window, displayInput& input)
{
    window.clear();

    sf::RectangleShape background(sf::Vector2f(WIDTH2, HEIGHT2));
    sf::Color backgroundColor{ 21, 24, 31 };
    background.setFillColor(backgroundColor);
    window.draw(background);

    updateCamera(input);

    /*sf::RectangleShape board(sf::Vector2f(COLUMNS * SQ_WIDTH, ROWS * SQ_WIDTH));
    board.setPosition({
        WIDTH2 / 2 - (static_cast<int>(COLUMNS) / 2) * static_cast<int>(SQ_WIDTH),
        HEIGHT2 / 2 - (static_cast<int>(ROWS) / 2) * static_cast<int>(SQ_WIDTH) });
    board.setFillColor(sf::Color::Black);
    window.draw(board);*/

    float lapTime = clock.getElapsedTime().asSeconds();
    bool elapsed = (lapTime > 0.25f) ? true : false;
    if (elapsed)
    {
        clock.restart();

        for (auto& [pairShit, entitySprite] : entitySprites)
            entitySprite.updateFrameIdx();

        for (auto& [tiletype, tilesprite] : tilesprites)
            tilesprite.updateFrameIdx();
        
        for (auto& [tileId, playersprite] : playersprites)
            playersprite.updateFrameIdx();
    }

    //TODO: FIX THIS HARDCODED 
    int boardSize = 8;

    for (int i = 0; i <= boardSize; i++)
    {
        int r{ i - boardSize / 2 };

        for (int ii = 0; ii <= boardSize; ii++)
        {
            int c{ ii - boardSize / 2 };

            TileIdx tileIdx{ r, c };
            if (game.isOutOfBounds(tileIdx))
                continue;

            // Optimize this shit!!
            for (auto& [tileId, tile] : game.getTiles())
            {
                if (tile.getTileIdx().first == r && tile.getTileIdx().second == c)
                {
                    Tilesprite& tilesprite{ getTilesprite(tile) };
                    tilesprite.updateSprite(tile, camera);
                    tilesprite.draw(window, tile);

                    // Draw players on tile
                    drawPlayersOnTile(game, window, tile);

                    // Draw effects
                    for (effectData& effect : tile.getProperties())
                    {
                        if (HasTexture(effect.type))
                        {
                            int value{ static_cast<int>(effect.type) };
                            EntitySprite& entitySprite{ getEntitySprite(EntityType::effect, value) };
                            entitySprite.updateSprite(tile.getId(), effect, tile.getTileIdx(), camera);
                            entitySprite.draw(window);
                        }
                        // Is this really the way to do it?
                        if (effect.type == EffectType::spawn)
                            effect.active = false;
                    }
                }
            }
        }
    }

    for (castedSpellData& castedSpell : game.getCastedSpells())
    {
        if (castedSpell.spellType == SpellType::teleport) {
            continue;
        }
        else {
            int value{ static_cast<int>(castedSpell.spellType) };
            EntitySprite& entitySprite{ getEntitySprite(EntityType::spell, value) };
            entitySprite.updateSprite(castedSpell, camera);
            entitySprite.draw(window);
        }
    }

    drawPlayerGUI(game, window);

    window.display();
}


void Graphics::drawPlayersOnTile(Game& game, sf::RenderWindow& window, Tile& tile)
{
    for (auto& [id, player] : game.getPlayers())
    {
        if (player.getTileIdx() == tile.getTileIdx())
        {
            // Player characters on board
            EntitySprite& playersprite{ getEntitySprite(EntityType::player, player.getId()) };
            if (player.isSpellOngoing()) {
                for (castedSpellData& castedSpell : game.getCastedSpells())
                {
                    if (castedSpell.playerId == player.getId())
                        playersprite.updateSprite(castedSpell, camera);
                }
            }
            else {
                //TODO: Change this when players have a direction
                playersprite.updateSprite(player.getId(), player.getTileIdx(), directionType::none, camera);
            }
            playersprite.draw(window);
        }
    }
}

void Graphics::drawPlayerGUI(Game& game, sf::RenderWindow& window)
{
    for (auto& [id, player] : game.getPlayers())
    {
        // Need a box / canvas to draw text to ??
        std::string hi{ "Hello" };
        sf::Text texttt{ hi, font };
        // set the character size
        texttt.setCharacterSize(24); // in pixels, not points!
        float turnTimer{ player.getTurnTime() };
        std::string turnTimerStr{ std::to_string(static_cast<int>(turnTimer)) };
        std::string completeStr{};
        float xText{ WIDTH2 - 200 };
        float yText{ HEIGHT2 / 2 };

        if (id == 0)
        {
            yText -= SQ_WIDTH * 2;
        }
        else
        {
            yText += SQ_WIDTH;
        }

        completeStr += player.getName();
        completeStr += ("\n WINS: " + std::to_string(player.getPoints()));
        completeStr += ("\n TIME: " + turnTimerStr);
        texttt.setString(completeStr);

        texttt.setPosition(sf::Vector2f(xText, yText));
        // set the color
        sf::Color gray{ 255, 255, 255, 100 };
        sf::Color textColor{ player.isCurrentPlayer() ? sf::Color::White : gray };
        texttt.setColor(textColor);

        window.draw(texttt);

        //TODO: This will not work when multiplayer
        if (player.isCurrentPlayer())
        {
            // Draw aim arrow
            if (player.isLJoyMode(LJoyMode::aim))
            {
                if (IsDirectional(player.getSelectedSpell()))
                {
                    TileIdx tileIdx{ player.getAimTileIdx() };
                    directionType direction{ player.getSelectedSpellDir() };

                    int value{ static_cast<int>(VisualType::aimDir) };
                    EntitySprite& entitySprite{ getEntitySprite(EntityType::visual, value) };
                    entitySprite.updateSprite(0, tileIdx, direction, camera);
                    entitySprite.draw(window);
                }
            }
            drawInventory(player, window);
        }
    }
}



void Graphics::updateCamera(displayInput& input)
{
    if (std::abs(input.zoom) > 1.0)
    {
        camera.zoom += input.zoom / 500.0;
        if (camera.zoom > 20.0)
            camera.zoom = 20.0;
        else if (camera.zoom < 1.0)
            camera.zoom = 1.0;
    }
    if (std::abs(input.horizontal) > 5.0)
    {
        camera.horizontal -= input.horizontal / 2.0;
        if (camera.horizontal > 2000.0 * camera.zoom)
            camera.horizontal = 2000.0 * camera.zoom;
        else if (camera.horizontal < 0.0)
            camera.horizontal = 0.0;
    }
    if (std::abs(input.vertical) > 5.0)
    {
        camera.vertical -= input.vertical / 2.0;
        if (camera.vertical > 1100.0 * camera.zoom)
            camera.vertical = 1100.0 * camera.zoom;
        else if (camera.vertical < 0.0)
            camera.vertical = 0.0;
    }

    // Toggle tile spacing
    camera.tileSpacing = (input.tileSpacing) ? !camera.tileSpacing : camera.tileSpacing;
}

void Graphics::drawInventory(Player& player, sf::RenderWindow& window)
{
    if (player.isLJoyMode(LJoyMode::aim))
    {
        std::string hi{ "Hello" };
        sf::Text texttt{ hi, font };
        // set the character size
        texttt.setCharacterSize(24); // in pixels, not points!
        sf::Color gray{ 255, 255, 255, 100 };
        std::string completeStr{};
        float xText{ 20 };
        float yText{ 7 * HEIGHT2 / 8 };
        int idx{ 0 };

        for (SpellType spell : player.getSelectionSpells())
        {
            completeStr = ToString(spell);
            texttt.setString(completeStr);
            texttt.setPosition(sf::Vector2f(xText, yText));
            sf::Color textColor{ (player.getSelectedSpellIdx() == idx) ? sf::Color::White : gray };
            texttt.setColor(textColor);
            window.draw(texttt);
            yText += 30;
            idx++;
        }
    }
}

EntitySprite& Graphics::getEntitySprite(EntityType entitytype, int value)
{
    std::pair<EntityType, int> key{ entitytype, value };
    if (entitySprites.find(key) == entitySprites.end()) {
        // Couldn't find sprite so add it
        entitySprites.insert({ key, EntitySprite(entitytype, value) });
    }
    return entitySprites[key];
}

Tilesprite& Graphics::getTilesprite(Tile& tile)
{
    if (tilesprites.find(tile.getTileType()) == tilesprites.end()) {
        // Couldn't find sprite so add it
        tilesprites.insert({tile.getTileType(), Tilesprite(tile)});
    }
    return tilesprites[tile.getTileType()];
}