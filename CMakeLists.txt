cmake_minimum_required(VERSION 3.30)

project(Abrogue LANGUAGES CXX)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/bin)
configure_file(lib/SDL3.dll ${CMAKE_CURRENT_BINARY_DIR}/bin COPYONLY)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/bin/textures)
configure_file(textures/tiles.png ${CMAKE_CURRENT_BINARY_DIR}/bin/textures COPYONLY)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/bin/shaders)
configure_file(shaders/quad.vert ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders COPYONLY)
configure_file(shaders/quad.frag ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders COPYONLY)
configure_file(shaders/quadVert.spv ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders COPYONLY)
configure_file(shaders/quadFrag.spv ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders COPYONLY)

set(ABROGUE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(ABROGUE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ABROGUE_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(ABROGUE_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(STANDARD_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/std.ixx)

add_subdirectory(src/BitmapGenerator)
add_subdirectory(src/Abrogue)





