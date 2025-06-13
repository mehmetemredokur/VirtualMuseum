cmake_minimum_required(VERSION 3.10)
project(VirtualMuseum)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# =================== VCPKG Packages ===================
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# =================== Executable ===================
file(GLOB SRC_FILES
    src/*.cpp
    src/stb_image.cpp
)

add_executable(VirtualMuseum ${SRC_FILES})

# =================== Manual ImGui Backend Setup ===================
# Set path to local ImGui repo (cloned manually, not from vcpkg)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)

# Add backend implementation files (GLFW + OpenGL3)
set(IMGUI_BACKENDS
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add the backend source files to the build
target_sources(VirtualMuseum PRIVATE ${IMGUI_BACKENDS})

# Include paths for imgui and backends
target_include_directories(VirtualMuseum PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# =================== Link Libraries ===================
target_link_libraries(VirtualMuseum
  PRIVATE
    glfw
    assimp::assimp
    glm::glm
    glad::glad
    imgui::imgui
)

# =================== Project Includes ===================
target_include_directories(VirtualMuseum
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)
