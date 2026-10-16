#pragma once

#include <array>
#include <cmath>
#include <cstddef>

namespace grrt {

struct Coordinates {
    std::array<double, 4> data{0.0, 0.0, 0.0, 0.0};

    constexpr Coordinates() noexcept = default;

    constexpr Coordinates(double t_val, double r_val, double theta_val, double phi_val) noexcept
        : data{t_val, r_val, theta_val, phi_val} {}

    [[nodiscard]] constexpr double& t() noexcept { return data[0]; }
    [[nodiscard]] constexpr double t() const noexcept { return data[0]; }

    [[nodiscard]] constexpr double& r() noexcept { return data[1]; }
    [[nodiscard]] constexpr double r() const noexcept { return data[1]; }

    [[nodiscard]] constexpr double& theta() noexcept { return data[2]; }
    [[nodiscard]] constexpr double theta() const noexcept { return data[2]; }

    [[nodiscard]] constexpr double& phi() noexcept { return data[3]; }
    [[nodiscard]] constexpr double phi() const noexcept { return data[3]; }

    [[nodiscard]] constexpr double& operator[](std::size_t index) noexcept { return data[index]; }
    [[nodiscard]] constexpr double operator[](std::size_t index) const noexcept { return data[index]; }

    constexpr Coordinates& operator+=(const Coordinates& rhs) noexcept {
        for (std::size_t i = 0; i < 4; ++i) {
            data[i] += rhs.data[i];
        }
        return *this;
    }

    constexpr Coordinates& operator-=(const Coordinates& rhs) noexcept {
        for (std::size_t i = 0; i < 4; ++i) {
            data[i] -= rhs.data[i];
        }
        return *this;
    }

    constexpr Coordinates& operator*=(double scalar) noexcept {
        for (std::size_t i = 0; i < 4; ++i) {
            data[i] *= scalar;
        }
        return *this;
    }
};

[[nodiscard]] constexpr Coordinates operator+(Coordinates lhs, const Coordinates& rhs) noexcept {
    lhs += rhs;
    return lhs;
}

[[nodiscard]] constexpr Coordinates operator-(Coordinates lhs, const Coordinates& rhs) noexcept {
    lhs -= rhs;
    return lhs;
}

[[nodiscard]] constexpr Coordinates operator*(Coordinates lhs, double scalar) noexcept {
    lhs *= scalar;
    return lhs;
}

[[nodiscard]] constexpr Coordinates operator*(double scalar, Coordinates rhs) noexcept {
    rhs *= scalar;
    return rhs;
}

}
