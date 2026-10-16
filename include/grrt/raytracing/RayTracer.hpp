#pragma once

#include <cstddef>
#include "grrt/geodesics/GeodesicState.hpp"
#include "grrt/geodesics/Integrator.hpp"
#include "grrt/metrics/Metric.hpp"
#include "grrt/objects/AccretionDisk.hpp"
#include "grrt/raytracing/RayOutcome.hpp"

namespace grrt {

struct RayTracerConfig {
    double escape_radius{100.0};
    double capture_margin{0.02};
    IntegratorConfig integrator_config{};
    AccretionDisk disk{};
};

struct RayResult {
    RayOutcome outcome{RayOutcome::NumericalFailure};
    std::size_t steps{0};
    GeodesicState final_state{};
    DiskIntersection disk_intersection{};
    double initial_hamiltonian{0.0};
    double max_hamiltonian_drift{0.0};
};

class RayTracer {
public:
    explicit RayTracer(RayTracerConfig config = {}) noexcept : config_(config) {}

    [[nodiscard]] RayResult trace_ray(const GeodesicState& initial_state, const Metric& metric) const;
    [[nodiscard]] const RayTracerConfig& config() const noexcept { return config_; }
    void set_config(const RayTracerConfig& config) noexcept { config_ = config; }

private:
    RayTracerConfig config_;
};

}
