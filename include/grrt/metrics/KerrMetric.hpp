#pragma once

#include "grrt/metrics/Metric.hpp"

namespace grrt {

class KerrMetric final : public Metric {
public:
    explicit constexpr KerrMetric(double mass = 1.0, double spin = 0.0) noexcept
        : mass_(mass), spin_(spin) {}

    [[nodiscard]] MetricSample evaluate(const Coordinates& x) const override;
    [[nodiscard]] double horizon_radius() const override;
    [[nodiscard]] double mass() const noexcept override { return mass_; }
    [[nodiscard]] double spin() const noexcept override { return spin_; }

private:
    double mass_{1.0};
    double spin_{0.0};
};

}
