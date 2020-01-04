cmake_minimum_required(VERSION 3.5)

project(BitTorrentem LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(BitTorrentem
  main.cpp
  mainwindow.cpp
  torrent.cpp
  include/mainwindow.h
  include/torrentinfo.h
)

target_link_libraries(BitTorrentem PRIVATE Qt5::Widgets)