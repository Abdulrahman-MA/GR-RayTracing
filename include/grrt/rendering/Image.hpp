#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace grrt {

struct ColorRGB {
    std::uint8_t r{0};
    std::uint8_t g{0};
    std::uint8_t b{0};
};

class Image {
public:
    Image(int width, int height);

    [[nodiscard]] int width() const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }

    void set_pixel(int x, int y, ColorRGB color);
    void set_pixel(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b);
    [[nodiscard]] ColorRGB get_pixel(int x, int y) const;

    [[nodiscard]] bool write_ppm(const std::string& filepath) const;

private:
    int width_{0};
    int height_{0};
    std::vector<std::uint8_t> data_;
};

}
