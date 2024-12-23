cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#set(CMAKE_CXX_STANDARD 17)
#find_package(Threads REQUIRED)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

#add_executable(WizardClient src/WizardClient.cpp src/Tile.cpp src/Graphics.cpp src/Game.cpp src/Tilesprite.cpp src/Board.cpp src/Player.cpp)
#target_link_libraries(WizardClient
#    hw_grpc_proto
#    absl::flags
#    absl::flags_parse
#    ${_REFLECTION}
#    ${_GRPC_GRPCPP}
#    ${_PROTOBUF_LIBPROTOBUF}
#    sfml-graphics
#)
#target_compile_features(WizardClient PRIVATE cxx_std_17)


add_executable(WizardServer src/main.cpp src/ConfigParser.cpp src/Tile.cpp src/WorldObject.cpp src/Graphics.cpp src/Game.cpp src/Tilesprite.cpp src/WorldObjectSprite.cpp src/Board.cpp src/Player.cpp src/Fire.cpp)
target_link_libraries(WizardServer
    sfml-graphics
)
target_compile_features(WizardServer PRIVATE cxx_std_17) 


# add_executable(WizardServer src/main.cpp src/Tile.cpp src/Graphics.cpp src/Game.cpp src/Tilesprite.cpp src/Board.cpp src/RuleList.cpp src/Player.cpp src/WizardServer.cpp src/WizardServer.cpp)
#add_executable(WizardServer src/WizardServer.cpp)
#target_link_libraries(WizardServer PRIVATE CMakeSFMLProject sfml-graphics grpc++)
#target_compile_features(CMakeSFMLProject PRIVATE cxx_std_20)

#add_executable(WizardServer src/main.cpp src/Tile.cpp src/Graphics.cpp src/Game.cpp src/Tilesprite.cpp src/Board.cpp src/RuleList.cpp src/Player.cpp src/WizardServer.cpp src/WizardServer.cpp)
#target_link_libraries (WizardServer PRIVATE CMakeSFMLProject sfml-graphics grpc++)
#target_compile_features(WizardServer PRIVATE cxx_std_20)

#[[
if(WIN32)
    add_custom_command(
        TARGET CMakeSFMLProject
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:CMakeSFMLProject>
        VERBATIM)
endif()]]

#install(TARGETS CMakeSFMLProject)