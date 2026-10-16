#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace grrt {

struct IntegratorConfig {
    double abs_tol{1e-8};
    double rel_tol{1e-8};
    double initial_step_size{0.01};
    double min_step_size{1e-6};
    double max_step_size{1.0};
    std::size_t max_steps{20000};
};

template <typename StateType, typename SystemType>
class Integrator {
public:
    explicit Integrator(IntegratorConfig config = {}) noexcept : config_(config) {}

    [[nodiscard]] static StateType rk4_step(const SystemType& system, const StateType& y, double dt) {
        const StateType k1 = system.evaluate_derivatives(y);
        const StateType k2 = system.evaluate_derivatives(y + k1 * (0.5 * dt));
        const StateType k3 = system.evaluate_derivatives(y + k2 * (0.5 * dt));
        const StateType k4 = system.evaluate_derivatives(y + k3 * dt);
        return y + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0);
    }

    struct StepResult {
        StateType next_state;
        double actual_dt;
        double next_dt;
        double error_ratio;
        bool accepted;
    };

    [[nodiscard]] StepResult adaptive_dormand_prince_step(
        const SystemType& system, const StateType& y, double dt) const {
        constexpr double c2 = 1.0 / 5.0;
        constexpr double c3 = 3.0 / 10.0;
        constexpr double c4 = 4.0 / 5.0;
        constexpr double c5 = 8.0 / 9.0;

        constexpr double a21 = 1.0 / 5.0;

        constexpr double a31 = 3.0 / 40.0;
        constexpr double a32 = 9.0 / 40.0;

        constexpr double a41 = 44.0 / 45.0;
        constexpr double a42 = -56.0 / 15.0;
        constexpr double a43 = 32.0 / 9.0;

        constexpr double a51 = 19372.0 / 6561.0;
        constexpr double a52 = -25360.0 / 2187.0;
        constexpr double a53 = 64448.0 / 6561.0;
        constexpr double a54 = -212.0 / 729.0;

        constexpr double a61 = 9017.0 / 3168.0;
        constexpr double a62 = -355.0 / 33.0;
        constexpr double a63 = 46732.0 / 5247.0;
        constexpr double a64 = 49.0 / 176.0;
        constexpr double a65 = -5103.0 / 18656.0;

        constexpr double b1 = 35.0 / 384.0;
        constexpr double b3 = 500.0 / 1113.0;
        constexpr double b4 = 125.0 / 192.0;
        constexpr double b5 = -2187.0 / 6784.0;
        constexpr double b6 = 11.0 / 84.0;

        constexpr double bs1 = 5179.0 / 57600.0;
        constexpr double bs3 = 7571.0 / 16695.0;
        constexpr double bs4 = 393.0 / 640.0;
        constexpr double bs5 = -92097.0 / 339200.0;
        constexpr double bs6 = 187.0 / 2100.0;
        constexpr double bs7 = 1.0 / 40.0;

        const StateType k1 = system.evaluate_derivatives(y);
        const StateType k2 = system.evaluate_derivatives(y + k1 * (a21 * dt));
        const StateType k3 = system.evaluate_derivatives(y + (k1 * a31 + k2 * a32) * dt);
        const StateType k4 = system.evaluate_derivatives(y + (k1 * a41 + k2 * a42 + k3 * a43) * dt);
        const StateType k5 = system.evaluate_derivatives(y + (k1 * a51 + k2 * a52 + k3 * a53 + k4 * a54) * dt);
        const StateType k6 = system.evaluate_derivatives(y + (k1 * a61 + k2 * a62 + k3 * a63 + k4 * a64 + k5 * a65) * dt);

        const StateType y5 = y + (k1 * b1 + k3 * b3 + k4 * b4 + k5 * b5 + k6 * b6) * dt;
        const StateType k7 = system.evaluate_derivatives(y5);
        const StateType y4 = y + (k1 * bs1 + k3 * bs3 + k4 * bs4 + k5 * bs5 + k6 * bs6 + k7 * bs7) * dt;

        double max_err_ratio = 0.0;
        constexpr std::size_t n_components = sizeof(StateType) / sizeof(double);

        for (std::size_t i = 0; i < n_components; ++i) {
            const double diff = std::abs(y5[i] - y4[i]);
            const double scale = config_.abs_tol + config_.rel_tol * std::max(std::abs(y[i]), std::abs(y5[i]));
            const double ratio = diff / scale;
            if (ratio > max_err_ratio) {
                max_err_ratio = ratio;
            }
        }

        StepResult result;
        result.actual_dt = dt;
        result.error_ratio = max_err_ratio;
        result.accepted = (max_err_ratio <= 1.0);

        double factor = 0.9 * std::pow(std::max(1e-10, max_err_ratio), -0.2);
        factor = std::clamp(factor, 0.2, 5.0);

        double next_dt = dt * factor;
        next_dt = std::clamp(next_dt, config_.min_step_size, config_.max_step_size);
        result.next_dt = next_dt;

        if (result.accepted) {
            result.next_state = y5;
        } else {
            result.next_state = y;
        }

        return result;
    }

    [[nodiscard]] const IntegratorConfig& config() const noexcept { return config_; }
    void set_config(const IntegratorConfig& config) noexcept { config_ = config; }

private:
    IntegratorConfig config_;
};

}
