project(engine CXX)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCE_FILES "*.h" "*.cpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ../../source)

add_dependencies(${PROJECT_NAME} shaders)

target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

target_link_libraries(${PROJECT_NAME} PUBLIC assimp)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC freetype)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC TracyClient)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Headers GPUOpen::VulkanMemoryAllocator VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ../../libraries/stb/stb)
