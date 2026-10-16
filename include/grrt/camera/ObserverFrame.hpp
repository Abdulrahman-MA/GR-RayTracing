#pragma once

#include <array>
#include "grrt/math/Coordinates.hpp"
#include "grrt/metrics/Metric.hpp"

namespace grrt {

class ObserverFrame {
public:
    ObserverFrame(const Metric& metric, const Coordinates& observer_pos);

    [[nodiscard]] const Coordinates& position() const noexcept { return position_; }
    [[nodiscard]] const std::array<Coordinates, 4>& tetrad() const noexcept { return tetrad_; }

    [[nodiscard]] std::array<double, 4> local_to_coordinate_momentum(
        double n1, double n2, double n3) const;

    [[nodiscard]] std::array<double, 4> lower_momentum(
        const std::array<double, 4>& contravariant_p) const;

private:
    Coordinates position_;
    std::array<Coordinates, 4> tetrad_{};
    MetricSample sample_{};
};

}
