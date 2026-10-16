#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include "grrt/geodesics/HamiltonianSystem.hpp"
#include "grrt/metrics/SchwarzschildMetric.hpp"

TEST_CASE("Schwarzschild photon sphere radial equilibrium", "[geodesics]") {
    const grrt::SchwarzschildMetric metric(1.0);
    const grrt::HamiltonianSystem system(metric, false);

    const double r_sphere = 3.0;
    const double theta = 1.5707963267948966;
    const double pt = -1.0;
    const double pphi = 3.0 * std::sqrt(3.0);

    const grrt::GeodesicState state(0.0, r_sphere, theta, 0.0, pt, 0.0, 0.0, pphi);

    const double h = system.evaluate_hamiltonian(state);
    REQUIRE_THAT(h, Catch::Matchers::WithinAbs(0.0, 1e-12));

    const grrt::GeodesicState deriv = system.evaluate_derivatives(state);

    REQUIRE_THAT(deriv.r(), Catch::Matchers::WithinAbs(0.0, 1e-12));
    REQUIRE_THAT(deriv.p_r(), Catch::Matchers::WithinAbs(0.0, 1e-12));
    REQUIRE_THAT(deriv.theta(), Catch::Matchers::WithinAbs(0.0, 1e-12));
    REQUIRE_THAT(deriv.p_theta(), Catch::Matchers::WithinAbs(0.0, 1e-12));
}
