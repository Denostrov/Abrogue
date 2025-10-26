export module Helpers:Flags;

import std;
import :Concepts;

//Convert defines into constexpr flags
#ifdef NDEBUG
export inline constexpr bool isDebugBuild{false};
#else
export inline constexpr bool isDebugBuild{true};
#endif

#ifdef EXTRA_ASSERTS
export inline constexpr bool useExtraAsserts{true};
#else
export inline constexpr bool useExtraAsserts{false};
#endif

//Helper class for converting runtime flags into template arguments
export template <bool... Vals>
struct BoolSequence
{
    template <std::size_t I>
    static constexpr bool get()
    {
        return Vals...[I];
    }

    template <bool NewVal>
    static constexpr auto prepend() { return BoolSequence<NewVal, Vals...>{}; }
};

export class BoolFlag
{
public:
    template <class Func, IsBoolArray BoolArray, class... Args>
    static constexpr void call(Func&& func, BoolArray&& flags, Args&&... args)
    {
        convertFlags<flags.size()>(std::forward<Func>(func), std::forward<BoolArray>(flags), BoolSequence{}, std::forward<Args>(args)...);
    }

private:
    template <class Func, class ResultSequence, class... Args>
    static constexpr void callFunc(Func&& func, ResultSequence sequence, Args&&... args)
    {
        std::forward<Func>(func)(std::forward<Args>(args)..., sequence);
    }

    template <std::size_t ArraySize, class Func, IsBoolArray BoolArray, class ResultSequence, class... Args>
    static constexpr void convertFlags(Func&& func, BoolArray&& flags, ResultSequence sequence, Args&&... args)
    {
        if constexpr (ArraySize == 0)
        {
            callFunc(std::forward<Func>(func), sequence, std::forward<Args>(args)...);
        }
        else
        {
            if (flags[ArraySize - 1])
                convertFlags<ArraySize - 1>(std::forward<Func>(func), std::forward<BoolArray>(flags), sequence.template prepend<true>(),
                                            std::forward<Args>(args)...);
            else
                convertFlags<ArraySize - 1>(std::forward<Func>(func), std::forward<BoolArray>(flags), sequence.template prepend<false>(),
                                            std::forward<Args>(args)...);
        }
    }
};


