export module Helpers;

export import SDL3;
export import std;
export import :Flags;
export import :Concepts;
export import :Logger;
export import :Array;
export import :Color;
export import :FixedString;
export import :FixedVector;
export import :Random;
export import :Tags;
export import :Literals;
#include <dlfcn.h>

// Helper template aliases
export template <class T>
using optRef = std::optional<std::reference_wrapper<T>>;
export template <class T>
using optCRef = std::optional<std::reference_wrapper<T const>>;

template <class Enum, template <Enum> class T>
class EnumTupleTrait
{
    template <std::size_t... I>
    static consteval auto getType(std::index_sequence<I...>)
    {
        return std::tuple<T<static_cast<Enum>(I)>...>{};
    }

public:
    using type = decltype(getType(std::make_index_sequence<static_cast<std::size_t>(Enum::COUNT)>{}));
};
export template <class Enum, template <Enum> class T>
using EnumTupleType = EnumTupleTrait<Enum, T>::type;

export template <class... Args>
class GlobalHandler
{
public:
    GlobalHandler(Args&... args) : globals(args...) {}

    void cleanup() { cleanupIndex<std::tuple_size_v<decltype(globals)> - 1>(); }

private:
    template <std::size_t Index>
    void cleanupIndex()
    {
        destructGlobal(std::get<Index>(globals));
        if constexpr (Index > 0)
            cleanupIndex<Index - 1>();
    }

    template <class T>
    static void destructGlobal(T& obj)
    {
        obj.~T();
        new (&obj) T();
    }

    std::tuple<Args&...> globals;
};

export class LibraryLoader
{
public:
    LibraryLoader() = default;

    [[nodiscard]] bool init(char const* libraryName)
    {
        library = dlopen(libraryName, RTLD_LOCAL | RTLD_NOW);
        return library;
    }

    void cleanup()
    {
        dlclose(library);
    }

    template<class Func>
    [[nodiscard]] Func getFunction(char const* functionName)
    {
        return (Func)dlsym(library, functionName);
    }

private:
    void* library{};
};
