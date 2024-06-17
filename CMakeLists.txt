cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

if(MSVC)
    add_definitions(-D_WIN32_WINNT=0x600)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_CXX_STANDARD 17)
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_ENABLE_INSTALL ON)

find_package(Threads REQUIRED)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    GIT_TAG        v1.62.0
)
FetchContent_MakeAvailable(grpc)

# Since FetchContent uses add_subdirectory under the hood, we can use
# the grpc targets directly from this build.
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_REFLECTION grpc++_reflection)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP grpc++)
if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
endif()

# Proto file
get_filename_component(hw_proto "src/proto/gameservice.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/gameservice.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/gameservice.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/gameservice.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/gameservice.grpc.pb.h")
add_custom_command(
    OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${hw_proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${hw_proto}"
    DEPENDS "${hw_proto}"
)

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(hw_grpc_proto
    ${hw_grpc_srcs}
    ${hw_grpc_hdrs}
    ${hw_proto_srcs}
    ${hw_proto_hdrs}
)
target_link_libraries(hw_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
)

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


add_executable(WizardServer src/main.cpp src/ConfigParser.cpp src/Tile.cpp src/Graphics.cpp src/Game.cpp src/Tilesprite.cpp src/EntitySprite.cpp src/Board.cpp src/Player.cpp)
target_link_libraries(WizardServer
    hw_grpc_proto
    absl::flags
    absl::flags_parse
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
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