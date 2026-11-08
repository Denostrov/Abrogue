cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")

project(AbrogueFull LANGUAGES CXX)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=16777216")
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_STANDARD 26)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../dist/Abrogue)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(CMAKE_BUILD_RPATH \$ORIGIN)

set(ABROGUE_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ABROGUE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ABROGUE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

option(EXTRA_ASSERTS ON)

add_library(SDL3Module)
target_sources(SDL3Module PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES ${ABROGUE_BASE_DIR}/src/SDL3.ixx)
target_include_directories(SDL3Module PUBLIC ${ABROGUE_INCLUDE_DIR})
target_link_directories(SDL3Module PUBLIC ${ABROGUE_LIB_DIR})
target_link_libraries(SDL3Module SDL3)

add_library(STBModule)
target_sources(STBModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES ${ABROGUE_BASE_DIR}/src/ImageLoader.ixx)
target_include_directories(STBModule PUBLIC ${ABROGUE_INCLUDE_DIR})

add_library(GLMModule)
target_sources(GLMModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES ${ABROGUE_BASE_DIR}/src/GLM.ixx)
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
target_link_libraries(HelpersModule SDL3Module stdc++exp)
if (EXTRA_ASSERTS)
    target_compile_definitions(HelpersModule PUBLIC EXTRA_ASSERTS)
endif ()

add_library(RenderEngineModule)
target_sources(RenderEngineModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES
        src/RenderEngine/RenderEngine.ixx
)
target_link_libraries(RenderEngineModule HelpersModule GLMModule STBModule)

add_executable(BitmapGenerator src/BitmapGenerator/main.cpp)
target_include_directories(BitmapGenerator PUBLIC ${ABROGUE_INCLUDE_DIR})
target_link_libraries(BitmapGenerator HelpersModule)

add_executable(Abrogue src/Abrogue/main.cpp)
target_include_directories(Abrogue PUBLIC ${ABROGUE_INCLUDE_DIR})
target_sources(Abrogue PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES
        src/Abrogue/Abrogue.ixx
)
target_link_directories(Abrogue PUBLIC ${ABROGUE_LIB_DIR})
target_link_libraries(Abrogue RenderEngineModule)
target_compile_options(Abrogue PRIVATE -fno-module-lazy)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
configure_file(data.dat ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders)
configure_file(shaders/quad.vert ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders COPYONLY)
configure_file(shaders/quad.frag ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders COPYONLY)
configure_file(shaders/quadVert.spv ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders COPYONLY)
configure_file(shaders/quadFrag.spv ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders COPYONLY)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures)
configure_file(textures/tiles.png ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures COPYONLY)
