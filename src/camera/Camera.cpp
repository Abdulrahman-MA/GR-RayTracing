#include "grrt/camera/Camera.hpp"
#include <cmath>

namespace grrt {

Camera::Camera(const Metric& metric, CameraConfig config)
    : config_(config),
      frame_(metric, config.position),
      tan_half_fov_(std::tan(config.fov_radians * 0.5)),
      aspect_ratio_(static_cast<double>(config.width) / static_cast<double>(config.height)) {}

GeodesicState Camera::generate_ray(int pixel_x, int pixel_y) const {
    const double u = (2.0 * (pixel_x + 0.5) / static_cast<double>(config_.width) - 1.0) * tan_half_fov_ * aspect_ratio_;
    const double v = (1.0 - 2.0 * (pixel_y + 0.5) / static_cast<double>(config_.height)) * tan_half_fov_;

    const double dir1 = -1.0;
    const double dir2 = -v;
    const double dir3 = u;
    const double norm = std::sqrt(dir1 * dir1 + dir2 * dir2 + dir3 * dir3);

    const double n1 = dir1 / norm;
    const double n2 = dir2 / norm;
    const double n3 = dir3 / norm;

    const std::array<double, 4> p_contra = frame_.local_to_coordinate_momentum(n1, n2, n3);
    const std::array<double, 4> p_cov = frame_.lower_momentum(p_contra);

    return GeodesicState(frame_.position(), p_cov);
}

}
