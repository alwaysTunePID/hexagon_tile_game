cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#set(CMAKE_CXX_STANDARD 17)
#find_package(Threads REQUIRED)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
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

link_directories(${CMAKE_SOURCE_DIR}/libraries/lib)
add_executable(WizardClient src/HexagonClient.cpp src/PacketFunctions.cpp src/ConfigParser.cpp src/Tile.cpp src/WorldObject.cpp src/Graphics.cpp src/Game.cpp src/Tilesprite.cpp src/WorldObjectSprite.cpp src/Board.cpp src/Player.cpp src/Fire.cpp)
target_include_directories(WizardClient PRIVATE ${CMAKE_SOURCE_DIR}/libraries/include)
target_compile_features(WizardClient PRIVATE cxx_std_17)
target_link_libraries(WizardClient PRIVATE
    SFML::Graphics
    SFML::Network
    glew32
)

add_custom_command(TARGET WizardClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/libraries/bin/glew32.dll  # Adjust this path to where your DLL is located
        $<TARGET_FILE_DIR:WizardClient>  # This copies it to the same directory as the executable
)

#set(SFML_BIN_DIR "C:/SFML/bin")  # Adjust to your SFML installation path

## Find all SFML DLLs
#file(GLOB SFML_DLLS "${SFML_BIN_DIR}/*.dll")

## Copy each DLL to the output folder after building
#foreach(DLL ${SFML_DLLS})
#    add_custom_command(TARGET MyApp POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL} $<TARGET_FILE_DIR:MyApp>
#    )
#endforeach()

add_executable(WizardServer src/HexagonServer.cpp src/PacketFunctions.cpp src/ConfigParser.cpp src/Tile.cpp src/WorldObject.cpp src/Game.cpp src/Board.cpp src/Player.cpp src/Fire.cpp)
target_compile_features(WizardServer PRIVATE cxx_std_17)
target_link_libraries(WizardServer PRIVATE
    SFML::Graphics
    SFML::Network
)



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