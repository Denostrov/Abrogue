export module GUI;

export import Menu;
export import Discoveries;
export import DebugMenu;

export class GUI
{
public:
	enum ButtonType
	{
		eStartGame,
		eQuitToDesktop,
		ePause,
		eHealth,
		eNutrition,
		COUNT
	};

	enum class TabButton
	{
		eDebug,
		eSearch,
		eDiscoveries,
		eMenu,
		COUNT
	};

	enum class State
	{
		eMainMenu,
		eNormal,
		eDebug,
		eDiscoveries,
		eMenu
	};

	GUI() = default;

	void init();

	void showStartMenu();
	void startGame();
	void quitToMenu();

	void onMouseMoved(std::uint32_t x, std::uint32_t y);
	void onMousePressed(std::uint32_t x, std::uint32_t y);

	void togglePause();
	void toggleMenu() { setPressedTabButton(pressedTabButton == TabButton::COUNT ? TabButton::eMenu : TabButton::COUNT); }
	void toggleDiscoveries() { setPressedTabButton(pressedTabButton == TabButton::eDiscoveries ? TabButton::COUNT : TabButton::eDiscoveries); }
	void toggleDebugOptions() { setPressedTabButton(pressedTabButton == TabButton::eDebug ? TabButton::COUNT : TabButton::eDebug); }

	void setFPS(std::uint32_t fps);

private:
	void onButtonPressed(ButtonType type);
	void setPressedTabButton(TabButton type);
	void setPaused(bool paused);

	template<class T>
	void updateMouseMoved(T& element, std::uint32_t x, std::uint32_t y)
	{
		auto updateHoveredButton = [this](Label* newHoveredButton)
		{
			if(newHoveredButton != hoveredButton)
			{
				if(hoveredButton)
					hoveredButton->setHovered(false);
				hoveredButton = newHoveredButton;
			}
		};

		auto& checkedButtons = element.getButtons();

		for(size_t i = 0; i < checkedButtons.size(); i++)
		{
			if(checkedButtons[i].checkCollision(x, y))
			{
				checkedButtons[i].setHovered(true);
				updateHoveredButton(&checkedButtons[i]);
				return;
			}
		}
		updateHoveredButton(nullptr);
	}
	template<class T>
	void updateMousePressed(T& element, std::uint32_t x, std::uint32_t y)
	{
		auto& checkedButtons = element.getButtons();

		for(size_t i = 0; i < checkedButtons.size(); i++)
		{
			if(checkedButtons[i].checkCollision(x, y))
			{
				element.onButtonPressed(static_cast<T::ButtonType>(i));
				return;
			}
		}
	}

	State state{State::eMainMenu};

	std::array<Label, COUNT> buttons;
	std::array<Label, (size_t)TabButton::COUNT> tabButtons;
	Label* hoveredButton{};
	TabButton pressedTabButton{TabButton::COUNT};

	Menu menu;
	Discoveries discoveries;
	DebugMenu debugMenu;

	Label fpsLabel;
};