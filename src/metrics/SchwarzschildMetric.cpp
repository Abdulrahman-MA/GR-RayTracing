#include "grrt/metrics/SchwarzschildMetric.hpp"
#include <cmath>

namespace grrt {

double SchwarzschildMetric::horizon_radius() const {
    return 2.0 * mass_;
}

MetricSample SchwarzschildMetric::evaluate(const Coordinates& x) const {
    MetricSample sample;
    const double r = x.r();
    const double theta = x.theta();
    const double sin_theta = std::sin(theta);
    const double cos_theta = std::cos(theta);
    const double sin2_theta = sin_theta * sin_theta;

    const double f = 1.0 - (2.0 * mass_) / r;
    const double inv_f = 1.0 / f;

    sample.covariant(0, 0) = -f;
    sample.covariant(1, 1) = inv_f;
    sample.covariant(2, 2) = r * r;
    sample.covariant(3, 3) = r * r * sin2_theta;

    sample.inverse(0, 0) = -inv_f;
    sample.inverse(1, 1) = f;
    sample.inverse(2, 2) = 1.0 / (r * r);
    sample.inverse(3, 3) = 1.0 / (r * r * sin2_theta);

    const double df_dr = (2.0 * mass_) / (r * r);
    const double d_inv_f_dr = -df_dr * (inv_f * inv_f);

    sample.derivatives.d_cov[1](0, 0) = -df_dr;
    sample.derivatives.d_cov[1](1, 1) = d_inv_f_dr;
    sample.derivatives.d_cov[1](2, 2) = 2.0 * r;
    sample.derivatives.d_cov[1](3, 3) = 2.0 * r * sin2_theta;

    sample.derivatives.d_cov[2](3, 3) = 2.0 * r * r * sin_theta * cos_theta;

    sample.derivatives.d_inv[1](0, 0) = -d_inv_f_dr;
    sample.derivatives.d_inv[1](1, 1) = df_dr;
    sample.derivatives.d_inv[1](2, 2) = -2.0 / (r * r * r);
    sample.derivatives.d_inv[1](3, 3) = -2.0 / (r * r * r * sin2_theta);

    sample.derivatives.d_inv[2](3, 3) = -2.0 * cos_theta / (r * r * sin2_theta * sin_theta);

    return sample;
}

}
