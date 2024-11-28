project(stb)

file(GLOB SOURCE_FILES "stb/*.h")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/stb FILES ${SOURCE_FILES})

add_custom_target(${PROJECT_NAME} SOURCES ${SOURCE_FILES})
