module Abrogue:GameOver;

import :ScreenComponent;

//Enum for game over labels
enum class GameOverLabelType
{
    eGameOver,
    ePressSpace,
    COUNT
};

//Class for a popup screen with game over text
class GameOver : public ScreenComponent<GameOver, GameOverLabelType>
{
public:
    void init();

    void setWinner(bool winner);

    void refreshLabels();
};