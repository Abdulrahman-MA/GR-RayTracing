#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include "grrt/camera/Camera.hpp"
#include "grrt/metrics/KerrMetric.hpp"
#include "grrt/metrics/SchwarzschildMetric.hpp"
#include "grrt/raytracing/RayTracer.hpp"
#include "grrt/rendering/Renderer.hpp"

int main(int argc, char* argv[]) {
    std::string mode = "schwarzschild";
    if (argc > 1) {
        mode = argv[1];
    }

    std::filesystem::create_directories("results/images");

    std::unique_ptr<grrt::Metric> metric;
    std::string output_filename;

    if (mode == "kerr") {
        metric = std::make_unique<grrt::KerrMetric>(1.0, 0.9);
        output_filename = "results/images/kerr_shadow.ppm";
        std::cout << "Rendering Kerr black hole (M=1.0, a=0.9)...\n";
    } else {
        metric = std::make_unique<grrt::SchwarzschildMetric>(1.0);
        output_filename = "results/images/schwarzschild_shadow.ppm";
        std::cout << "Rendering Schwarzschild black hole (M=1.0)...\n";
    }

    grrt::CameraConfig cam_config;
    cam_config.width = 256;
    cam_config.height = 256;
    cam_config.fov_radians = 0.35;
    cam_config.position = grrt::Coordinates(0.0, 40.0, 1.48, 0.0);

    const grrt::Camera camera(*metric, cam_config);

    grrt::RayTracerConfig tracer_config;
    tracer_config.escape_radius = 80.0;
    tracer_config.capture_margin = 0.05;
    tracer_config.integrator_config.abs_tol = 1e-7;
    tracer_config.integrator_config.rel_tol = 1e-7;
    tracer_config.integrator_config.initial_step_size = 0.05;
    tracer_config.integrator_config.min_step_size = 1e-5;
    tracer_config.integrator_config.max_step_size = 0.5;
    tracer_config.integrator_config.max_steps = 10000;

    const grrt::RayTracer ray_tracer(tracer_config);
    const grrt::Renderer renderer;

    const auto start_time = std::chrono::high_resolution_clock::now();
    const grrt::Image image = renderer.render(camera, *metric, ray_tracer);
    const auto end_time = std::chrono::high_resolution_clock::now();

    const std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Render completed in " << elapsed.count() << " seconds.\n";

    if (image.write_ppm(output_filename)) {
        std::cout << "Saved image to: " << output_filename << "\n";
    } else {
        std::cerr << "Failed to save image to: " << output_filename << "\n";
        return 1;
    }

    return 0;
}
