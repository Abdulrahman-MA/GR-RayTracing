#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include "grrt/geodesics/Integrator.hpp"
#include "grrt/math/State.hpp"

namespace {

struct HarmonicOscillator {
    double omega{1.0};

    [[nodiscard]] grrt::State2 evaluate_derivatives(const grrt::State2& state) const noexcept {
        return grrt::State2{{state[1], -omega * omega * state[0]}};
    }
};

}

TEST_CASE("Harmonic oscillator RK4 accuracy", "[ode]") {
    HarmonicOscillator system{1.0};
    grrt::State2 state{{1.0, 0.0}};
    const double dt = 0.01;
    const double t_final = 2.0 * 3.14159265358979323846;

    double t = 0.0;
    while (t < t_final) {
        state = grrt::Integrator<grrt::State2, HarmonicOscillator>::rk4_step(system, state, dt);
        t += dt;
    }

    REQUIRE_THAT(state[0], Catch::Matchers::WithinAbs(1.0, 1e-4));
    REQUIRE_THAT(state[1], Catch::Matchers::WithinAbs(0.0, 1e-4));
}

TEST_CASE("Harmonic oscillator adaptive Dormand-Prince", "[ode]") {
    HarmonicOscillator system{1.0};
    grrt::State2 state{{1.0, 0.0}};

    grrt::IntegratorConfig config;
    config.abs_tol = 1e-9;
    config.rel_tol = 1e-9;
    config.initial_step_size = 0.1;
    config.min_step_size = 1e-6;
    config.max_step_size = 0.5;

    grrt::Integrator<grrt::State2, HarmonicOscillator> integrator(config);

    const double t_final = 6.283185307179586;
    double t = 0.0;
    double dt = config.initial_step_size;

    while (t < t_final) {
        if (t + dt > t_final) {
            dt = t_final - t;
        }
        const auto step_res = integrator.adaptive_dormand_prince_step(system, state, dt);
        dt = step_res.next_dt;
        if (step_res.accepted) {
            state = step_res.next_state;
            t += step_res.actual_dt;
        }
    }

    REQUIRE_THAT(state[0], Catch::Matchers::WithinAbs(1.0, 1e-6));
    REQUIRE_THAT(state[1], Catch::Matchers::WithinAbs(0.0, 1e-6));
}
