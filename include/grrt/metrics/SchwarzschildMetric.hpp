#pragma once

#include "grrt/metrics/Metric.hpp"

namespace grrt {

class SchwarzschildMetric final : public Metric {
public:
    explicit constexpr SchwarzschildMetric(double mass = 1.0) noexcept : mass_(mass) {}

    [[nodiscard]] MetricSample evaluate(const Coordinates& x) const override;
    [[nodiscard]] double horizon_radius() const override;
    [[nodiscard]] double mass() const noexcept override { return mass_; }
    [[nodiscard]] double spin() const noexcept override { return 0.0; }

private:
    double mass_{1.0};
};

}
