#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <math.h>
#include "Enums.h"
#include "ConfigParser.h"
#include "Game.h"
#include "Graphics.h"

int main()
{
    double joyThreshHigh{ 0.0 };
    double joyThreshLow{ 0.0 };
    unsigned seed { 2952795 };

    std::string configPath{ "../../../config.txt" };

    bool enableDebugPrint{ false };
    bool showCoordinateSystem{ false };
    bool enableShaders{ false };

    auto window = sf::RenderWindow { {WIDTH2, HEIGHT2}, "Some Game" };
    window.setFramerateLimit(144);
    window.setVerticalSyncEnabled(true);

    ConfigParser configParser{ configPath };
    configParser.loadConfig(joyThreshHigh, joyThreshLow, seed);

    Game game{seed};
    Graphics graphics{};


    game.createPlayer();
    game.createPlayer();

    actionType begunAction{ actionType::none };
    int tileSpacingIgnoreCounter{ 0 };

    timePoint startTime{ Time::now() };
    timePoint endTime{};

    while (window.isOpen())
    {
        moveInput move{};
        gameInput input{ xbox::none, actionType::none, move };
        displayInput dispInput{};

        float posX{};
        float posY{};
        float posZ{};
        float posR{};
        float posU{};
        float posV{};

        bool xboxConnected = sf::Joystick::isConnected(0);

        // How many buttons does joystick #0 support?
        //unsigned int buttons = sf::Joystick::getButtonCount(0);

        /*
        // Does joystick #0 define a X axis?  
        bool hasX = sf::Joystick::hasAxis(0, sf::Joystick::X);
        
        // Is button #2 pressed on joystick #0?
        bool pressed = sf::Joystick::isButtonPressed(0, 2);
        
        // What's theif (pos.second < k * pos.first + m) current position of the Y axis on joystick #0?
        float position = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);*/

        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            /*if (std::pow(posX, 2) + std::pow(posY, 2) > joyThreshHigh)
            {
                float result = atan2(posY, posX) * 180 / PI;
                if (std::abs(result + 90.0) < 30.0)
                {
                    //std::cout << "Up" << std::endl;
                    begunAction = actionType::up;
                }
                if (std::abs(result + 150.0) < 30.0)
                {
                    //std::cout << "UpLeft" << std::endl;
                    begunAction = actionType::upLeft;
                }
                else if (std::abs(result - 150.0) < 30.0)
                {
                    //std::cout << "DownLeft" << std::endl;
                    begunAction = actionType::downLeft;
                }
                else if (std::abs(result - 90.0) < 30.0)
                {
                    //std::cout << "Down" << std::endl;
                    begunAction = actionType::down;
                }
                else if (std::abs(result - 30.0) < 30.0)
                {
                    //std::cout << "DownRight" << std::endl; 
                    begunAction = actionType::downRight;
                }
                else if (std::abs(result + 30.0) < 30.0)
                {
                    //std::cout << "UpRight" << std::endl;
                    begunAction = actionType::upRight;
                }
            }
            else if (std::pow(posX, 2) + std::pow(posY, 2) < joyThreshLow)
            {
                if (begunAction != actionType::none)
                {
                    input.action = begunAction;
                    begunAction = actionType::none;
                }
            }*/

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
            {
                input.action = actionType::undoMove;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
            {
                input.action = actionType::skipTurn;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
            {
                std::cout << "Pressed P " << std::endl;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            {
                posZ = -160.f;
            }
            else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            {
                posZ = 160.f;
            }
            else if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::C))
            {
                if (showCoordinateSystem)
                {
                    showCoordinateSystem = false;
                    std::cout << "Hide Coordinate System" << std::endl;
                }
                else
                {
                    std::cout << "Show Coordinate System" << std::endl;
                    showCoordinateSystem = true;
                }
            }
            else if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::V))
            {
                if (enableShaders)
                {
                    std::cout << "Disable Shaders" << std::endl;
                    enableShaders = false;
                }
                else
                {
                    std::cout << "Enable Shaders" << std::endl;
                    enableShaders = true;
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
            {
                configParser.loadConfig(joyThreshHigh, joyThreshLow, seed);
                std::cout << joyThreshHigh << " " << joyThreshLow << std::endl;
            }
            else if (sf::Joystick::isButtonPressed(0, static_cast<unsigned int>(xbox::A)))
            {
                input.button = xbox::A;
                std::cout << "A" << std::endl;
            }
            else if (sf::Joystick::isButtonPressed(0, static_cast<unsigned int>(xbox::B)))
            {
                input.button = xbox::B;
                std::cout << "B" << std::endl;
            }
            else if (sf::Joystick::isButtonPressed(0, static_cast<unsigned int>(xbox::X)))
            {
                input.button = xbox::X;
                std::cout << "X" << std::endl;
            }
            else if (sf::Joystick::isButtonPressed(0, static_cast<unsigned int>(xbox::Y)))
            {
                input.button = xbox::Y;
                std::cout << "Y" << std::endl;
                enableDebugPrint = enableDebugPrint ? false : true;
            }
            else if (sf::Joystick::isButtonPressed(0, static_cast<unsigned int>(xbox::LB)))
            {
                input.button = xbox::LB;
                std::cout << "LB" << std::endl;
            }
            else if (sf::Joystick::isButtonPressed(0, static_cast<unsigned int>(xbox::RB)))
            {
                input.button = xbox::RB;
                std::cout << "RB" << std::endl;
            }
            else if (sf::Joystick::isButtonPressed(0, static_cast<unsigned int>(xbox::RJoy)))
            {
                if (tileSpacingIgnoreCounter <= 0)
                {
                    dispInput.tileSpacing = true;
                    tileSpacingIgnoreCounter = 20;
                    std::cout << "RJoy" << std::endl;
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            posY = -100.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            posX = -100.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            posY = 100.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            posX = 100.f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            posV = -100.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            posU = -100.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            posV = 100.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            posU = 100.f;
        }

        if (xboxConnected)
        {
            posX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
            posY = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
            posZ = sf::Joystick::getAxisPosition(0, sf::Joystick::Z); // <- Zoom 
            posR = sf::Joystick::getAxisPosition(0, sf::Joystick::R);
            posU = sf::Joystick::getAxisPosition(0, sf::Joystick::U);
            posV = sf::Joystick::getAxisPosition(0, sf::Joystick::V);
        }

        if (enableDebugPrint) // && (std::abs(posX) > 0.1 || std::abs(posY) > 0.1)
        {
            std::cout << "Joystick x,y,z,r,u,v: " 
                        << posX << " " << posY << " " << posZ << " "
                        << posR << " " << posU << " " << posV << std::endl;
        }

        double power{ std::sqrt(std::pow(posX, 2) + std::pow(posY, 2)) };
        power = (power > 100.0) ? 100.0 : power;
        if (power > joyThreshLow){
            input.move.power = power / 200.0;
            input.move.angle = atan2(posY, posX);
        }
        else{
            input.move.power = 0.f;
            input.move.angle = 0.f;
        }

        if (std::abs(posU) > 10 || std::abs(posV) > 10)
        {
            dispInput.horizontal = posU; 
            dispInput.vertical = posV;
        }

        if (std::abs(posZ) > 10.0)
        {
            dispInput.zoom = posZ;
        }

        dispInput.showCoordinateSystem = showCoordinateSystem;
        dispInput.enableShaders = enableShaders;

        if (tileSpacingIgnoreCounter > 0)
            tileSpacingIgnoreCounter -= 1;

        endTime = Time::now();
        timeDuration timePast{ endTime - startTime };
        double dt{ timePast.count() };
        startTime = endTime;

        // Activate the window for OpenGL rendering
        window.setActive();
 
        // OpenGL drawing commands go here...

        game.update(input, 0, dt);
        graphics.update(game, window, dispInput, dt);

        sf::sleep(sf::milliseconds(10));
    }
}