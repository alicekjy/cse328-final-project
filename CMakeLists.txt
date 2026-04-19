cmake_minimum_required(VERSION 3.10)
project(XToon)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

add_executable(xtoon
    src/main.cpp
    src/shader.cpp
    src/glad.c
    src/stb_image.cpp
)

target_include_directories(xtoon PRIVATE
    src/
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(xtoon
    OpenGL::GL
    glfw
    glm::glm
    assimp::assimp
)