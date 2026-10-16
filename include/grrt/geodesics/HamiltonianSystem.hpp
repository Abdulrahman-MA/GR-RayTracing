#pragma once

#include "grrt/geodesics/GeodesicState.hpp"
#include "grrt/metrics/Metric.hpp"

namespace grrt {

class HamiltonianSystem {
public:
    explicit HamiltonianSystem(const Metric& metric, bool backward = true) noexcept
        : metric_(&metric), backward_(backward) {}

    [[nodiscard]] GeodesicState evaluate_derivatives(const GeodesicState& state) const;
    [[nodiscard]] double evaluate_hamiltonian(const GeodesicState& state) const;
    [[nodiscard]] const Metric& metric() const noexcept { return *metric_; }
    [[nodiscard]] bool is_backward() const noexcept { return backward_; }
    void set_backward(bool backward) noexcept { backward_ = backward; }

private:
    const Metric* metric_;
    bool backward_{true};
};

}
