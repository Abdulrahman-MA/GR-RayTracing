#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include "grrt/metrics/KerrMetric.hpp"
#include "grrt/metrics/SchwarzschildMetric.hpp"

TEST_CASE("Schwarzschild metric inverse and symmetry", "[metrics]") {
    grrt::SchwarzschildMetric metric(1.0);
    const grrt::Coordinates coords(0.0, 10.0, 1.5707963267948966, 0.5);
    const grrt::MetricSample sample = metric.evaluate(coords);

    for (std::size_t mu = 0; mu < 4; ++mu) {
        for (std::size_t nu = 0; nu < 4; ++nu) {
            REQUIRE(sample.covariant(mu, nu) == sample.covariant(nu, mu));
            REQUIRE(sample.inverse(mu, nu) == sample.inverse(nu, mu));
        }
    }

    const grrt::Matrix4 identity = sample.covariant * sample.inverse;
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            const double expected = (i == j) ? 1.0 : 0.0;
            REQUIRE_THAT(identity(i, j), Catch::Matchers::WithinAbs(expected, 1e-12));
        }
    }
}

TEST_CASE("Kerr metric reduces to Schwarzschild at zero spin", "[metrics]") {
    grrt::SchwarzschildMetric schw(1.0);
    grrt::KerrMetric kerr(1.0, 0.0);

    const grrt::Coordinates coords(0.0, 8.0, 1.2, 0.3);
    const grrt::MetricSample sample_schw = schw.evaluate(coords);
    const grrt::MetricSample sample_kerr = kerr.evaluate(coords);

    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            REQUIRE_THAT(sample_kerr.covariant(i, j), Catch::Matchers::WithinAbs(sample_schw.covariant(i, j), 1e-12));
            REQUIRE_THAT(sample_kerr.inverse(i, j), Catch::Matchers::WithinAbs(sample_schw.inverse(i, j), 1e-12));
        }
    }
}

TEST_CASE("Kerr metric inverse and symmetry with non-zero spin", "[metrics]") {
    grrt::KerrMetric kerr(1.0, 0.9);
    const grrt::Coordinates coords(0.0, 6.0, 0.8, 1.1);
    const grrt::MetricSample sample = kerr.evaluate(coords);

    for (std::size_t mu = 0; mu < 4; ++mu) {
        for (std::size_t nu = 0; nu < 4; ++nu) {
            REQUIRE_THAT(sample.covariant(mu, nu), Catch::Matchers::WithinAbs(sample.covariant(nu, mu), 1e-14));
            REQUIRE_THAT(sample.inverse(mu, nu), Catch::Matchers::WithinAbs(sample.inverse(nu, mu), 1e-14));
        }
    }

    const grrt::Matrix4 identity = sample.covariant * sample.inverse;
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            const double expected = (i == j) ? 1.0 : 0.0;
            REQUIRE_THAT(identity(i, j), Catch::Matchers::WithinAbs(expected, 1e-11));
        }
    }
}
