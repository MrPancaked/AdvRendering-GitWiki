cmake_minimum_required(VERSION 3.29)
project(RawEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(RawEngine main.cpp)

add_custom_command(
        TARGET RawEngine PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders/
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/)

add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders
        DEPENDS ${SHADER_FILES}
        COMMENT "Copying shaders..."
)

add_dependencies(RawEngine copy_shaders)

find_package(glad CONFIG REQUIRED)
target_link_libraries(RawEngine PRIVATE glad::glad)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(RawEngine PRIVATE glfw)
find_package(glm CONFIG REQUIRED)
target_link_libraries(RawEngine PRIVATE glm::glm)