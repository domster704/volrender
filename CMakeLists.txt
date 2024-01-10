cmake_minimum_required(VERSION 3.8)
project(VolRender)

set(CMAKE_CXX_STANDARD 17)

find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
add_executable(VolRender
        main.cpp
)

target_link_libraries(VolRender
        OpenGL::GL
        GLUT::GLUT
        C:/ITSoft/vcpkg/packages/glew_x64-windows/bin/glew32.dll
)
