cmake_minimum_required(VERSION 3.10)

# set the project name
project(2dwater)

find_path(GLEW_INCLUDE_DIR GL/glew.h HINTS /opt/homebrew/include)

find_library(GLEW_LIBRARY GLEW)
find_library(FREEGLUT3_LIBRARY glut)
find_library(Z_LIBRARY z)
find_library(JPEG_LIBRARY jpeg)
find_library(PNG_LIBRARY png)
find_library(GL_LIBRARY GL)

include_directories(${GLEW_INCLUDE_DIR})

add_library(loadtga LoadTGA.cpp)

target_compile_features(metagoop PUBLIC cxx_std_17)
