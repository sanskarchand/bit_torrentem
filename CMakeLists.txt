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
  parser.cpp
  torrent.cpp
  bt_network/network.cpp
  bt_utils/utils.cpp
  bt_utils/urlencode.cpp
  bt_utils/general_utils.cpp
  include/mainwindow.h
  include/parser.hpp
  include/torrent.hpp
  include/bt_network/network.hpp
  include/bt_utils/utils.hpp
  include/bt_utils/urlencode.hpp
  include/bt_utils/general_utils.hpp
)

target_link_libraries(BitTorrentem PRIVATE Qt5::Widgets)
