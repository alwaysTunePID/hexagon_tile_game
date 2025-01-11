#include <iostream>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include "Enums.h"
#include "ConfigParser.h"
#include "Game.h"
#include "Graphics.h"
#include "PacketFunctions.h"

///////////////// Global stuff /////////////////////////
typedef struct clientData {
    sf::IpAddress ip;
    unsigned short port;
    std::string name;
    bool needsAllData;
    bool connected;
} clientData;

std::map<size_t, clientData> clients{};
size_t numOfPlayers{ 2 };

sf::IpAddress serverIp;
unsigned short port{ 55001 };

///////////////// Functions ////////////////////////////
void waitForAllClientsToSendInitMsg(Game& game, sf::UdpSocket& socket)
{
    while(clients.size() != numOfPlayers)
    {
        std::string gameStatus{ "Waiting" };
        sf::Packet packet;
        //char r_data[100];
        //std::size_t received{ 0 };
        sf::IpAddress sender;
        unsigned short clientPort;

        //sf::Socket::Status status = socket.receive(r_data, (std::size_t)100, received, sender, clientPort);
        sf::Socket::Status status = socket.receive(packet, sender, clientPort);
        if (status != sf::Socket::Status::Done && status != sf::Socket::Status::NotReady)
        {
            std::cout << "ERROR - Couldn't receive data" << std::endl;
        }
        else if (status == sf::Socket::Status::Done)
        {
            std::string name;
            if (packet >> name)
            {
                bool isNewClient{ true };
                for (auto& [id, clientdata] : clients)
                {
                    if (clientdata.ip == sender && clientdata.port == clientPort && clientdata.name == name)
                    {
                        packet.clear();
                        packet << gameStatus;
                        if (socket.send(packet, sender, clientPort) != sf::Socket::Status::Done)
                            std::cout << "ERROR - Couldn't send data" << std::endl;
                        isNewClient = false;
                        break;
                    }
                }

                if (isNewClient)
                {
                    size_t id{ clients.size() };
                    clientData clientdata{ sender, clientPort, name, true, true};
                    clients.insert({ id, clientdata });
                    game.createPlayer();
                    std::cout << name << " connected. (" << clients.size() << "/" << numOfPlayers << ")" << std::endl;

                    packet.clear();
                    packet << (sf::Uint8)id;
                    if (socket.send(packet, sender, clientPort) != sf::Socket::Status::Done)
                    {
                        std::cout << "ERROR - Couldn't send data" << std::endl;
                    }
                }
            }
        }
    }
}

void sendToAllClientsThatGameWillBegin(sf::UdpSocket& socket)
{
    std::string gameStatus{ "Ready" };
    sf::Packet packet;
    packet << gameStatus;
    for (auto& [id, clientdata] : clients)
    {
        if (socket.send(packet, clientdata.ip, clientdata.port) != sf::Socket::Status::Done)
            std::cout << "ERROR - Couldn't send data" << std::endl;
    }

    std::cout << "Let the game begin!" << std::endl;
}

///////////////// Main /////////////////////////////////
int main()
{
    double joyThreshHigh{ 0.0 };
    double joyThreshLow{ 0.0 };
    unsigned seed { 2952795 };
    std::string configPath{ "../../../config.txt" };
    ConfigParser configParser{ configPath };
    configParser.loadConfig(joyThreshHigh, joyThreshLow, seed);
    Game game{seed};

    sf::UdpSocket socket;
    socket.setBlocking(true);
    // Change this!!
    serverIp = sf::IpAddress::getLocalAddress(); // sf::IpAddress::LocalHost;

    // bind the socket to a port
    if (socket.bind(port, serverIp) != sf::Socket::Status::Done)
    {
        std::cout << "ERROR - Binding socket" << std::endl;
    }

    if (DEBUG_LAUNCH)
        std::cout << "Debug Launch\n" << std::endl;
    else
    {
        //std::cout << "Private IP: " << serverIp.toString() << std::endl;
        std::cout << "This is the server IP that the clients need to enter:\n" << sf::IpAddress::getPublicAddress() << std::endl;
        std::cout << "Enter number of players (E.g. 2):" << std::endl;
        std::cin >> numOfPlayers;
    }

    waitForAllClientsToSendInitMsg(game, socket);
    sendToAllClientsThatGameWillBegin(socket);

    // Temp - To get from lobby to generateLevel
    gameInput input{};
    game.update(input, 0, 0.0);

    timePoint startTime{ Time::now() };
    timePoint endTime{};
    timePoint serverStartTime{};
    timePoint serverEndTime{};

    while (true)
    {
        bool sendToClient{ false };
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short clientPort;
        socket.receive(packet, sender, clientPort);
        serverStartTime = Time::now();

        gameInput input;
        if (packet >> input)
        {
            GameStruct gameMsg;
            // Data extracted successfully...
            for (auto& [id, clientdata] : clients)
            {
                if (clientdata.ip == sender && clientdata.port == clientPort)
                {
                    //std::cout << "Got power: " << input.move.power << " and angle: " << input.move.angle << " from player id: " << id << std::endl;
                    endTime = Time::now();
                    timeDuration timePast{ endTime - startTime };
                    double dt{ timePast.count() };
                    startTime = endTime;
                    game.update(input, id, dt);

                    sf::Packet packet;
                    if (clientdata.needsAllData)
                    {
                        game.getAllData(gameMsg);
                        packet << (sf::Int8)PackageType::all;
                        clientdata.needsAllData = false;
                    }
                    else
                    {
                        game.getDeltaData(gameMsg);
                        packet << (sf::Int8)PackageType::delta;
                    }                      
                    packet << gameMsg;
                    socket.send(packet, clientdata.ip, clientdata.port);
                    break;
                }
            }
        }
        else
        {
            std::cout << "Error extracting package from client!" << std::endl;
        }

        serverEndTime = Time::now();
        timeDuration serverTimePast{ serverEndTime - serverStartTime };
        //std::cout << "Active time: " << serverTimePast.count() << std::endl;
    }
    
}