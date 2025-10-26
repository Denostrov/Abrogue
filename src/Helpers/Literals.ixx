export module Helpers:Literals;

import std;

export constexpr std::int64_t operator""_i64(unsigned long long int val) { return val; }
export constexpr std::uint64_t operator""_u64(unsigned long long int val) { return val; }