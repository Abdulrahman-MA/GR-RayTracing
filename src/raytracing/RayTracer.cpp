#include "grrt/raytracing/RayTracer.hpp"
#include <cmath>
#include "grrt/geodesics/HamiltonianSystem.hpp"

namespace grrt {

RayResult RayTracer::trace_ray(const GeodesicState& initial_state, const Metric& metric) const {
    RayResult result;
    const double r_horizon = metric.horizon_radius();
    const double capture_radius = r_horizon + config_.capture_margin;

    const HamiltonianSystem system(metric, true);
    Integrator<GeodesicState, HamiltonianSystem> integrator(config_.integrator_config);

    GeodesicState current_state = initial_state;
    result.initial_hamiltonian = system.evaluate_hamiltonian(current_state);
    double max_drift = 0.0;
    double dt = config_.integrator_config.initial_step_size;

    for (std::size_t step = 0; step < config_.integrator_config.max_steps; ++step) {
        result.steps = step + 1;

        for (std::size_t i = 0; i < 8; ++i) {
            if (std::isnan(current_state[i]) || std::isinf(current_state[i])) {
                result.outcome = RayOutcome::NumericalFailure;
                result.final_state = current_state;
                result.max_hamiltonian_drift = max_drift;
                return result;
            }
        }

        const double r = current_state.r();
        if (r <= capture_radius) {
            result.outcome = RayOutcome::Captured;
            result.final_state = current_state;
            result.max_hamiltonian_drift = max_drift;
            return result;
        }

        if (r >= config_.escape_radius) {
            result.outcome = RayOutcome::Escaped;
            result.final_state = current_state;
            result.max_hamiltonian_drift = max_drift;
            return result;
        }

        const auto step_res = integrator.adaptive_dormand_prince_step(system, current_state, dt);
        dt = step_res.next_dt;

        if (!step_res.accepted) {
            continue;
        }

        if (config_.disk.is_enabled()) {
            const auto disk_hit = config_.disk.check_crossing(
                current_state, step_res.next_state, metric.mass(), metric.spin());
            if (disk_hit.hit) {
                result.outcome = RayOutcome::DiskIntersection;
                result.disk_intersection = disk_hit;
                result.final_state = step_res.next_state;
                result.max_hamiltonian_drift = max_drift;
                return result;
            }
        }

        current_state = step_res.next_state;
        const double h = system.evaluate_hamiltonian(current_state);
        const double drift = std::abs(h - result.initial_hamiltonian);
        if (drift > max_drift) {
            max_drift = drift;
        }
    }

    result.outcome = RayOutcome::MaximumSteps;
    result.final_state = current_state;
    result.max_hamiltonian_drift = max_drift;
    return result;
}

}
