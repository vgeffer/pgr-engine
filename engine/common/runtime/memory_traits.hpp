#pragma once
#include <cstdint>
#include <limits>
#include <type_traits>

typedef std::integral_constant<uint32_t, 4096>       blocks;
typedef std::integral_constant<uint32_t, 1024>       kilobytes;
typedef std::integral_constant<uint32_t, 1048576>    megabytes;
typedef std::integral_constant<uint32_t, 1073741824> gigabytes;

template <uint32_t Val, typename Unit = std::integral_constant<uint32_t, 1>> 
struct size { 
    static_assert(static_cast<std::size_t>(Val) * static_cast<std::size_t>(Unit::value) <= std::numeric_limits<uint32_t>::max(), 
        "Requested size does not fit into uint32_t"); 
    static inline constexpr uint32_t value = Val * Unit::value;
};
