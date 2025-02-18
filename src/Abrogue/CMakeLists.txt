add_executable(${PROJECT_NAME} 
	main.cpp 
	"helpers/Configuration.cpp" 
	"helpers/Logger.cpp" 
	"helpers/ImageLoader.cpp"

	"RenderEngine.cpp" 
	"RenderWindow.cpp" 
	"PhysicsComponent.cpp" 
	"Enemy.cpp")
target_sources(${PROJECT_NAME} PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES 
	${STANDARD_MODULE_PATH} 
	"helpers/Configuration.ixx" 
	"helpers/Logger.ixx" 
	"helpers/ImageLoader.ixx"
	"helpers/Constants.ixx" 

	"RenderEngine.ixx"  
	"RenderWindow.ixx" 
	"PhysicsComponent.ixx"
	"Enemy.ixx" 

	"Game.ixx" 
	"ObjectPools.ixx" 
	"Player.ixx")

target_link_directories(${PROJECT_NAME} PUBLIC ${ABROGUE_LIB_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${ABROGUE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} SDL3 vulkan-1)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ABROGUE_BIN_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 26)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_SCAN_FOR_MODULES ON)
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release,RelWithDebInfo,MinSizeRel>)