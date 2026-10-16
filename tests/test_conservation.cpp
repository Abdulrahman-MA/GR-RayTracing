#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include "grrt/geodesics/HamiltonianSystem.hpp"
#include "grrt/geodesics/Integrator.hpp"
#include "grrt/metrics/KerrMetric.hpp"

TEST_CASE("Conservation of pt, pphi, and null Hamiltonian in Kerr spacetime", "[conservation]") {
    const grrt::KerrMetric metric(1.0, 0.7);
    const grrt::HamiltonianSystem system(metric, true);

    const grrt::Coordinates pos(0.0, 25.0, 1.2, 0.5);
    const auto sample = metric.evaluate(pos);

    const double pt = -1.0;
    const double pphi = 2.5;
    const double ptheta = 1.0;

    const double g_rr_inv = sample.inverse(1, 1);
    const double g_tt_inv = sample.inverse(0, 0);
    const double g_tphi_inv = sample.inverse(0, 3);
    const double g_thth_inv = sample.inverse(2, 2);
    const double g_phiphi_inv = sample.inverse(3, 3);

    const double h_rest = 0.5 * (g_tt_inv * pt * pt +
                                 2.0 * g_tphi_inv * pt * pphi +
                                 g_thth_inv * ptheta * ptheta +
                                 g_phiphi_inv * pphi * pphi);

    const double pr_squared = -2.0 * h_rest / g_rr_inv;
    REQUIRE(pr_squared > 0.0);
    const double pr = -std::sqrt(pr_squared);

    grrt::GeodesicState state(pos, {pt, pr, ptheta, pphi});

    const double h_initial = system.evaluate_hamiltonian(state);
    REQUIRE_THAT(h_initial, Catch::Matchers::WithinAbs(0.0, 1e-12));

    grrt::IntegratorConfig config;
    config.abs_tol = 1e-8;
    config.rel_tol = 1e-8;
    config.initial_step_size = 0.02;
    config.min_step_size = 1e-6;
    config.max_step_size = 0.1;

    grrt::Integrator<grrt::GeodesicState, grrt::HamiltonianSystem> integrator(config);

    double dt = config.initial_step_size;
    for (int step = 0; step < 100; ++step) {
        const auto step_res = integrator.adaptive_dormand_prince_step(system, state, dt);
        dt = step_res.next_dt;
        if (step_res.accepted) {
            state = step_res.next_state;
        }
    }

    REQUIRE_THAT(state.p_t(), Catch::Matchers::WithinAbs(pt, 1e-7));
    REQUIRE_THAT(state.p_phi(), Catch::Matchers::WithinAbs(pphi, 1e-7));

    const double h_final = system.evaluate_hamiltonian(state);
    REQUIRE_THAT(h_final, Catch::Matchers::WithinAbs(0.0, 1e-6));
}
