#pragma once

#include <array>
#include <cstddef>

namespace grrt {

template <std::size_t N>
struct State {
    std::array<double, N> data{};

    constexpr State() noexcept = default;

    constexpr State(std::array<double, N> values) noexcept : data(values) {}

    [[nodiscard]] constexpr double& operator[](std::size_t index) noexcept { return data[index]; }
    [[nodiscard]] constexpr double operator[](std::size_t index) const noexcept { return data[index]; }

    constexpr State& operator+=(const State& rhs) noexcept {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] += rhs.data[i];
        }
        return *this;
    }

    constexpr State& operator-=(const State& rhs) noexcept {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] -= rhs.data[i];
        }
        return *this;
    }

    constexpr State& operator*=(double scalar) noexcept {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] *= scalar;
        }
        return *this;
    }
};

template <std::size_t N>
[[nodiscard]] constexpr State<N> operator+(State<N> lhs, const State<N>& rhs) noexcept {
    lhs += rhs;
    return lhs;
}

template <std::size_t N>
[[nodiscard]] constexpr State<N> operator-(State<N> lhs, const State<N>& rhs) noexcept {
    lhs -= rhs;
    return lhs;
}

template <std::size_t N>
[[nodiscard]] constexpr State<N> operator*(State<N> lhs, double scalar) noexcept {
    lhs *= scalar;
    return lhs;
}

template <std::size_t N>
[[nodiscard]] constexpr State<N> operator*(double scalar, State<N> rhs) noexcept {
    rhs *= scalar;
    return rhs;
}

using State2 = State<2>;
using State4 = State<4>;
using State8 = State<8>;

}
