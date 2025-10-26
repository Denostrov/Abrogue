export module Helpers:Concepts;

import std;

//Helper concepts
export template <class T, class U>
concept IsSameType = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

export template <class T>
concept IsLikeStringView = requires
{
    T(std::string_view{});
};

export template <class T>
concept IsLikePair = requires
{
    typename T::first_type;
    typename T::second_type;
};

export template <class T>
concept IsBoolArray = std::same_as<typename T::value_type, bool>;