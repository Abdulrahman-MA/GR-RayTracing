#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include "grrt/math/Coordinates.hpp"

namespace grrt {

struct Matrix4 {
    std::array<double, 16> data{0.0};

    constexpr Matrix4() noexcept = default;

    [[nodiscard]] static constexpr Matrix4 identity() noexcept {
        Matrix4 m;
        m(0, 0) = 1.0;
        m(1, 1) = 1.0;
        m(2, 2) = 1.0;
        m(3, 3) = 1.0;
        return m;
    }

    [[nodiscard]] static constexpr Matrix4 zero() noexcept {
        return Matrix4{};
    }

    [[nodiscard]] constexpr double& operator()(std::size_t row, std::size_t col) noexcept {
        return data[row * 4 + col];
    }

    [[nodiscard]] constexpr double operator()(std::size_t row, std::size_t col) const noexcept {
        return data[row * 4 + col];
    }

    [[nodiscard]] constexpr Matrix4 transpose() const noexcept {
        Matrix4 res;
        for (std::size_t r = 0; r < 4; ++r) {
            for (std::size_t c = 0; c < 4; ++c) {
                res(c, r) = (*this)(r, c);
            }
        }
        return res;
    }

    [[nodiscard]] constexpr std::array<double, 4> multiply(const std::array<double, 4>& vec) const noexcept {
        std::array<double, 4> res{0.0, 0.0, 0.0, 0.0};
        for (std::size_t r = 0; r < 4; ++r) {
            for (std::size_t c = 0; c < 4; ++c) {
                res[r] += (*this)(r, c) * vec[c];
            }
        }
        return res;
    }

    [[nodiscard]] constexpr Coordinates multiply(const Coordinates& coords) const noexcept {
        Coordinates res{0.0, 0.0, 0.0, 0.0};
        for (std::size_t r = 0; r < 4; ++r) {
            for (std::size_t c = 0; c < 4; ++c) {
                res[r] += (*this)(r, c) * coords[c];
            }
        }
        return res;
    }

    [[nodiscard]] constexpr Matrix4 multiply(const Matrix4& rhs) const noexcept {
        Matrix4 res;
        for (std::size_t i = 0; i < 4; ++i) {
            for (std::size_t k = 0; k < 4; ++k) {
                const double aik = (*this)(i, k);
                for (std::size_t j = 0; j < 4; ++j) {
                    res(i, j) += aik * rhs(k, j);
                }
            }
        }
        return res;
    }

    [[nodiscard]] Matrix4 inverse() const noexcept {
        Matrix4 inv;
        std::array<std::array<double, 8>, 4> aug{};

        for (std::size_t i = 0; i < 4; ++i) {
            for (std::size_t j = 0; j < 4; ++j) {
                aug[i][j] = (*this)(i, j);
            }
            aug[i][4 + i] = 1.0;
        }

        for (std::size_t i = 0; i < 4; ++i) {
            std::size_t max_row = i;
            double max_val = std::abs(aug[i][i]);
            for (std::size_t k = i + 1; k < 4; ++k) {
                const double val = std::abs(aug[k][i]);
                if (val > max_val) {
                    max_val = val;
                    max_row = k;
                }
            }

            if (max_val < 1e-18) {
                return Matrix4::zero();
            }

            if (max_row != i) {
                std::swap(aug[i], aug[max_row]);
            }

            const double pivot = aug[i][i];
            const double inv_pivot = 1.0 / pivot;
            for (std::size_t j = 0; j < 8; ++j) {
                aug[i][j] *= inv_pivot;
            }

            for (std::size_t k = 0; k < 4; ++k) {
                if (k != i) {
                    const double factor = aug[k][i];
                    for (std::size_t j = 0; j < 8; ++j) {
                        aug[k][j] -= factor * aug[i][j];
                    }
                }
            }
        }

        for (std::size_t i = 0; i < 4; ++i) {
            for (std::size_t j = 0; j < 4; ++j) {
                inv(i, j) = aug[i][4 + j];
            }
        }

        return inv;
    }
};

[[nodiscard]] constexpr Matrix4 operator+(Matrix4 lhs, const Matrix4& rhs) noexcept {
    for (std::size_t i = 0; i < 16; ++i) {
        lhs.data[i] += rhs.data[i];
    }
    return lhs;
}

[[nodiscard]] constexpr Matrix4 operator-(Matrix4 lhs, const Matrix4& rhs) noexcept {
    for (std::size_t i = 0; i < 16; ++i) {
        lhs.data[i] -= rhs.data[i];
    }
    return lhs;
}

[[nodiscard]] inline Matrix4 operator*(const Matrix4& lhs, const Matrix4& rhs) noexcept {
    return lhs.multiply(rhs);
}

[[nodiscard]] inline Coordinates operator*(const Matrix4& lhs, const Coordinates& rhs) noexcept {
    return lhs.multiply(rhs);
}

}
