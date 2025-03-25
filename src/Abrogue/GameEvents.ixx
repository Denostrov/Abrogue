export module GameEvents;

export import Event;

export class KeyPausedEvent: public Event<KeyPausedEvent>
{};

export class KeyDebugMenuEvent: public Event<KeyDebugMenuEvent>
{};

export class KeyDiscoveriesEvent: public Event<KeyDiscoveriesEvent>
{};

export class KeyMenuEvent: public Event<KeyMenuEvent>
{};