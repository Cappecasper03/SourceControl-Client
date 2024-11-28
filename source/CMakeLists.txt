project(source)

set(CMAKE_FOLDER source)

if(MSVC)
    add_compile_options(-MP -W4 -WX)

    add_link_options($<$<CONFIG:Release>:/LTCG>)
else()
endif()

add_compile_definitions(VULKAN_HPP_NO_EXCEPTIONS)
add_compile_definitions(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC)
add_compile_definitions(VULKAN_NO_PROTOTYPES)
add_compile_definitions(VULKAN_HPP_NO_NODISCARD_WARNINGS)

add_compile_definitions(TRACY_ONLY_LOCALHOST)
add_compile_definitions(TRACY_VK_USE_SYMBOL_TABLE)

add_subdirectory(application)
add_subdirectory(engine)
add_subdirectory(shaders)
