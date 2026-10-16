#include "grrt/camera/ObserverFrame.hpp"
#include <cmath>

namespace grrt {

ObserverFrame::ObserverFrame(const Metric& metric, const Coordinates& observer_pos)
    : position_(observer_pos), sample_(metric.evaluate(observer_pos)) {
    const double g00 = sample_.covariant(0, 0);
    const double g03 = sample_.covariant(0, 3);
    const double g11 = sample_.covariant(1, 1);
    const double g22 = sample_.covariant(2, 2);
    const double g33 = sample_.covariant(3, 3);

    const double omega = (std::abs(g33) > 1e-12) ? (-g03 / g33) : 0.0;
    const double alpha2 = -g00 + omega * omega * g33;
    const double alpha = std::sqrt(std::max(1e-12, alpha2));

    tetrad_[0] = Coordinates(1.0 / alpha, 0.0, 0.0, omega / alpha);
    tetrad_[1] = Coordinates(0.0, 1.0 / std::sqrt(std::max(1e-12, g11)), 0.0, 0.0);
    tetrad_[2] = Coordinates(0.0, 0.0, 1.0 / std::sqrt(std::max(1e-12, g22)), 0.0);
    tetrad_[3] = Coordinates(0.0, 0.0, 0.0, 1.0 / std::sqrt(std::max(1e-12, g33)));
}

std::array<double, 4> ObserverFrame::local_to_coordinate_momentum(
    double n1, double n2, double n3) const {
    const double p0 = -1.0;
    std::array<double, 4> p_up{0.0, 0.0, 0.0, 0.0};

    for (std::size_t mu = 0; mu < 4; ++mu) {
        p_up[mu] = p0 * tetrad_[0][mu] +
                   n1 * tetrad_[1][mu] +
                   n2 * tetrad_[2][mu] +
                   n3 * tetrad_[3][mu];
    }
    return p_up;
}

std::array<double, 4> ObserverFrame::lower_momentum(
    const std::array<double, 4>& contravariant_p) const {
    return sample_.covariant.multiply(contravariant_p);
}

}
