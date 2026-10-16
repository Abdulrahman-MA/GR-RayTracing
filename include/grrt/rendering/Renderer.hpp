#pragma once

#include "grrt/camera/Camera.hpp"
#include "grrt/metrics/Metric.hpp"
#include "grrt/raytracing/RayTracer.hpp"
#include "grrt/rendering/Image.hpp"

namespace grrt {

struct RenderConfig {
    ColorRGB captured_color{0, 0, 0};
    ColorRGB escaped_color{25, 35, 65};
    ColorRGB failure_color{220, 30, 30};
    ColorRGB max_steps_color{200, 180, 20};
    unsigned int num_threads{0};
};

class Renderer {
public:
    explicit Renderer(RenderConfig config = {}) noexcept : config_(config) {}

    [[nodiscard]] Image render(
        const Camera& camera, const Metric& metric, const RayTracer& ray_tracer) const;

    [[nodiscard]] const RenderConfig& config() const noexcept { return config_; }
    void set_config(const RenderConfig& config) noexcept { config_ = config; }

private:
    RenderConfig config_;
};

}
