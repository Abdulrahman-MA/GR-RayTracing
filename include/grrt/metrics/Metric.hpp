#pragma once

#include <array>
#include "grrt/math/Coordinates.hpp"
#include "grrt/math/Matrix4.hpp"

namespace grrt {

struct MetricDerivatives {
    std::array<Matrix4, 4> d_cov{};
    std::array<Matrix4, 4> d_inv{};
};

struct MetricSample {
    Matrix4 covariant;
    Matrix4 inverse;
    MetricDerivatives derivatives;
};

class Metric {
public:
    virtual ~Metric() = default;

    [[nodiscard]] virtual MetricSample evaluate(const Coordinates& x) const = 0;
    [[nodiscard]] virtual double horizon_radius() const = 0;
    [[nodiscard]] virtual double mass() const noexcept = 0;
    [[nodiscard]] virtual double spin() const noexcept = 0;
};

}
