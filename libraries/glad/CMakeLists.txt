project(glad)

file(GLOB_RECURSE INCLUDE_FILES "include/*")
file(GLOB_RECURSE SOURCE_FILES "source/*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${INCLUDE_FILES} ${SOURCE_FILES})

add_library(${PROJECT_NAME} ${INCLUDE_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "include" PUBLIC "source")
