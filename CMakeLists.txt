cmake_minimum_required(VERSION 3.10)

project(SideProjectCardsGame)

set(CMAKE_CXX_STANDARD 17)

# Ajoutez cette ligne pour inclure la bibliothèque nlohmann/json
include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.10.5
)
FetchContent_MakeAvailable(json)

add_executable(SideProjectCardsGame main.cpp Card.cpp Player.cpp Game.cpp Fight.h Event.cpp Event.h Fight.cpp Artefact.cpp Artefact.h Artefact.h)

# Lier la bibliothèque à votre projet
target_link_libraries(SideProjectCardsGame PRIVATE nlohmann_json::nlohmann_json)
