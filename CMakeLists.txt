cmake_minimum_required(VERSION 3.5)

project(BitTorrentem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# included conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")
file(GLOB_RECURSE HEADERS2 "src/*.h")

add_executable(BitTorrentem ${SOURCES} ${HEADERS} ${HEADERS2})

target_link_libraries(BitTorrentem PRIVATE Qt5::Widgets ${CONAN_LIBS})
