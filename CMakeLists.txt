cmake_minimum_required(VERSION 3.10)
project(Tanki)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(Tanki
    src/main.cpp
    src/App.cpp
    src/App.hpp
    src/UI.cpp
    src/UI.hpp
    src/Deck.cpp
    src/Deck.hpp
    src/Card.cpp
    src/Card.hpp
    src/SM2Scheduler.cpp
    src/SM2Scheduler.hpp
    src/FileManager.cpp
    src/FileManager.hpp
    src/Stats.cpp
    src/Stats.hpp
)

target_link_libraries(Tanki ${CURSES_LIBRARIES})
