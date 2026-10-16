cmake_minimum_required(VERSION 3.20)

project(gr-raytracer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8 /O2)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3)
endif()

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)

FetchContent_MakeAvailable(Catch2)

add_library(grrt_core STATIC
    src/metrics/SchwarzschildMetric.cpp
    src/metrics/KerrMetric.cpp
    src/geodesics/HamiltonianSystem.cpp
    src/camera/ObserverFrame.cpp
    src/camera/Camera.cpp
    src/raytracing/RayTracer.cpp
    src/rendering/Image.cpp
    src/rendering/Renderer.cpp
)

target_include_directories(grrt_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(gr_raytracer
    src/main.cpp
)

target_link_libraries(gr_raytracer PRIVATE
    grrt_core
)

enable_testing()

add_executable(grrt_tests
    tests/test_ode.cpp
    tests/test_metrics.cpp
    tests/test_geodesics.cpp
    tests/test_conservation.cpp
    tests/test_convergence.cpp
    tests/test_ray_classification.cpp
)

target_link_libraries(grrt_tests PRIVATE
    grrt_core
    Catch2::Catch2WithMain
)

include(CTest)
add_test(NAME AllTests COMMAND grrt_tests)
