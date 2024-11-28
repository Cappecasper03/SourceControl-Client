project(shaders)

file(GLOB_RECURSE OPENGL_FILES "opengl/*.vert" "opengl/*.frag")
file(GLOB_RECURSE VULKAN_FILES "vulkan/*.vert" "vulkan/*.frag")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${OPENGL_FILES} ${VULKAN_FILES})

add_custom_target(${PROJECT_NAME} SOURCES ${OPENGL_FILES} ${VULKAN_FILES})

foreach(SHADER_FILE ${OPENGL_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_FILE} ${CMAKE_SOURCE_DIR}/game/binaries/shaders/opengl/${SHADER_NAME}
        COMMENT "OpenGL: ${SHADER_NAME}"
    )
endforeach()

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/game/binaries/shaders/vulkan
)

foreach(SHADER_FILE ${VULKAN_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER_FILE} -o ${CMAKE_SOURCE_DIR}/game/binaries/shaders/vulkan/${SHADER_NAME}.spv
        COMMENT "Vulkan: ${SHADER_NAME}.spv"
    )
endforeach()
