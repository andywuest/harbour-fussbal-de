cmake_minimum_required(VERSION 3.16)
project(fussball_decoder VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(fussball_decoder
    main.cpp
    fontdecoder.cpp
    fontdecoder.h
    agl_data.h
    post_names.h
)

target_link_libraries(fussball_decoder
    Qt5::Widgets
    Qt5::Network
    ZLIB::ZLIB
)

add_executable(decode_font
    decode_font_cli.cpp
    fontdecoder.cpp
    fontdecoder.h
    agl_data.h
    post_names.h
)

target_link_libraries(decode_font
    Qt5::Core
    Qt5::Network
    ZLIB::ZLIB
)