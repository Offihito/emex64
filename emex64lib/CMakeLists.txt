cmake_minimum_required(VERSION 3.16)

project(emex64lib
    VERSION 1.0.0
    LANGUAGES C
)

add_library(emex64lib STATIC
    src/bitwalker.c
    src/parser.c
    src/fdwalker.c
)

target_include_directories(emex64lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(emex64lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(emex64lib PUBLIC c_std_99)

if (MSVC)
    target_compile_options(emex64lib PRIVATE /W4)
else()
    target_compile_options(emex64lib PRIVATE -Wall -Wextra -Wpedantic)
endif()
