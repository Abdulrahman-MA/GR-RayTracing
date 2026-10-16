#pragma once

#include <array>
#include <cstddef>
#include "grrt/math/Coordinates.hpp"

namespace grrt {

struct GeodesicState {
    std::array<double, 8> data{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    constexpr GeodesicState() noexcept = default;

    constexpr GeodesicState(double t, double r, double theta, double phi,
                            double pt, double pr, double ptheta, double pphi) noexcept
        : data{t, r, theta, phi, pt, pr, ptheta, pphi} {}

    constexpr GeodesicState(const Coordinates& pos, const std::array<double, 4>& mom) noexcept
        : data{pos.t(), pos.r(), pos.theta(), pos.phi(), mom[0], mom[1], mom[2], mom[3]} {}

    [[nodiscard]] constexpr double& t() noexcept { return data[0]; }
    [[nodiscard]] constexpr double t() const noexcept { return data[0]; }

    [[nodiscard]] constexpr double& r() noexcept { return data[1]; }
    [[nodiscard]] constexpr double r() const noexcept { return data[1]; }

    [[nodiscard]] constexpr double& theta() noexcept { return data[2]; }
    [[nodiscard]] constexpr double theta() const noexcept { return data[2]; }

    [[nodiscard]] constexpr double& phi() noexcept { return data[3]; }
    [[nodiscard]] constexpr double phi() const noexcept { return data[3]; }

    [[nodiscard]] constexpr double& p_t() noexcept { return data[4]; }
    [[nodiscard]] constexpr double p_t() const noexcept { return data[4]; }

    [[nodiscard]] constexpr double& p_r() noexcept { return data[5]; }
    [[nodiscard]] constexpr double p_r() const noexcept { return data[5]; }

    [[nodiscard]] constexpr double& p_theta() noexcept { return data[6]; }
    [[nodiscard]] constexpr double p_theta() const noexcept { return data[6]; }

    [[nodiscard]] constexpr double& p_phi() noexcept { return data[7]; }
    [[nodiscard]] constexpr double p_phi() const noexcept { return data[7]; }

    [[nodiscard]] constexpr double& operator[](std::size_t index) noexcept { return data[index]; }
    [[nodiscard]] constexpr double operator[](std::size_t index) const noexcept { return data[index]; }

    [[nodiscard]] constexpr Coordinates position() const noexcept {
        return Coordinates{data[0], data[1], data[2], data[3]};
    }

    [[nodiscard]] constexpr std::array<double, 4> momentum() const noexcept {
        return {data[4], data[5], data[6], data[7]};
    }

    constexpr GeodesicState& operator+=(const GeodesicState& rhs) noexcept {
        for (std::size_t i = 0; i < 8; ++i) {
            data[i] += rhs.data[i];
        }
        return *this;
    }

    constexpr GeodesicState& operator-=(const GeodesicState& rhs) noexcept {
        for (std::size_t i = 0; i < 8; ++i) {
            data[i] -= rhs.data[i];
        }
        return *this;
    }

    constexpr GeodesicState& operator*=(double scalar) noexcept {
        for (std::size_t i = 0; i < 8; ++i) {
            data[i] *= scalar;
        }
        return *this;
    }
};

[[nodiscard]] constexpr GeodesicState operator+(GeodesicState lhs, const GeodesicState& rhs) noexcept {
    lhs += rhs;
    return lhs;
}

[[nodiscard]] constexpr GeodesicState operator-(GeodesicState lhs, const GeodesicState& rhs) noexcept {
    lhs -= rhs;
    return lhs;
}

[[nodiscard]] constexpr GeodesicState operator*(GeodesicState lhs, double scalar) noexcept {
    lhs *= scalar;
    return lhs;
}

[[nodiscard]] constexpr GeodesicState operator*(double scalar, GeodesicState rhs) noexcept {
    rhs *= scalar;
    return rhs;
}

}
