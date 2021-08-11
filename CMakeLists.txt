cmake_minimum_required(VERSION 3.20)
project(fractal_generator)

set(CMAKE_CXX_STANDARD 20)

set(BOOST_VERSION 1_67_0)
set(GLEW_VERSION 2.1.0)
set(SOURCE_FILES
        main.cpp
        classes/ogl_tools.cpp
        classes/obj_parser.cpp
        classes/color_manager.cpp
        classes/fractal_generator.cpp
        classes/geometry_generator.cpp
        classes/jeploot.cpp
        classes/random_generator.cpp
        classes/screencap.cpp
        classes/settings_manager.cpp
        dependencies/glm/glm/glm.hpp)

add_executable(artGen ${SOURCE_FILES})

find_package(Threads REQUIRED) #required for glfw

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(./dependencies/glfw/src)
target_link_libraries(artGen glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(artGen OpenGL::GL)

set(GLEW_INCLUDE_DIR ./dependencies/glew-2.1.0/include)
set(GLEW_LIBRARY ./dependencies/glew-2.1.0/lib/Release/Win32)

find_path(GLEW_INCLUDE_DIR GL/glew.h)
find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATH_SUFFIXES lib64)
set(CMAKE_PREFIX_PATH "./dependencies/glew-2.1.0")
set(CMAKE_LIBRARY_PATH "./dependencies/glew-2.1.0/lib/Release/Win32")
find_package(GLEW REQUIRED)

include_directories(C:/dev/git/fractal_generator/include)
include_directories(C:/dev/git/fractal_generator/dependencies/glm/glm)
include_directories(C:/dev/git/fractal_generator/dependencies/glfw/include)
include_directories(C:/dev/git/fractal_generator/dependencies/boost_${BOOST_VERSION})
include_directories(C:/dev/git/fractal_generator/dependencies/glew-${GLEW_VERSION}/include)