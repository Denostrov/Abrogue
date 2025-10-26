module Abrogue:OptionsMenuImpl;

import :OptionsMenu;
import :InputHandler;
import :RenderWindow;
import :Configuration;

using namespace std::literals;

void OptionsMenu::init()
{
	using enum ButtonType;

	labels[LabelType::eControls].init("Controls"sv, 60, 5);
	labels[LabelType::eVideo].init("Video"sv, 82, 5);
	labels[LabelType::eResolution].init(""sv, 80, 7);

	buttons[eMoveUp].init(""sv, 58, 7);
	buttons[eMoveDown].init(""sv, 58, 9);
	buttons[eMoveLeft].init(""sv, 58, 11);
	buttons[eMoveRight].init(""sv, 58, 13);
	buttons[eAttack].init(""sv, 58, 15);
	buttons[ePause].init(""sv, 58, 17);
	buttons[eSearch].init(""sv, 58, 19);
	buttons[eDiscoveries].init(""sv, 58, 21);
	buttons[eDebug].init(""sv, 58, 23);
	buttons[eStopTime].init(""sv, 58, 25);
	buttons[eStepTime].init(""sv, 58, 27);

	buttons[eResetToDefault].init("Reset To Default"sv, 58, 32);

	buttons[eFullscreen].init(""sv, 78, 9);

	refreshLabels();
}

void OptionsMenu::onButtonPressed(ButtonType type)
{
	using enum ButtonType;

	if(type <= eStepTime)
		buttons[type].setPressed(true);

	if(type == eMoveUp)
		inputHandler.setChangingControlType(InputControlType::eMoveUp);
	else if(type == eMoveDown)
		inputHandler.setChangingControlType(InputControlType::eMoveDown);
	else if(type == eMoveLeft)
		inputHandler.setChangingControlType(InputControlType::eMoveLeft);
	else if(type == eMoveRight)
		inputHandler.setChangingControlType(InputControlType::eMoveRight);
	else if(type == eAttack)
		inputHandler.setChangingControlType(InputControlType::eAttack);
	else if(type == ePause)
		inputHandler.setChangingControlType(InputControlType::ePause);
	else if(type == eSearch)
		inputHandler.setChangingControlType(InputControlType::eSearch);
	else if(type == eDiscoveries)
		inputHandler.setChangingControlType(InputControlType::eDiscoveries);
	else if(type == eDebug)
		inputHandler.setChangingControlType(InputControlType::eDebug);
	else if(type == eStopTime)
		inputHandler.setChangingControlType(InputControlType::eStopTime);
	else if(type == eStepTime)
		inputHandler.setChangingControlType(InputControlType::eStepTime);
	else if(type == eResetToDefault)
		configuration.resetInputControlsToDefault();
	else if(type == eFullscreen)
	{
		auto isFullscreen = configuration.getIsFullscreen();
		if(isFullscreen)
			renderWindow.setIsMaximized(configuration.getIsMaximized());

		renderWindow.setIsFullscreen(!isFullscreen);
	}
}

void OptionsMenu::refreshLabels()
{
	using enum ButtonType;

	FixedString<32> labelText;

	buttons[eMoveUp].setText(labelText.fill("Move Up"sv, configuration.getInputControlName(InputControlType::eMoveUp)));
	buttons[eMoveDown].setText(labelText.fill("Move Down"sv, configuration.getInputControlName(InputControlType::eMoveDown)));
	buttons[eMoveLeft].setText(labelText.fill("Move Left"sv, configuration.getInputControlName(InputControlType::eMoveLeft)));
	buttons[eMoveRight].setText(labelText.fill("Move Right"sv, configuration.getInputControlName(InputControlType::eMoveRight)));
	buttons[eAttack].setText(labelText.fill("Attack"sv, configuration.getInputControlName(InputControlType::eAttack)));
	buttons[ePause].setText(labelText.fill("Pause"sv, configuration.getInputControlName(InputControlType::ePause)));
	buttons[eSearch].setText(labelText.fill("Search"sv, configuration.getInputControlName(InputControlType::eSearch)));
	buttons[eDiscoveries].setText(labelText.fill("Discoveries"sv, configuration.getInputControlName(InputControlType::eDiscoveries)));
	buttons[eDebug].setText(labelText.fill("Debug"sv, configuration.getInputControlName(InputControlType::eDebug)));
	buttons[eStopTime].setText(labelText.fill("Stop Time"sv, configuration.getInputControlName(InputControlType::eStopTime)));
	buttons[eStepTime].setText(labelText.fill("Step Time"sv, configuration.getInputControlName(InputControlType::eStepTime)));

	auto [windowWidth, windowHeight] = renderWindow.getWindowSize();
	labels[LabelType::eResolution].setText(labelText.format("{}x{}", windowWidth, windowHeight));
	buttons[eFullscreen].setText(renderWindow.getIsFullscreen() ? "[X]Fullscreen"sv : "[ ]Fullscreen"sv);

	for(auto i = (std::size_t)eMoveUp; i <= (std::size_t)eStepTime; i++)
		buttons[i].setPressed(false);
}
