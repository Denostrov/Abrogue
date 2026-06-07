cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(AbrogueFull LANGUAGES CXX)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=16777216")

set(CMAKE_CXX_MODULE_STD 1)

set(ABROGUE_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ABROGUE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ABROGUE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(ABROGUE_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)

option(EXTRA_ASSERTS ON)

add_library(SDL3Module)
target_sources(SDL3Module PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES ${ABROGUE_BASE_DIR}/src/SDL3.ixx)
set_target_properties(SDL3Module PROPERTIES CXX_STANDARD 26)
target_include_directories(SDL3Module PUBLIC ${ABROGUE_INCLUDE_DIR})
target_link_directories(SDL3Module PUBLIC ${ABROGUE_LIB_DIR})
target_link_libraries(SDL3Module SDL3)

add_library(JSONModule)
target_sources(JSONModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES ${ABROGUE_BASE_DIR}/src/JSON.ixx)
set_target_properties(JSONModule PROPERTIES CXX_STANDARD 26)
target_include_directories(JSONModule PUBLIC ${ABROGUE_INCLUDE_DIR})

add_library(STBModule)
target_sources(STBModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES ${ABROGUE_BASE_DIR}/src/ImageLoader.ixx)
set_target_properties(STBModule PROPERTIES CXX_STANDARD 26)
target_include_directories(STBModule PUBLIC ${ABROGUE_INCLUDE_DIR})

add_library(GLMModule)
target_sources(GLMModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES ${ABROGUE_BASE_DIR}/src/GLM.ixx)
set_target_properties(GLMModule PROPERTIES CXX_STANDARD 26)
target_include_directories(GLMModule PUBLIC ${ABROGUE_INCLUDE_DIR})

add_library(HelpersModule)
target_sources(HelpersModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES
        src/Helpers/Helpers.ixx
        src/Helpers/Concepts.ixx
        src/Helpers/Flags.ixx
        src/Helpers/Logger.ixx
        src/Helpers/Array.ixx
        src/Helpers/Color.ixx
        src/Helpers/FixedString.ixx
        src/Helpers/FixedVector.ixx
        src/Helpers/Random.ixx
        src/Helpers/Tags.ixx
        src/Helpers/Literals.ixx
)
set_target_properties(HelpersModule PROPERTIES CXX_STANDARD 26)
target_link_libraries(HelpersModule SDL3Module stdc++exp)
if (EXTRA_ASSERTS)
    target_compile_definitions(HelpersModule PUBLIC EXTRA_ASSERTS)
endif ()

add_library(RenderEngineModule)
target_sources(RenderEngineModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES
        src/RenderEngine/RenderEngine.ixx
)
set_target_properties(RenderEngineModule PROPERTIES CXX_STANDARD 26)
target_link_libraries(RenderEngineModule HelpersModule GLMModule STBModule)

add_executable(BitmapGenerator src/BitmapGenerator/main.cpp)
target_include_directories(BitmapGenerator PUBLIC ${ABROGUE_INCLUDE_DIR})
set_target_properties(BitmapGenerator PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ABROGUE_BIN_DIR})
set_target_properties(BitmapGenerator PROPERTIES CXX_STANDARD 26)
target_link_libraries(BitmapGenerator HelpersModule)

add_executable(Abrogue src/Abrogue/main.cpp)
target_include_directories(Abrogue PUBLIC ${ABROGUE_INCLUDE_DIR})
set_target_properties(Abrogue PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ABROGUE_BIN_DIR})
set_target_properties(Abrogue PROPERTIES CXX_STANDARD 26)
target_sources(Abrogue PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES
        src/Abrogue/Abrogue.ixx
)
target_link_directories(Abrogue PUBLIC ${ABROGUE_LIB_DIR})
target_link_libraries(Abrogue JSONModule RenderEngineModule)
target_compile_options(Abrogue PRIVATE -fno-module-lazy)

file(MAKE_DIRECTORY ${ABROGUE_BIN_DIR})
configure_file(data.json ${ABROGUE_BIN_DIR} COPYONLY)

file(MAKE_DIRECTORY ${ABROGUE_BIN_DIR}/shaders)
configure_file(shaders/quad.vert ${ABROGUE_BIN_DIR}/shaders COPYONLY)
configure_file(shaders/quad.frag ${ABROGUE_BIN_DIR}/shaders COPYONLY)
configure_file(shaders/quadVert.spv ${ABROGUE_BIN_DIR}/shaders COPYONLY)
configure_file(shaders/quadFrag.spv ${ABROGUE_BIN_DIR}/shaders COPYONLY)

file(MAKE_DIRECTORY ${ABROGUE_BIN_DIR}/textures)
configure_file(textures/tiles.png ${ABROGUE_BIN_DIR}/textures COPYONLY)
