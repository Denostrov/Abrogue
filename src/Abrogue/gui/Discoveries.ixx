module Abrogue:Discoveries;

import :ScreenComponent;

//Enum for discovery labels
enum class DiscoveriesLabelType
{
    eTitle,
    COUNT
};

//Class for a popup menu with discovered items
class Discoveries : public ScreenComponent<Discoveries, DiscoveriesLabelType>
{
public:
    void init();
};