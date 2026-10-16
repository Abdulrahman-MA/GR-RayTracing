# Development Log

## Milestone 0: Initialization and Project Baseline

- Established Git repository linked with remote `https://github.com/Abdulrahman-MA/GR-RayTracing`.
- Initialized core repository specification and architectural guidelines.
- Configured modern C++20 CMake build system with strict warnings, optimization flags, and test target.
- Defined flat, zero-heap data models for spacetime coordinates, fixed-size matrices, and geodesic ray states.
- Implemented numerical solvers: fixed-step 4th-order Runge-Kutta (RK4) and adaptive Dormand-Prince 5(4).
- Implemented Schwarzschild and Kerr spacetimes with exact analytical metric derivatives.
- Formulated Hamiltonian geodesic equations with backward ray tracing support.
- Built virtual camera and local orthonormal tetrad frame transformation.
- Implemented multi-threaded scanline renderer producing binary P6 Netpbm images.
- Designed automated Catch2 test suite covering ODEs, metrics, geodesics, conservation laws, and ray classification.
- Implemented independent Python SciPy validation scripts for baseline checks and shadow analysis.
