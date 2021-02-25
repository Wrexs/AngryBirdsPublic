cmake_minimum_required(VERSION 3.11.4)
project(AngryBirds)
set(GAMEDATA_FOLDER "data")
set(ENABLE_ENET         OFF CACHE BOOL "Adds Networking"   FORCE)
set(ENABLE_SOUND        OFF CACHE BOOL "Adds SoLoud Audio" FORCE)
set(ENABLE_JSON         OFF CACHE BOOL "Adds JSON to the Project" FORCE)
set(ENABLE_CLANG_FORMAT ON  CACHE BOOL "Adds FormatYaCode" FORCE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

## out of source builds ##
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
## out of source builds ##

## add the files to be compiled here
set(SOURCE_FILES
        "game/main.cpp"
        "game/game.cpp"
        "game/GameObjects/GameObject.cpp"
        "game/Components/SpriteComponent.cpp")

set(HEADER_FILES
        "game/game.h"
        "game/GameObjects/GameObject.h"

        "game/Components/SpriteComponent.h"
        game/GameObjects/BirdObjects.cpp game/GameObjects/BirdObjects.h game/GameObjects/EnemyObjects.cpp game/GameObjects/EnemyObjects.h)

## the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

## lets us find the source files
target_include_directories(
        ${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/game")

## logging levels
target_compile_definitions(
        ${PROJECT_NAME}
        PRIVATE
        "$<$<CONFIG:Debug>:LOGGING_LEVEL_DEBUG>"
        "$<$<CONFIG:Release>:LOGGING_LEVEL_ERROR>")

## these are the build directories
get_target_property(CLIENT ${PROJECT_NAME} NAME)
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${CLIENT}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${CLIENT}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${CLIENT}/bin")

## important build scripts
include(build/compilation)
include(libs/asge)
include(libs/json)
include(libs/soloud)
include(tools/itch.io)

## hide console unless debug build ##
if (NOT CMAKE_BUILD_TYPE STREQUAL  "Debug" AND WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -mwindows)
endif()


