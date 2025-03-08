export module Event;

export import Logger;

export template<class DerivedEvent>
class Event
{
public:
	class Handler
	{
	public:
		Handler(bool enabled = false)
			:enabled(enabled)
		{
			if(!enabled) 
				return;

			Logger::assert(size < maxHandlers, "Exceeded max event handlers");

			handlers[size] = this;
			size++;
		}
		~Handler()
		{
			if(!enabled) 
				return;

			std::int64_t erasedIndex{-1};
			for(size_t i = 0; i < size; i++)
			{
				if(handlers[i] == this)
					erasedIndex = i;
			}
			Logger::assert(erasedIndex != -1, "Erased event handler doesn't exist");

			std::swap(handlers[erasedIndex], handlers[size - 1]);
			size--;
		}

		virtual void handleEvent(DerivedEvent const& event) = 0;

	private:
		bool enabled{};
	};

private:
	static constexpr size_t maxHandlers{16};
	inline static Handler* handlers[maxHandlers];
	inline static size_t size{};
};