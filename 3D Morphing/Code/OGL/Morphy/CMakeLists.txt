cmake_minimum_required(VERSION 3.16) # Use a more recent CMake version for FetchContent

# set the project name
project(Morphy CXX) # Specify CXX language for project


# Add your library
add_library(skeleton STATIC Skeleton.cpp LoadOBJ.cpp) # Use STATIC for a static library on Windows by default

# Set C++ standard
target_compile_features(skeleton PUBLIC cxx_std_17)

target_compile_definitions(skeleton PRIVATE _AFXDLL)

# Link all collected libraries to your skeleton library
target_link_libraries(skeleton PUBLIC ${MORPHY_LINK_LIBS})

add_executable(morphy Morphy.cpp MainFrm.cpp)

target_link_libraries(morphy PRIVATE skeleton)

target_compile_definitions(morphy PRIVATE _AFXDLL)

set_target_properties(morphy PROPERTIES
    WIN32_EXECUTABLE TRUE
)


# For includes, targets usually handle their own include directories automatically
# when using FetchContent and target_link_libraries.
# However, for explicit control or when finding libraries, you might still need:
# target_include_directories(skeleton PUBLIC ...)