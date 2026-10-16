#pragma once

#include <cstdint>

namespace grrt {

enum class RayOutcome : std::uint8_t {
    Captured = 0,
    Escaped = 1,
    DiskIntersection = 2,
    MaximumSteps = 3,
    NumericalFailure = 4
};

}
