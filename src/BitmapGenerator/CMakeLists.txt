project(BitmapGenerator)

add_executable(${PROJECT_NAME} main.cpp)
target_sources(${PROJECT_NAME} PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES ${STANDARD_MODULE_PATH})
target_include_directories(${PROJECT_NAME} PUBLIC include)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 26)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_SCAN_FOR_MODULES ON)