cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")

project(AbrogueFull LANGUAGES CXX)

if(EXISTS ${CMAKE_SOURCE_DIR}/../tools69)
    set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/../tools69)
    include_directories(SYSTEM ${CMAKE_PREFIX_PATH}/include)
    link_directories(${CMAKE_PREFIX_PATH}/lib)

    set(CMAKE_BUILD_RPATH \$ORIGIN ${CMAKE_SOURCE_DIR}/../tools69/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../dist/${CMAKE_BUILD_TYPE}/Abrogue)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=16777216")
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_STANDARD 26)

option(EXTRA_ASSERTS ON)

find_package(SDL3 REQUIRED)
add_library(SDL3Module)
target_sources(SDL3Module PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES src/SDL3.ixx)
target_link_libraries(SDL3Module PUBLIC SDL3::SDL3-shared)

find_package(Vulkan REQUIRED)
add_library(VulkanModule)
target_sources(VulkanModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES src/Vulkan.ixx)
target_link_libraries(VulkanModule PUBLIC Vulkan::Headers)

add_library(HelpersModule)
target_sources(HelpersModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
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
endif()

add_library(RenderEngineModule)
target_sources(RenderEngineModule PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES src/RenderEngine/RenderEngine.ixx)
target_link_libraries(RenderEngineModule HelpersModule VulkanModule)

add_executable(BitmapGenerator src/BitmapGenerator/main.cpp)
target_link_libraries(BitmapGenerator HelpersModule)

add_executable(Abrogue src/Abrogue/main.cpp)
target_sources(Abrogue PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES src/Abrogue/Abrogue.ixx)
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

configure_file(${CMAKE_PREFIX_PATH}/lib64/libstdc++.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
configure_file(${CMAKE_PREFIX_PATH}/lib64/libstdc++.so.6 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)