#pragma once

#include "grrt/camera/ObserverFrame.hpp"
#include "grrt/geodesics/GeodesicState.hpp"
#include "grrt/math/Coordinates.hpp"
#include "grrt/metrics/Metric.hpp"

namespace grrt {

struct CameraConfig {
    Coordinates position{0.0, 50.0, 1.5707963267948966, 0.0};
    int width{256};
    int height{256};
    double fov_radians{0.4};
};

class Camera {
public:
    Camera(const Metric& metric, CameraConfig config);

    [[nodiscard]] GeodesicState generate_ray(int pixel_x, int pixel_y) const;
    [[nodiscard]] const CameraConfig& config() const noexcept { return config_; }
    [[nodiscard]] const ObserverFrame& frame() const noexcept { return frame_; }

private:
    CameraConfig config_;
    ObserverFrame frame_;
    double tan_half_fov_{0.0};
    double aspect_ratio_{1.0};
};

}
