export module GameEvents;

export import Event;

export class GamePausedEvent: public Event<GamePausedEvent>
{
public:
	bool paused{};
};