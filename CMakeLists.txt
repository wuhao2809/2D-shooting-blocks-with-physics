cmake_minimum_required(VERSION 3.10)
project(TopDownShooter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: Add SDL3 and Box2D integration

add_executable(TopDownShooter src/main.cpp)
