#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <math.h>
#include "Enums.h"
#include "ConfigParser.h"
#include "Game.h"
#include "Graphics.h"
#include "PacketFunctions.h"

///////////////// Global stuff /////////////////////////
bool enableDebugPrint{ false };
bool showCoordinateSystem{ false };
bool enableShaders{ false };
int  tileSpacingIgnoreCounter{ 0 };

int playerId{ 0 };
double joyThreshHigh{ 0.0 };
double joyThreshLow{ 0.0 };
unsigned seed { 2952795 };
std::string configPath{ "../../../config.txt" };
ConfigParser configParser{ configPath };

sf::IpAddress  serverIp;
unsigned short serverPort{ 55001 };

///////////////// Functions ////////////////////////////
void getInputs(sf::RenderWindow& window, gameInput& input, displayInput& dispInput)
{
    float posX{};
    float posY{};
    float posZ{};
    float posR{};
    float posU{};
    float posV{};

    bool xboxConnected = sf::Joystick::isConnected(0);

    for (auto event = sf::Event{}; window.pollEvent(event);)
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

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
}

void sendInitMsgToServerAndWaitForResp(sf::UdpSocket& socket)
{
    bool serverAnswered{ false };
    while (!serverAnswered)
    {
        char s_data[100] = "Helllooooo from Client!";

        if (socket.send(s_data, 100, serverIp, serverPort) != sf::Socket::Status::Done)
        {
            std::cout << "ERROR - Couldn't send data" << std::endl;
        }

        sf::Uint8 r_data[100];
        std::size_t received;
        sf::IpAddress sender;
        unsigned short senderPort;
        sf::Socket::Status status = socket.receive(r_data, 100, received, sender, senderPort);
        if (status != sf::Socket::Status::Done && status != sf::Socket::Status::NotReady)
        {
            std::cout << "ERROR - Couldn't receive data" << std::endl;
        }
        else if (status == sf::Socket::Status::Done)
        {
            if (sender != serverIp)
                continue;

            serverAnswered = true;
            playerId = r_data[0];
            std::cout << "Player id: " << playerId << std::endl;
            std::cout <<  "Waiting for other wizards" << std::endl;     
        }

        sf::sleep(sf::milliseconds(100));
    }
}

void waitForAllPlayersToConnect(sf::UdpSocket& socket)
{
    bool waitForAllToConnect{ true };
    while (waitForAllToConnect)
    {
        char r_data[100];
        std::size_t received;
        sf::IpAddress sender;
        unsigned short senderPort;
        sf::Socket::Status status = socket.receive(r_data, 100, received, sender, senderPort);
        if (status != sf::Socket::Status::Done && status != sf::Socket::Status::NotReady)
        {
            std::cout << "ERROR - Couldn't receive data" << std::endl;
        }
        else if (status == sf::Socket::Status::Done)
        {
            if (sender != serverIp)
                continue;

            if (r_data[0] == 'r')
            {
                waitForAllToConnect = false;
                std::cout << "Let the game begin!" << std::endl;   
            }    
        }
        sf::sleep(sf::milliseconds(100));
    }
}

///////////////// Main /////////////////////////////////
int main()
{
    serverIp = sf::IpAddress::LocalHost; // getLocalAddress();  // my address on the local network
    //sf::IpAddress a9 = sf::IpAddress::getPublicAddress(); // my address on the internet
    unsigned short port{ 55002 };
    sf::UdpSocket  socket;
    socket.setBlocking(true);

    // bind the socket to a port
    for (size_t i{ 0 }; i < 10; i++)
    {
        if (socket.bind(port, sf::IpAddress::LocalHost) == sf::Socket::Status::Done)
            break;
        if (i == 9)
            std::cout << "ERROR - Binding sockett" << std::endl;
        port++;
    }

    if (!DEBUG_LAUNCH)
    {
        std::cout << "Enter server IP address:" << std::endl;
        std::cin >> serverIp;
    }
    
    sendInitMsgToServerAndWaitForResp(socket);
    waitForAllPlayersToConnect(socket);

    auto window = sf::RenderWindow { {WIDTH2, HEIGHT2}, "Some Game" };
    window.setFramerateLimit(144);
    window.setVerticalSyncEnabled(true);

    configParser.loadConfig(joyThreshHigh, joyThreshLow, seed);

    Game game{seed};
    Graphics graphics{};

    game.createPlayer();
    game.createPlayer();

    timePoint startTime{ Time::now() };
    timePoint endTime{};
    timePoint clientStartTime{};
    timePoint clientEndTime{};

    while (window.isOpen())
    {
        moveInput move{};
        gameInput input{ xbox::none, actionType::none, move };
        displayInput dispInput{};

        getInputs(window, input, dispInput);

        // Send input to server
        sf::Packet packet;
        packet << input;
        socket.send(packet, serverIp, serverPort);

        // Measure client active time
        clientEndTime = Time::now();
        timeDuration clientTimePast{ clientEndTime - clientStartTime };
        //std::cout << "Active time: " << clientTimePast.count() << std::endl;

        // Wait for answer
        GameStruct serverMsg;
        sf::IpAddress sender;
        unsigned short senderPort;
        socket.receive(packet, sender, senderPort);
        clientStartTime = Time::now();

        //std::cout << "Package size: " << packet.getDataSize() << std::endl;

        if (sender == serverIp)
        {
            sf::Int8 packageTypeInt;
            packet = packet >> packageTypeInt;
            PackageType packageType = (PackageType)packageTypeInt;

            if (packet >> serverMsg)
            {
                if (packageType == PackageType::all)
                    game.setAllData(serverMsg);
                else if (packageType == PackageType::delta)
                    game.setDeltaData(serverMsg);
                // TODO: Change to SFML time
                endTime = Time::now();
                timeDuration timePast{ endTime - startTime };
                double dt{ timePast.count() };
                startTime = endTime;

                // Activate the window for OpenGL rendering
                window.setActive();

                // OpenGL drawing commands go here...

                //moveInput noMove{};
                //gameInput noInput{ xbox::none, actionType::none, noMove }; // Temp
                //game.update(noInput, 0, dt); // Id has to be known!!!
                graphics.update(game, window, dispInput, dt);

            }
            else
            {
                std::cout << "Error extracting package from server!" << std::endl;
            }
        }

        //sf::sleep(sf::milliseconds(10));
    }
}