cmake_minimum_required(VERSION 3.31.0)

project(DragonForge-Engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(USE_FOLDERS true)

if(MSVC)
    add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

    add_compile_definitions($<$<CONFIG:Release>:NDEBUG>)
    add_compile_definitions($<$<CONFIG:Release>:RELEASE>)
else()
endif()

add_subdirectory(libraries)

add_subdirectory(source)
