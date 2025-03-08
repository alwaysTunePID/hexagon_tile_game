#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <math.h>
#include <GL/glew.h>
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

std::string name;
uint8_t playerId{ 0 };
double joyThreshHigh{ 0.0 };
double joyThreshLow{ 0.0 };
unsigned seed { 2952795 };
std::string configPath{ "../../../config.txt" };
ConfigParser configParser{ configPath };

unsigned short serverPort{ 55001 };
bool sendingActivated{ true };

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
    bool hasFocus = window.hasFocus();

    const auto onClose = [&window](const sf::Event::Closed&)
    {
        window.close();
    };

    const auto onKeyPressed = [&window, &hasFocus](const sf::Event::KeyPressed& keyPressed)
    {
        if (hasFocus) {
            if (keyPressed.scancode == sf::Keyboard::Scancode::Escape)
                window.close();
        }
    };

    const auto onKeyReleased = [&window, &hasFocus, &input](const sf::Event::KeyReleased& keyReleased)
    {
        if (hasFocus) {
            if (keyReleased.scancode == sf::Keyboard::Scancode::N)
                input.action = actionType::skipTurn;
            else if (keyReleased.scancode == sf::Keyboard::Scancode::C)
            {
                if (showCoordinateSystem) {
                    showCoordinateSystem = false;
                    std::cout << "Hide Coordinate System" << std::endl;
                } else {
                    std::cout << "Show Coordinate System" << std::endl;
                    showCoordinateSystem = true;
                }
            }
            else if (keyReleased.scancode == sf::Keyboard::Scancode::V)
            {
                if (enableShaders) {
                    std::cout << "Disable Shaders" << std::endl;
                    enableShaders = false;
                } else {
                    std::cout << "Enable Shaders" << std::endl;
                    enableShaders = true;
                }
            }
            else if (keyReleased.scancode == sf::Keyboard::Scancode::L)
            {
                configParser.loadConfig(joyThreshHigh, joyThreshLow, seed);
                std::cout << joyThreshHigh << " " << joyThreshLow << std::endl;
            }
        }
    };

    const auto onJoystickButtonReleased = [&window, &hasFocus, &input](const sf::Event::JoystickButtonReleased& joystickButtonReleased)
    {
        if (hasFocus) {
            if (joystickButtonReleased.button == static_cast<unsigned int>(xbox::A))
            {
                input.button = xbox::A;
                std::cout << "A" << std::endl;
            }
            else if (joystickButtonReleased.button == static_cast<unsigned int>(xbox::B))
            {
                input.button = xbox::B;
                std::cout << "B" << std::endl;
            }
            else if (joystickButtonReleased.button == static_cast<unsigned int>(xbox::X))
            {
                input.button = xbox::X;
                std::cout << "X" << std::endl;
            }
            else if (joystickButtonReleased.button == static_cast<unsigned int>(xbox::Y))
            {
                input.button = xbox::Y;
                std::cout << "Y" << std::endl;
                enableDebugPrint = enableDebugPrint ? false : true;
            }
            else if (joystickButtonReleased.button == static_cast<unsigned int>(xbox::LB))
            {
                input.button = xbox::LB;
                std::cout << "LB" << std::endl;
            }
            else if (joystickButtonReleased.button == static_cast<unsigned int>(xbox::RB))
            {
                input.button = xbox::RB;
                std::cout << "RB" << std::endl;
            }
            else if (joystickButtonReleased.button == static_cast<unsigned int>(xbox::RJoy))
            {
                std::cout << "RJoy" << std::endl;
            }
        }
    };

    window.handleEvents(onClose, onKeyPressed, onKeyReleased, onJoystickButtonReleased);

    // Now, handle "while hold down" keys
    auto isKeyDown = [&hasFocus](sf::Keyboard::Scancode key) -> auto {
        // isKeyPressed also works if window is unfocused. Ensure window is focused.
        return hasFocus && sf::Keyboard::isKeyPressed(key);
    };

    if (isKeyDown(sf::Keyboard::Scancode::P))
    {
        std::cout << "Pressed P " << std::endl;
    }
    if (isKeyDown(sf::Keyboard::Scancode::LShift) && isKeyDown(sf::Keyboard::Scancode::Z))
    {
        posZ = -160.f;
    }
    if (!isKeyDown(sf::Keyboard::Scancode::LShift) && isKeyDown(sf::Keyboard::Scancode::Z))
    {
        posZ = 160.f;
    }
    // Move player
    if (isKeyDown(sf::Keyboard::Scancode::W))
    {
        posY = -100.f;
    }
    if (isKeyDown(sf::Keyboard::Scancode::A))
    {
        posX = -100.f;
    }
    if (isKeyDown(sf::Keyboard::Scancode::S))
    {
        posY = 100.f;
    }
    if (isKeyDown(sf::Keyboard::Scancode::D))
    {
        posX = 100.f;
    }
    // Move camera
    if (isKeyDown(sf::Keyboard::Scancode::Up))
    {
        posV = -100.f;
    }
    if (isKeyDown(sf::Keyboard::Scancode::Left))
    {
        posU = -100.f;
    }
    if (isKeyDown(sf::Keyboard::Scancode::Down))
    {
        posV = 100.f;
    }
    if (isKeyDown(sf::Keyboard::Scancode::Right))
    {
        posU = 100.f;
    }

    if (xboxConnected)
    {
        posX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
        posY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);
        posZ = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z); // <- Zoom 
        posR = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::R);
        posU = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U);
        posV = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V);
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

