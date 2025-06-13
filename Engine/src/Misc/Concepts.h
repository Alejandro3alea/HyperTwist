#pragma once
#include <concepts>

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;