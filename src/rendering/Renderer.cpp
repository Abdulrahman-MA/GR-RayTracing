#include "grrt/rendering/Renderer.hpp"
#include <algorithm>
#include <cmath>
#include <thread>
#include <vector>

namespace grrt {

Image Renderer::render(
    const Camera& camera, const Metric& metric, const RayTracer& ray_tracer) const {
    const int width = camera.config().width;
    const int height = camera.config().height;
    Image image(width, height);

    unsigned int threads = config_.num_threads;
    if (threads == 0) {
        threads = std::max(1u, std::thread::hardware_concurrency());
    }

    auto render_chunk = [&](int y_start, int y_end) {
        for (int y = y_start; y < y_end; ++y) {
            for (int x = 0; x < width; ++x) {
                const GeodesicState ray = camera.generate_ray(x, y);
                const RayResult res = ray_tracer.trace_ray(ray, metric);

                ColorRGB color = config_.failure_color;
                switch (res.outcome) {
                    case RayOutcome::Captured:
                        color = config_.captured_color;
                        break;
                    case RayOutcome::Escaped:
                        color = config_.escaped_color;
                        break;
                    case RayOutcome::DiskIntersection: {
                        const double intensity = std::clamp(res.disk_intersection.intensity * 255.0, 0.0, 255.0);
                        const auto byte_val = static_cast<std::uint8_t>(intensity);
                        color = ColorRGB{byte_val, static_cast<std::uint8_t>(byte_val * 0.7), static_cast<std::uint8_t>(byte_val * 0.2)};
                        break;
                    }
                    case RayOutcome::MaximumSteps:
                        color = config_.max_steps_color;
                        break;
                    case RayOutcome::NumericalFailure:
                    default:
                        color = config_.failure_color;
                        break;
                }
                image.set_pixel(x, y, color);
            }
        }
    };

    std::vector<std::thread> workers;
    workers.reserve(threads);
    const int chunk_size = (height + static_cast<int>(threads) - 1) / static_cast<int>(threads);

    for (unsigned int t = 0; t < threads; ++t) {
        const int y_start = static_cast<int>(t) * chunk_size;
        const int y_end = std::min(height, y_start + chunk_size);
        if (y_start < height) {
            workers.emplace_back(render_chunk, y_start, y_end);
        }
    }

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    return image;
}

}
