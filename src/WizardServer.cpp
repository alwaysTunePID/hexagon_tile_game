#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "gameservice.grpc.pb.h"

#include "Game.h"


ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using GameService::ConnectRequest;
using GameService::ConnectResponse;
using GameService::DisconnectRequest;
using GameService::DisconnectResponse;
using GameService::StateRequest;
using GameService::StateResponse;
using GameService::StepRequest;
using GameService::StepResponse;


// Logic and data behind the server's behavior.
class GameServiceImpl final : public GameService::GameService::Service {

public:
    Status Connect(ServerContext* context, const ConnectRequest* request, ConnectResponse* response) override
    {
        
        if (m_players > 1)
            return { grpc::StatusCode::UNAVAILABLE, "Two player are connected." };

        game.createPlayer();
        game.update(actionType::none, m_players);
        response->set_id(m_players);

        std::cout << "Player " << m_players << " is connected." << std::endl;
        ++m_players;

        return Status::OK;
    }

    Status Disconnect(ServerContext* context, const DisconnectRequest* request, DisconnectResponse* response) override
    {
        --m_players;
        std::cout << "Player disconnected." << std::endl;

        return Status::OK;
    }

    Status MakeStep(ServerContext* context, const StepRequest* request, StepResponse* response) override
    {
        actionType action = static_cast<actionType>(request->action());

        game.update(action, request->id());

        // return { grpc::StatusCode::PERMISSION_DENIED, "Not you turn." };
        return Status::OK;
    }

    Status GetState(ServerContext* context, const StateRequest* request, StateResponse* response) override
    {

        response->set_state(static_cast<int>(game.getState()));

        if (game.getState() == stateType::playerTurn)
        {
            // response->clear_blocks(); Probably don't need this
            for (auto& [id, tile] : game.getTiles())
            {
                StateResponse::Tile* blockM = response->add_blocks();
                blockM->set_id(id);
                blockM->mutable_tile()->set_r(tile.getTile().r);
                blockM->mutable_tile()->set_c(tile.getTile().c);
                blockM->set_dir(static_cast<int>(tile.getDirection()));
                blockM->set_adjmask(tile.getAdjMask());
                blockM->set_moveidx(tile.getMoveIdx());
                blockM->set_type(static_cast<int>(tile.getTileType()));
                blockM->set_text(static_cast<int>(tile.getTextType()));
                blockM->set_active(tile.isActive());

                for (EffectType& effect : tile.getProperties())
                {
                    blockM->add_properties(static_cast<int>(effect));
                }
            }

            for (Player& player : game.getPlayers())
            {
                StateResponse::Player* playerM = response->add_players();
                playerM->set_points(player.getPoints());
                playerM->set_turntime(player.getTurnTime());
                playerM->set_textblock(static_cast<int>(player.getTextBlock()));
                playerM->set_currentplayer(player.isCurrentPlayer());
            }
        }

        return Status::OK;
    }

private:
    int m_players{ 0 };
    Game game{};

};

void RunServer(uint16_t port) {
    std::string server_address = absl::StrFormat("localhost:%d", port);
    GameServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv) {
    absl::ParseCommandLine(argc, argv);
    RunServer(absl::GetFlag(FLAGS_port));
    return 0;
}