add_executable(${PROJECT_NAME} 
	main.cpp 
	"helpers/Configuration.cpp" 
	"helpers/Logger.cpp" 
	"helpers/ImageLoader.cpp"

	"RenderEngine.cpp" 
	"RenderWindow.cpp" 
	"PhysicsComponent.cpp" 
	"Enemy.cpp" 
	"Game.cpp" 
	"Map.cpp" 
	"Player.cpp" 
	"QuadPool.cpp" 
	"InputHandler.cpp"
	
	"gui/GUI.cpp" 
	"gui/Label.cpp" 
	"gui/Menu.cpp" 
	"gui/Discoveries.cpp" 
	"gui/DebugMenu.cpp"  
	"gui/PlayArea.cpp" 
	"gui/MainMenu.cpp"
	"gui/Screen.cpp"
 "Weapon.cpp" "gui/GameOver.cpp" "gui/Background.cpp")
target_sources(${PROJECT_NAME} PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${ABROGUE_BASE_DIR} FILES 
	${STANDARD_MODULE_PATH} 
	"helpers/Configuration.ixx" 
	"helpers/Logger.ixx" 
	"helpers/ImageLoader.ixx"
	"helpers/Constants.ixx" 
	"helpers/Event.ixx"

	"RenderEngine.ixx"  
	"RenderWindow.ixx" 
	"PhysicsComponent.ixx"
	"Enemy.ixx" 
	"Game.ixx" 
	"QuadPool.ixx" 
	"Player.ixx" 
	"Map.ixx" 
	"GameSystems.ixx"
	"GameEvents.ixx"
	"InputHandler.ixx"

	"gui/Label.ixx" 
	"gui/GUI.ixx"    
	"gui/Menu.ixx" 
	"gui/Discoveries.ixx" 
	"gui/DebugMenu.ixx"   
	"gui/PlayArea.ixx" 
	"gui/MainMenu.ixx"
	"gui/GameOver.ixx"
	"gui/Screen.ixx"
 "Weapon.ixx" "gui/Background.ixx")

target_link_directories(${PROJECT_NAME} PUBLIC ${ABROGUE_LIB_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${ABROGUE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} SDL3 vulkan-1)

if(EXTRA_ASSERTS)
	target_compile_definitions(${PROJECT_NAME} PRIVATE EXTRA_ASSERTS)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ABROGUE_BIN_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 26)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_SCAN_FOR_MODULES ON)

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release,RelWithDebInfo,MinSizeRel>)
endif()

if(MSVC)
	target_link_options(${PROJECT_NAME} PRIVATE /STACK:8388608)
endif()