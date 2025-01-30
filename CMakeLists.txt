cmake_minimum_required(VERSION 3.30)

project(Abrogue LANGUAGES CXX)

add_executable(${PROJECT_NAME} main.cpp "RenderWindow.cpp" "Configuration.cpp")
target_sources(${PROJECT_NAME} PUBLIC FILE_SET modules TYPE CXX_MODULES FILES "RenderEngine.ixx" "std.ixx" "RenderWindow.ixx" "Configuration.ixx")

target_link_directories(${PROJECT_NAME} PUBLIC lib)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} SDL3 vulkan-1)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 26)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_SCAN_FOR_MODULES ON)
configure_file(lib/SDL3.dll ${CMAKE_CURRENT_BINARY_DIR}/bin COPYONLY)