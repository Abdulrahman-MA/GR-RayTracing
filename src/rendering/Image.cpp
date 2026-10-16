#include "grrt/rendering/Image.hpp"
#include <fstream>

namespace grrt {

Image::Image(int width, int height)
    : width_(width), height_(height), data_(static_cast<std::size_t>(width * height * 3), 0) {}

void Image::set_pixel(int x, int y, ColorRGB color) {
    set_pixel(x, y, color.r, color.g, color.b);
}

void Image::set_pixel(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return;
    }
    const std::size_t idx = static_cast<std::size_t>((y * width_ + x) * 3);
    data_[idx] = r;
    data_[idx + 1] = g;
    data_[idx + 2] = b;
}

ColorRGB Image::get_pixel(int x, int y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return ColorRGB{0, 0, 0};
    }
    const std::size_t idx = static_cast<std::size_t>((y * width_ + x) * 3);
    return ColorRGB{data_[idx], data_[idx + 1], data_[idx + 2]};
}

bool Image::write_ppm(const std::string& filepath) const {
    std::ofstream out(filepath, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }

    out << "P6\n" << width_ << " " << height_ << "\n255\n";
    out.write(reinterpret_cast<const char*>(data_.data()), static_cast<std::streamsize>(data_.size()));
    return out.good();
}

}
