#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include "grrt/geodesics/GeodesicState.hpp"

namespace grrt {

struct DiskIntersection {
    bool hit{false};
    double r{0.0};
    double phi{0.0};
    double frequency_shift{1.0};
    double intensity{0.0};
};

class AccretionDisk {
public:
    explicit AccretionDisk(double r_in = 6.0, double r_out = 20.0, bool enabled = false) noexcept
        : r_in_(r_in), r_out_(r_out), enabled_(enabled) {}

    [[nodiscard]] bool is_enabled() const noexcept { return enabled_; }
    void set_enabled(bool enabled) noexcept { enabled_ = enabled; }

    [[nodiscard]] double r_in() const noexcept { return r_in_; }
    [[nodiscard]] double r_out() const noexcept { return r_out_; }

    [[nodiscard]] DiskIntersection check_crossing(
        const GeodesicState& prev_state, const GeodesicState& curr_state,
        double mass = 1.0, double spin = 0.0) const {
        DiskIntersection result;
        if (!enabled_) {
            return result;
        }

        constexpr double half_pi = 1.5707963267948966;
        const double th1 = prev_state.theta() - half_pi;
        const double th2 = curr_state.theta() - half_pi;

        if (th1 * th2 > 0.0) {
            return result;
        }

        const double denom = th1 - th2;
        const double frac = (std::abs(denom) > 1e-14) ? (th1 / denom) : 0.5;
        const double r_cross = prev_state.r() + frac * (curr_state.r() - prev_state.r());
        const double phi_cross = prev_state.phi() + frac * (curr_state.phi() - prev_state.phi());

        if (r_cross < r_in_ || r_cross > r_out_) {
            return result;
        }

        result.hit = true;
        result.r = r_cross;
        result.phi = phi_cross;

        const double r32 = std::pow(r_cross, 1.5);
        const double omega_k = std::sqrt(mass) / (r32 + spin * std::sqrt(mass));

        const double pt = curr_state.p_t();
        const double pphi = curr_state.p_phi();

        const double u_t = -1.0;
        const double u_phi = omega_k;
        const double p_dot_u = pt * u_t + pphi * u_phi;

        const double g = std::abs(p_dot_u) > 1e-12 ? (1.0 / std::abs(p_dot_u)) : 1.0;
        result.frequency_shift = g;

        const double base_intensity = std::pow(r_in_ / r_cross, 3.0) * (1.0 - std::sqrt(r_in_ / r_cross));
        result.intensity = std::max(0.0, base_intensity) * std::pow(g, 3.0);

        return result;
    }

private:
    double r_in_{6.0};
    double r_out_{20.0};
    bool enabled_{false};
};

}
