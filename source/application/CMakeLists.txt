project(application CXX)

file(GLOB_RECURSE SOURCE_FILES "*.h" "*.cpp" "*.rc")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/game/binaries/$<0:>)

add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ../application)

target_link_libraries(${PROJECT_NAME} PRIVATE engine)