void sendInitMsgToServerAndWaitForResp(sf::IpAddress& serverIp, sf::UdpSocket& socket)
{
    bool serverAnswered{ false };
    while (!serverAnswered)
    {
        sf::Packet packet;
        if (!DEBUG_LAUNCH)
        {
            std::cout << "Enter Wizard Name:" << std::endl;
            std::cin >> name;
        }
        else
        {
            // Ugly
            unsigned short port{socket.getLocalPort()}; 
            if (port == 55002U)
                name = "Grimnir";
            else if (port == 55003U)
                name = "Vlad";
            else
                name = "Khan";
        }
        packet << name;
        if (socket.send(packet, serverIp, serverPort) != sf::Socket::Status::Done)
        {
            std::cout << "ERROR - Couldn't send data" << std::endl;
        }

        //uint8_t r_data[100];
        //std::size_t received;
        std::optional<sf::IpAddress> sender;
        unsigned short senderPort;
        //sf::Socket::Status status = socket.receive(r_data, 100, received, sender, senderPort);
        sf::Socket::Status status = socket.receive(packet, sender, senderPort);
        if (status != sf::Socket::Status::Done && status != sf::Socket::Status::NotReady)
        {
            std::cout << "ERROR - Couldn't receive data" << std::endl;
        }
        else if (status == sf::Socket::Status::Done)
        {
            if (sender.value() != serverIp)
                continue;

            serverAnswered = true;
            packet >> playerId;
            //playerId = r_data[0];
            std::cout << "Player id: " << (int)playerId << std::endl;
            std::cout <<  "Waiting for other wizards" << std::endl;     
        }

        sf::sleep(sf::milliseconds(100));
    }
}

void waitForAllPlayersToConnect(sf::IpAddress& serverIp, sf::UdpSocket& socket)
{
    bool waitForAllToConnect{ true };
    while (waitForAllToConnect)
    {
        sf::Packet packet;
        packet << name;
        if (socket.send(packet, serverIp, serverPort) != sf::Socket::Status::Done)
        {
            std::cout << "ERROR - Couldn't send data" << std::endl;
        }

        std::optional<sf::IpAddress> sender;
        unsigned short senderPort;
        sf::Socket::Status status = socket.receive(packet, sender, senderPort);
        if (status != sf::Socket::Status::Done && status != sf::Socket::Status::NotReady)
        {
            std::cout << "ERROR - Couldn't receive data" << std::endl;
        }
        else if (status == sf::Socket::Status::Done)
        {
            if (sender.value() != serverIp)
                continue;

            std::string gameStatus;
            if (packet >> gameStatus)
            {
                if (gameStatus == "Ready")
                {
                    waitForAllToConnect = false;
                    std::cout << "Let the game begin!" << std::endl;  
                }
            }    
        }
        sf::sleep(sf::milliseconds(10));
    }
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, 
    GLenum severity, GLsizei length, 
    const GLchar* message, const void* userParam)
{
    std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "**GL ERROR**" : "")
    << " type = " << type
    << ", severity = " << severity
    << ", message = " << message << std::endl;
}

void EnableOpenGLDebug()
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
}

