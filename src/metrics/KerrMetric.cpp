#include "grrt/metrics/KerrMetric.hpp"
#include <algorithm>
#include <cmath>

namespace grrt {

double KerrMetric::horizon_radius() const {
    const double val = mass_ * mass_ - spin_ * spin_;
    return mass_ + std::sqrt(std::max(0.0, val));
}

MetricSample KerrMetric::evaluate(const Coordinates& x) const {
    MetricSample sample;
    const double r = x.r();
    const double theta = x.theta();
    const double a = spin_;
    const double m = mass_;

    const double sin_theta = std::sin(theta);
    const double cos_theta = std::cos(theta);
    const double sin2 = sin_theta * sin_theta;
    const double cos2 = cos_theta * cos_theta;
    const double a2 = a * a;
    const double r2 = r * r;

    const double sigma = r2 + a2 * cos2;
    const double delta = r2 - 2.0 * m * r + a2;
    const double inv_sigma = 1.0 / sigma;
    const double inv_delta = 1.0 / delta;

    sample.covariant(0, 0) = -(1.0 - (2.0 * m * r) * inv_sigma);
    sample.covariant(0, 3) = -(2.0 * m * a * r * sin2) * inv_sigma;
    sample.covariant(3, 0) = sample.covariant(0, 3);
    sample.covariant(1, 1) = sigma * inv_delta;
    sample.covariant(2, 2) = sigma;
    sample.covariant(3, 3) = (r2 + a2 + (2.0 * m * a2 * r * sin2) * inv_sigma) * sin2;

    const double inv_sigma_delta = inv_sigma * inv_delta;
    sample.inverse(0, 0) = -((r2 + a2) * (r2 + a2) - a2 * delta * sin2) * inv_sigma_delta;
    sample.inverse(0, 3) = -(2.0 * m * a * r) * inv_sigma_delta;
    sample.inverse(3, 0) = sample.inverse(0, 3);
    sample.inverse(1, 1) = delta * inv_sigma;
    sample.inverse(2, 2) = inv_sigma;
    sample.inverse(3, 3) = (delta - a2 * sin2) / (sigma * delta * sin2);

    const double d_sigma_dr = 2.0 * r;
    const double d_sigma_dtheta = -2.0 * a2 * sin_theta * cos_theta;
    const double d_delta_dr = 2.0 * r - 2.0 * m;

    const double d_inv_sigma_dr = -d_sigma_dr * (inv_sigma * inv_sigma);
    const double d_inv_sigma_dtheta = -d_sigma_dtheta * (inv_sigma * inv_sigma);

    Matrix4 dg_dr = Matrix4::zero();
    dg_dr(0, 0) = 2.0 * m * (inv_sigma + r * d_inv_sigma_dr);
    dg_dr(0, 3) = -2.0 * m * a * sin2 * (inv_sigma + r * d_inv_sigma_dr);
    dg_dr(3, 0) = dg_dr(0, 3);
    dg_dr(1, 1) = (d_sigma_dr * delta - sigma * d_delta_dr) * (inv_delta * inv_delta);
    dg_dr(2, 2) = d_sigma_dr;
    dg_dr(3, 3) = 2.0 * r * sin2 + 2.0 * m * a2 * sin2 * sin2 * (inv_sigma + r * d_inv_sigma_dr);

    Matrix4 dg_dtheta = Matrix4::zero();
    dg_dtheta(0, 0) = 2.0 * m * r * d_inv_sigma_dtheta;
    dg_dtheta(0, 3) = -2.0 * m * a * r * (2.0 * sin_theta * cos_theta * inv_sigma + sin2 * d_inv_sigma_dtheta);
    dg_dtheta(3, 0) = dg_dtheta(0, 3);
    dg_dtheta(1, 1) = d_sigma_dtheta * inv_delta;
    dg_dtheta(2, 2) = d_sigma_dtheta;
    dg_dtheta(3, 3) = 2.0 * (r2 + a2) * sin_theta * cos_theta +
                      2.0 * m * a2 * r * (4.0 * sin2 * sin_theta * cos_theta * inv_sigma + sin2 * sin2 * d_inv_sigma_dtheta);

    sample.derivatives.d_cov[1] = dg_dr;
    sample.derivatives.d_cov[2] = dg_dtheta;

    const Matrix4 inv = sample.inverse;
    const Matrix4 neg_inv = Matrix4::zero() - inv;

    auto compute_d_inv = [&](const Matrix4& d_cov_mat) {
        Matrix4 temp = d_cov_mat.multiply(inv);
        Matrix4 res = neg_inv.multiply(temp);
        return res;
    };

    sample.derivatives.d_inv[1] = compute_d_inv(dg_dr);
    sample.derivatives.d_inv[2] = compute_d_inv(dg_dtheta);

    return sample;
}

}
