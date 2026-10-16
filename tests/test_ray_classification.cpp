#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include "grrt/metrics/SchwarzschildMetric.hpp"
#include "grrt/raytracing/RayTracer.hpp"

TEST_CASE("Schwarzschild critical impact parameter capture versus escape", "[ray_classification]") {
    const grrt::SchwarzschildMetric metric(1.0);

    grrt::RayTracerConfig config;
    config.escape_radius = 60.0;
    config.capture_margin = 0.05;
    config.integrator_config.abs_tol = 1e-8;
    config.integrator_config.rel_tol = 1e-8;
    config.integrator_config.initial_step_size = 0.02;
    config.integrator_config.min_step_size = 1e-5;
    config.integrator_config.max_step_size = 0.2;
    config.integrator_config.max_steps = 15000;

    const grrt::RayTracer tracer(config);

    const double r_init = 40.0;
    const double theta = 1.5707963267948966;
    const double pt = -1.0;

    auto make_ray = [&](double b) {
        const double pphi = b;
        const double f = 1.0 - 2.0 / r_init;
        const double pr_squared = (pt * pt / f - pphi * pphi / (r_init * r_init)) / f;
        const double pr = -std::sqrt(std::max(0.0, pr_squared));
        return grrt::GeodesicState(0.0, r_init, theta, 0.0, pt, pr, 0.0, pphi);
    };

    const auto subcritical_ray = make_ray(4.5);
    const auto subcritical_res = tracer.trace_ray(subcritical_ray, metric);
    REQUIRE(subcritical_res.outcome == grrt::RayOutcome::Captured);

    const auto supercritical_ray = make_ray(6.0);
    const auto supercritical_res = tracer.trace_ray(supercritical_ray, metric);
    REQUIRE(supercritical_res.outcome == grrt::RayOutcome::Escaped);
}
