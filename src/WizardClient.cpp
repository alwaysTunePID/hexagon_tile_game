#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono> 
#include <mutex>
#include <SFML/Graphics.hpp>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "gameservice.grpc.pb.h"


#include "Graphics.h"
#include "Game.h"

ABSL_FLAG(std::string, target, "localhost:50051", "Server address");

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::Status;

using GameService::ConnectRequest;
using GameService::ConnectResponse;
using GameService::DisconnectRequest;
using GameService::DisconnectResponse;
using GameService::StateRequest;
using GameService::StateResponse;
using GameService::StepRequest;
using GameService::StepResponse;

unsigned int WIDTH{ 1920 };
unsigned int HEIGHT{ 1080 };

//window.create(sf::VideoMode(WIDTH, HEIGHT), "Babas are Us" );


class GameServiceClient {
public:
    explicit GameServiceClient(std::shared_ptr<Channel> channel)
        : stub_(GameService::GameService::NewStub(channel)),
          id_(0) {}


    bool Connect()
    {
        ConnectRequest request;
        request.set_ready(true);

        ConnectResponse response;
        ClientContext context;

        Status status = stub_->Connect(&context, request, &response);

        if (status.ok())
        {
            id_ = response.id();
        }
        else
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));

        return status.ok();
    }

    void Disconnect()
    {
        DisconnectRequest request;
        DisconnectResponse response;
        ClientContext context;

        Status status = stub_.get()->Disconnect(&context, request, &response);

        if (!status.ok())
        {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
            
    }


    void gameInput(actionType action) {

        StepRequest stepReq;
        stepReq.set_id(id_);
        stepReq.set_action(static_cast<int>(action));

        StepResponse stepResp;
        ClientContext context;

        Status status = stub_->MakeStep(&context, stepReq, &stepResp);

        if (!status.ok())
        {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
    }


    void getState(Game* game) {

        StateRequest stateReq;
        stateReq.set_id(id_);

        StateResponse stateResp;
        ClientContext context;

        Status status = stub_->GetState(&context, stateReq, &stateResp);

        if (!status.ok())
        {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }

        game->setState(static_cast<stateType>(stateResp.state()));

        if (static_cast<stateType>(stateResp.state()) == stateType::playerTurn)
        {
            game->removeAllTiles();
            for (int i = 0; i < stateResp.blocks_size(); i++) {
                const StateResponse::Tile& blockM = stateResp.blocks(i);

                int id{ blockM.id() };
                tileType blocktype{ blockM.type() };
                EffectType effecttype{ blockM.text() };

                Tile tile{ Tile(blocktype, effecttype, id) };
                TileIdx tileIdx = { blockM.tileIdx().r(), blockM.tileIdx().c() };

                tile.setDirection(static_cast<directionType>(blockM.dir()));
                tile.setAdjMask(blockM.adjmask());
                tile.setMoveIdx(blockM.moveidx());
                tile.setActive(blockM.active());

                for (int ii = 0; ii < blockM.properties_size(); ii++) {
                    tile.addEffect(static_cast<EffectType>(blockM.properties(ii)));
                }

                game->addTile(tile, tileIdx);
            }

            game->removeAllPlayers();
            for (int i = 0; i < stateResp.players_size(); i++) {
                const StateResponse::Player& playerM = stateResp.players(i);
                game->addPlayer(
                    Player(
                        playerM.points(),
                        playerM.turntime(),
                        static_cast<EffectType>(playerM.textblock()),
                        playerM.currentplayer()));
            }
        }
    }

private:

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<GameService::GameService::Stub> stub_;
    int id_;
};

int main(int argc, char** argv) {
    absl::ParseCommandLine(argc, argv);
    // 
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the argument "--target=" which is the only expected argument.

    std::cout << "HEEEEELLLLOOOOOO" << std::endl;

    auto window = sf::RenderWindow{ {WIDTH, HEIGHT} , "Babas are Us" };
    window.setFramerateLimit(144);

    std::string target_str = absl::GetFlag(FLAGS_target); //  "localhost:50051";
    // We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).

    Graphics graphics{};
    Game game{};

    GameServiceClient GameService(
        grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

    bool connected = GameService.Connect();

    while (!connected)
        connected = GameService.Connect();


    while (window.isOpen())
    {
        bool printCurrentRules{ false };
        actionType action{ actionType::none };

        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                action = actionType::up;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                action = actionType::down;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                action = actionType::left;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                action = actionType::right;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
            {
                action = actionType::undoMove;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            {
                printCurrentRules = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
            {
                action = actionType::skipTurn;
            }
        }

        // This optimization can't be done right now since the client makes the server to tick
        //if (action != actionType::none)
        GameService.gameInput(action);

        GameService.getState(&game);

        graphics.update(game, window);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Spawn reader thread that loops indefinitely
    //std::thread thread1_ = std::thread(&GameServiceClient::RpcInput, &GameService, &game, &window);
    //std::thread thread2_ = std::thread(&Game::update, std::ref(game), std::ref(window));
    //std::thread thread3_ = std::thread(&Graphics::update, &graphics, std::ref(game), std::ref(window));

    //std::cout << "Press control-c to quit" << std::endl << std::endl;
    //thread1_.join();
    //thread2_.join();
    //thread3_.join();

    return 0;
}