///////////////// Main /////////////////////////////////
int main()
{
    unsigned short port{ 55002 };
    sf::UdpSocket  socket;
    sf::IpAddress serverIp{ sf::IpAddress::getPublicAddress().value() };

    // TODO: Temp solution. Should be moved to a function
    if (!LOCAL_ONLY)
    {
        socket.setBlocking(true);

        // bind the socket to a port
        for (size_t i{ 0 }; i < 10; i++)
        {
            if (socket.bind(port) == sf::Socket::Status::Done) // sf::IpAddress::LocalHost
                break;
            if (i == 9)
                std::cout << "ERROR - Binding sockett" << std::endl;
            port++;
        }
        
        std::string serverIpStr{};
        if (!DEBUG_LAUNCH) {
            std::cout << "Enter server IP address:" << std::endl;
            std::cin >> serverIpStr;
            auto serverIp_p{ sf::IpAddress::resolve(serverIpStr) };
            if (serverIp_p) {
                serverIp = serverIp_p.value();
            } else {
                std::cout << "ERROR: Invalid server IP!" << std::endl;
            }
        } else {
            std::cout << "Debug Launch" << std::endl;
        }
    
        sendInitMsgToServerAndWaitForResp(serverIp, socket);
        waitForAllPlayersToConnect(serverIp, socket);
    }

    sf::ContextSettings settings{ sf::ContextSettings() };
    settings.majorVersion = 4;
    settings.minorVersion = 6;

    sf::RenderWindow window( sf::VideoMode({WIDTH2, HEIGHT2}), "Wizards of Hexagon", sf::Style::Default, sf::State::Windowed, settings );
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR: Failed to initialize GLEW" << std::endl;
    }
    glMatrixMode(GL_PROJECTION);
    EnableOpenGLDebug();

    configParser.loadConfig(joyThreshHigh, joyThreshLow, seed);

    Game game{ seed };
    Graphics graphics{ seed };

    if (LOCAL_ONLY) {
        game.createPlayer(0, "Grimnir");
        game.createPlayer(1, "Vlad");
    }

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

        if (LOCAL_ONLY)
        {
            endTime = Time::now();
            timeDuration timePast{ endTime - startTime };
            double dt{ timePast.count() };
            startTime = endTime;

            // playerId is always 0 in LOCAL_ONLY mode
            game.update(input, playerId, dt);
            //moveInput noMove{};
            //gameInput noInput{ xbox::none, actionType::none, noMove }; // Temp
            //game.update(noInput, 0, dt); // Id has to be known!!!
            graphics.update(game, window, dispInput, playerId, dt);
        }
        else
        {
            sf::Packet packet;
            if (sendingActivated)
            {
                // Send input to server
                packet << playerId << input;
                if (socket.send(packet, serverIp, serverPort) != sf::Socket::Status::Done) {
                    std::cout << "ERROR: Couldn't send package to server" << std::endl;
                }
            }
    
            // Measure client active time
            clientEndTime = Time::now();
            timeDuration clientTimePast{ clientEndTime - clientStartTime };
            //std::cout << "Active time: " << clientTimePast.count() << std::endl;
    
            // Wait for answer
            std::optional<sf::IpAddress> sender;
            unsigned short senderPort;
            if (socket.receive(packet, sender, senderPort) != sf::Socket::Status::Done) {
                std::cout << "ERROR: Couldn't receive package to server" << std::endl;
            }
            clientStartTime = Time::now();
    
            //std::cout << "Package size: " << packet.getDataSize() << std::endl;
    
            if (sender.value() == serverIp)
            {
                int8_t packageTypeInt;
                packet = packet >> packageTypeInt;
                PackageType packageType = (PackageType)packageTypeInt;
    
                if (packageType == PackageType::all)
                {
                    GameStruct serverMsg;
                    if (packet >> serverMsg)
                    {
                        game.setAllData(serverMsg);
                        //sendingActivated = false; // Because waiting for second part of message
                    }
                    else
                    {
                        std::cout << "Error extracting PackageType::all from server!" << std::endl;
                    }
                }
                else if (packageType == PackageType::delta)
                {
                    GameStruct serverMsg;
                    if (packet >> serverMsg)
                    {
                        game.setDeltaData(serverMsg);
                    }
                    else
                    {
                        std::cout << "Error extracting PackageType::delta from server!" << std::endl;
                    }
                }
                // TODO: Change to SFML time
                endTime = Time::now();
                timeDuration timePast{ endTime - startTime };
                double dt{ timePast.count() };
                startTime = endTime;
    
                //moveInput noMove{};
                //gameInput noInput{ xbox::none, actionType::none, noMove }; // Temp
                //game.update(noInput, 0, dt); // Id has to be known!!!
                graphics.update(game, window, dispInput, playerId, dt);
            }   
        }
        //sf::sleep(sf::milliseconds(10));
    }
}