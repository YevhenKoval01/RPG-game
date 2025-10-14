cmake_minimum_required(VERSION 3.27)
project(My2DGame)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS FALSE)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY  https://github.com/SFML/SFML
        GIT_TAG         2.6.1
)

FetchContent_MakeAvailable(SFML)

add_executable(My2DGame main.cpp)
target_link_libraries(My2DGame sfml-graphics)