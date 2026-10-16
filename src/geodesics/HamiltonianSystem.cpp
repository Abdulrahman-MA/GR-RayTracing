#include "grrt/geodesics/HamiltonianSystem.hpp"

namespace grrt {

GeodesicState HamiltonianSystem::evaluate_derivatives(const GeodesicState& state) const {
    const Coordinates x = state.position();
    const std::array<double, 4> p = state.momentum();
    const MetricSample sample = metric_->evaluate(x);

    GeodesicState deriv;
    const double sign = backward_ ? -1.0 : 1.0;

    for (std::size_t mu = 0; mu < 4; ++mu) {
        double dx_dlambda = 0.0;
        for (std::size_t nu = 0; nu < 4; ++nu) {
            dx_dlambda += sample.inverse(mu, nu) * p[nu];
        }
        deriv[mu] = sign * dx_dlambda;
    }

    for (std::size_t mu = 0; mu < 4; ++mu) {
        double dp_dlambda = 0.0;
        const Matrix4& d_inv_mu = sample.derivatives.d_inv[mu];
        for (std::size_t alpha = 0; alpha < 4; ++alpha) {
            for (std::size_t beta = 0; beta < 4; ++beta) {
                dp_dlambda -= 0.5 * d_inv_mu(alpha, beta) * p[alpha] * p[beta];
            }
        }
        deriv[4 + mu] = sign * dp_dlambda;
    }

    return deriv;
}

double HamiltonianSystem::evaluate_hamiltonian(const GeodesicState& state) const {
    const Coordinates x = state.position();
    const std::array<double, 4> p = state.momentum();
    const MetricSample sample = metric_->evaluate(x);

    double h = 0.0;
    for (std::size_t mu = 0; mu < 4; ++mu) {
        for (std::size_t nu = 0; nu < 4; ++nu) {
            h += 0.5 * sample.inverse(mu, nu) * p[mu] * p[nu];
        }
    }
    return h;
}

}
