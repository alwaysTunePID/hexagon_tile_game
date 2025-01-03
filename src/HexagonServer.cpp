#include <iostream>
#include <SFML/Network/UdpSocket.hpp>
#include "Enums.h"
#include "ConfigParser.h"
#include "Game.h"
#include "Graphics.h"

void updateGameWithInput(char* r_data)
{
}

int main()
{
    unsigned short port = 54001;

    sf::UdpSocket socket;
    socket.setBlocking(false);

    // bind the socket to a port
    if (socket.bind(port) != sf::Socket::Done)
    {
        std::cout << "ERROR - Binding socket" << std::endl;
    }

    while(true)
    {
        char r_data[100];
        std::size_t received;
        sf::IpAddress sender;
        unsigned short portt;

        // UDP socket:
        sf::Socket::Status status = socket.receive(r_data, 100, received, sender, portt);
        if (status != sf::Socket::Done && status != sf::Socket::NotReady)
        {
            std::cout << "ERROR - Couldn't receive data" << std::endl;
        }
        else if (status = sf::Socket::Done)
        {
            std::cout << "Received " << received << " bytes from " << sender << " on port " << portt << std::endl;

            updateGameWithInput(r_data);

            char s_data[100] = "Helllooooo!";

            // Send
            if (socket.send(s_data, 100, sender, portt) != sf::Socket::Done)
            {
                std::cout << "ERROR - Couldn't send data" << std::endl;
            }
        }
    
        sf::sleep(sf::milliseconds(100));
    }
    
}