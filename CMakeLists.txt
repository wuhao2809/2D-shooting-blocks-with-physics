cmake_minimum_required(VERSION 3.10)
project(TopDownShooter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use SDL3 CMake config package (Homebrew supports it)
find_package(SDL3 CONFIG REQUIRED)

# Find Box2D
find_package(box2d REQUIRED)

# JSON (manually locate header-only lib)
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp HINTS /opt/homebrew/include /usr/local/include)
if(NOT NLOHMANN_JSON_INCLUDE_DIR)
    message(FATAL_ERROR "nlohmann/json.hpp not found. Please install with 'brew install nlohmann-json'")
endif()

add_executable(TopDownShooter
    src/main.cpp
    src/core/Manager.cpp
    src/core/Blackboard.cpp
    src/core/GameEngine.cpp
    src/input/InputSystem.cpp
    src/movement/MovementSystem.cpp
    src/gameplay/ShootingSystem.cpp
    src/physics/PhysicsSystem.cpp
    src/map/MapSystem.cpp
    src/rendering/Renderer.cpp
    src/rendering/RenderingSystem.cpp
    src/rendering/HUDSystem.cpp
)

target_include_directories(TopDownShooter PRIVATE 
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

target_link_libraries(TopDownShooter PRIVATE 
    SDL3::SDL3
    box2d::box2d
)
