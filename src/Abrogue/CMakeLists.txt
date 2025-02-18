add_executable(${PROJECT_NAME} main.cpp 
"RenderWindow.cpp" 
"helpers/Configuration.cpp" 
"helpers/Logger.cpp" 
"RenderEngine.cpp" 
"PhysicsComponent.cpp" 
"Enemy.cpp" 
"helpers/ImageLoader.cpp")
target_sources(${PROJECT_NAME} PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES 
${STANDARD_MODULE_PATH} "RenderEngine.ixx"  "RenderWindow.ixx" "helpers/Configuration.ixx" "helpers/Logger.ixx" "Game.ixx" "ObjectPools.ixx" "helpers/Constants.ixx" "Player.ixx" "Enemy.ixx" "PhysicsComponent.ixx" "helpers/ImageLoader.ixx")

target_link_directories(${PROJECT_NAME} PUBLIC lib)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} SDL3 vulkan-1)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 26)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_SCAN_FOR_MODULES ON)
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release,RelWithDebInfo,MinSizeRel>)