#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include "grrt/geodesics/Integrator.hpp"
#include "grrt/math/State.hpp"

namespace {

struct Oscillator {
    [[nodiscard]] grrt::State2 evaluate_derivatives(const grrt::State2& s) const noexcept {
        return grrt::State2{{s[1], -s[0]}};
    }
};

}

TEST_CASE("RK4 fourth-order convergence rate", "[convergence]") {
    Oscillator sys;
    const double t_final = 1.0;

    auto compute_error = [&](double dt) {
        grrt::State2 state{{1.0, 0.0}};
        double t = 0.0;
        while (t < t_final - 1e-12) {
            state = grrt::Integrator<grrt::State2, Oscillator>::rk4_step(sys, state, dt);
            t += dt;
        }
        const double x_exact = std::cos(t_final);
        return std::abs(state[0] - x_exact);
    };

    const double err1 = compute_error(0.1);
    const double err2 = compute_error(0.05);

    const double ratio = err1 / err2;
    REQUIRE(ratio > 14.0);
    REQUIRE(ratio < 17.5);
}
