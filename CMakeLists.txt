cmake_minimum_required(VERSION 3.10)

project(Abrogue)

add_executable(${PROJECT_NAME} main.cpp)
target_link_directories(${PROJECT_NAME} PUBLIC lib)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} SDL3 vulkan-1)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)
configure_file(lib/SDL3.dll bin COPYONLY)