project(imgui)

file(GLOB SOURCE_FILES "imgui/*.h" "imgui/*.cpp")
file(GLOB_RECURSE MISC_FILES "imgui/misc/*.h" "imgui/misc/*.cpp")
file(GLOB BACKEND_FILES "imgui/backends/*glfw*" "imgui/backends/*opengl3*" "imgui/backends/*vulkan*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/imgui FILES ${SOURCE_FILES} ${MISC_FILES} ${BACKEND_FILES})

add_library(${PROJECT_NAME} ${SOURCE_FILES} ${MISC_FILES} ${BACKEND_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC imgui PUBLIC imgui/backends)

target_link_libraries(${PROJECT_NAME} PRIVATE freetype)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
