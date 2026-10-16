# General-Relativistic Ray Tracer: Exact Project Specification

C++20 · General relativity · Numerical integration · Scientific visualization

My recommendation is to build a Schwarzschild-to-Kerr black-hole ray tracer in C++, with Python used only for independent validation and scientific analysis.

Do not begin by trying to simulate a realistic black hole in every detail. First build a scientifically verified numerical engine, then extend it until it can render rotating black-hole shadows and, eventually, synthetic accretion-disk images.

## 1. Lock the technology stack

Use this stack for the initial project. Do not introduce Julia, JAX, CuPy or CUDA in the first version.

## Selected technologies

C++20 — primary language

All metric calculations, geodesic integration, camera logic, ray classification and rendering.

CMake — build system

Reproducible builds, dependencies, executable targets and automated tests. Follow the [official CMake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/?utm_source=chatgpt.com). 

[image](https://www.google.com/s2/favicons?domain=https://cmake.org&sz=32)

CMake 4.4.4 Documentation



Boost.Odeint — numerical solver

Use its adaptive Dormand–Prince 5(4) method for the main geodesic integrator. Implement RK4 yourself only as a learning exercise and independent fixed-step comparison. Odeint documents adaptive error control and recommends Dormand–Prince as a general-purpose default. 

[image](https://www.google.com/s2/favicons?domain=https://www.boost.org&sz=32)

boost.org

+1



Eigen 3 — small matrix operations

Use fixed-size matrices for spacetime metrics, inverse metrics and coordinate transformations. Keep the eight-component ray state separate from Eigen's matrix types.

Catch2 — automated testing

Test metrics, equations, conservation laws, convergence and ray outcomes. [Official testing tutorial](https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md?utm_source=chatgpt.com). 

[image](https://www.google.com/s2/favicons?domain=https://github.com&sz=32)

GitHub



Python, NumPy, SciPy and Matplotlib — validation

Create an independent reference solver, compare selected ray trajectories and produce convergence plots. SciPy's `solve_ivp` supports adaptive solvers, error tolerances and event detection, making it useful for checking your C++ results. 

[image](https://www.google.com/s2/favicons?domain=https://docs.scipy.org&sz=32)

SciPy v1.15.3 Manual



For image output, begin with simple PPM files written directly from C++. Add PNG output later. This avoids making image libraries a prerequisite for the physics.

Why this stack: C++ is your application; Boost.Odeint handles the numerical stepping; Eigen handles small matrices; Catch2 checks correctness; Python provides an independent scientific reference. Every component has a defined job.

## 2. Define exactly what you are building

Project name: `gr-raytracer`

The final project will answer this research question:

> How do black-hole spin and observer inclination affect the apparent shadow boundary, and how sensitive are the results to numerical integration accuracy?

It will progress through three physical models.

| Model                          | Purpose                               | Required outcome                                                     |
| ------------------------------ | ------------------------------------- | -------------------------------------------------------------------- |
| Schwarzschild black hole       | Establish a verified baseline         | Correct light deflection and circular shadow                         |
| Kerr black hole                | Introduce rotation and frame dragging | Spin-dependent shadow images                                         |
| Kerr with a thin emitting disk | Introduce realistic image formation   | Synthetic images including lensing and relativistic frequency shifts |

The first two models constitute the core project. The disk is an advanced extension.

### Fix these assumptions for version 1

Use these conventions throughout the initial implementation:

- Geometric units: \(G=c=1\).
- Black-hole mass: \(M=1\).
- Metric signature: \((-+++)\).
- Coordinates: Schwarzschild spherical coordinates for the first model, then Boyer–Lindquist coordinates for Kerr.
- Matter and plasma: omitted initially.
- Background: a simple, fixed background or a star field.
- Photon trajectories: geometric optics, represented by null geodesics.
- Observer: a virtual camera at a large but finite radius.
- Rendering: backward ray tracing from the camera into the scene.

This means version 1 models light propagation in an idealized spacetime. It does not simulate the black hole's formation, solve Einstein's field equations dynamically, or attempt to reproduce a real telescope image.

# 3. Exact implementation sequence

## Phase 0 — Set up the project and prove your numerical toolkit works

Beginner

Objective: establish a clean C++ project and a working ODE solver before introducing curved spacetime.

### Step 0.1 — Install the development environment

Install:

- A C++20-compatible compiler.
- CMake.
- Boost, including Odeint.
- Eigen 3.
- Catch2.
- Python with NumPy, SciPy and Matplotlib.
- Git.

On Windows, MSVC with a Visual Studio C++ workload is a suitable option. Use a consistent dependency setup, such as vcpkg, rather than manually mixing libraries from unrelated installations.

### Step 0.2 — Create the initial repository

Start with only these files:

```
gr-raytracer/
├── CMakeLists.txt
├── include/
│   └── grrt/
│       └── math/
│           └── State.hpp
├── src/
│   └── main.cpp
├── tests/
│   └── test_ode.cpp
├── tools/
│   └── validate_ode.py
└── docs/
    └── development-log.md
```

Do not generate the entire final folder tree at once. Add the metric, camera and rendering modules when you reach their respective milestones.

### Step 0.3 — Implement and test an ODE solver

Use a simple harmonic oscillator:

\[ \frac{dx}{dt}=v,\qquad \frac{dv}{dt}=-\omega^2x. \]

Implement it using RK4 for educational purposes, then run it with Boost.Odeint's adaptive Dormand–Prince integrator.

Record the numerical solution and compare it with the analytical solution.

### Step 0.4 — Verify numerical convergence

Run the same problem with progressively smaller fixed step sizes for RK4 and compare the error. Check that the solution approaches the analytical result at the expected rate.

Deliverable: a working C++ executable, a passing test suite, a CSV file and a plot showing numerical error versus step size.

Completion criterion: the ODE solver converges, the test suite passes, and you can explain absolute error, relative error, and the effect of the integration tolerance.

Do not proceed while this numerical baseline is unreliable.

## Phase 1 — Build a Schwarzschild light-bending simulator

Beginner

Objective: understand the equations for light propagation in a curved spacetime and reproduce an analytical prediction before rendering images.

### Step 1.1 — Study the minimum required theory

Study the following topics in this order:

1. Spacetime intervals and the metric tensor.
2. Null curves and the condition \(ds^2=0\).
3. Conserved energy and angular momentum in a stationary, spherically symmetric spacetime.
4. The Schwarzschild solution.
5. The geodesic equation and numerical integration.

You can learn the first four while implementing this phase. You do not need to complete an entire general-relativity course before starting.

### Step 1.2 — Implement the Schwarzschild geometry

The metric is

\[ ds^2 = -\left(1-\frac{2M}{r}\right)dt^2 +\left(1-\frac{2M}{r}\right)^{-1}dr^2 +r^2d\theta^2+r^2\sin^2\theta\,d\phi^2. \]

Implement a small `SchwarzschildMetric` component. Initially, it should only calculate the metric components and their derivatives.

For the first tests, do not attempt to render an entire image. Verify that the metric components have the correct values at known radii and that the metric tensor and inverse metric satisfy

\[ g_{\mu\alpha}g^{\alpha\nu}=\delta_\mu^{\ \nu}. \]

### Step 1.3 — Implement a simplified photon orbit

Begin with a photon constrained to the equatorial plane. You can derive a reduced orbit equation from the Schwarzschild geodesic equations:

\[ \frac{d^2u}{d\phi^2}=3Mu^2-u, \qquad u=\frac1r. \]

Write it as a first-order system:

\[ \frac{du}{d\phi}=v,\qquad \frac{dv}{d\phi}=3Mu^2-u. \]

This is a deliberately restricted model. It lets you study light bending without dealing with all four spacetime coordinates or camera transformations.

Integrate from an approximately asymptotic incoming ray to its outgoing branch. Extract the deflection angle by comparing the total angular change with the straight-line value \(\pi\).

### Step 1.4 — Validate against analytical results

In geometric units, the weak-field deflection angle approaches

\[ \alpha \approx \frac{4M}{b} \]

for impact parameter \(b\gg M\).

For Schwarzschild, the critical impact parameter for a ray arriving from infinity is

\[ b_{\mathrm{crit}}=3\sqrt{3}\,M. \]

The photon sphere is at \(r=3M\), while the event horizon is at \(r=2M\). These provide benchmark values for the simulation. 

[image](https://www.google.com/s2/favicons?domain=https://www.sciencedirect.com&sz=32)

ScienceDirect



Run a sweep over \(b\). Plot deflection angle against impact parameter and distinguish rays that escape from those that approach the capture region.

### Step 1.5 — Implement numerical stopping conditions

Add explicit conditions for:

- Reaching the outgoing asymptotic region.
- Approaching a defined capture radius.
- Exceeding a maximum number of integration steps.
- Encountering an invalid numerical state.

A solver failure or step budget exhaustion is not evidence that the ray was captured. Record such cases as integration failures.

Deliverable: a command-line simulator for Schwarzschild light bending, with trajectory data and a benchmark report.

Completion criterion: the weak-field deflection and critical impact parameter agree with their expected values within a documented numerical tolerance, and the results converge as the integration is refined.

## Phase 2 — Build the actual ray-tracing engine

Intermediate

Objective: replace the reduced orbit model with a reusable four-dimensional geodesic engine. This becomes the foundation for both Schwarzschild and Kerr.

### Step 2.1 — Define the ray state

Represent a ray by four coordinates and four covariant momenta:

\[ y=(t,r,\theta,\phi,p_t,p_r,p_\theta,p_\phi). \]

In C++, use a fixed-size state, such as `std::array<double, 8>`, and define named indices or a lightweight state wrapper.

Create a central `GeodesicState` type. Avoid scattering numeric indices such as `state[5]` through the entire codebase.

### Step 2.2 — Implement the Hamiltonian formulation

Use the null Hamiltonian

\[ H(x,p)=\frac12g^{\mu\nu}(x)p_\mu p_\nu=0. \]

Its equations of motion are

\[ \frac{dx^\mu}{d\lambda}=g^{\mu\nu}p_\nu, \]

\[ \frac{dp_\mu}{d\lambda} =-\frac12 \frac{\partial g^{\alpha\beta}}{\partial x^\mu} p_\alpha p_\beta. \]

Here \(\lambda\) is an affine parameter.

These equations provide a common formulation for different metrics. You will not need an entirely separate numerical solver for Kerr.

For the initial implementation, expose each ray's derivative as a function that maps the eight-component state to its eight derivatives.

### Step 2.3 — Refactor the metric interface

Introduce an abstract metric interface.

```
struct MetricSample {
    Matrix4 covariant;
    Matrix4 inverse;
    MetricDerivatives derivatives;
};

class Metric {
public:
    virtual ~Metric() = default;

    virtual MetricSample evaluate(
        const Coordinates& x
    ) const = 0;
};
```

The matrix and coordinate types are project-defined. `MetricDerivatives` should hold the first coordinate derivatives needed by the geodesic equations.

Implement `SchwarzschildMetric` behind this interface. The Hamiltonian system must depend on `Metric`, not directly on Schwarzschild-specific formulas.

For accuracy, implement analytical metric derivatives for the final solver. Finite differences are useful as an independent derivative check during development. You may obtain inverse-metric derivatives through the identity

\[ \partial_\mu g^{-1} =-g^{-1}(\partial_\mu g)g^{-1}. \]

### Step 2.4 — Implement adaptive numerical integration

Use Boost.Odeint's adaptive Dormand–Prince method as the default integrator.

Define configurable parameters for:

- Absolute tolerance.
- Relative tolerance.
- Initial step size.
- Minimum allowed step size.
- Maximum allowed step size.
- Maximum number of steps.

Do not treat the solver's estimated local error as a guarantee of the final shadow-boundary error. You must measure the convergence of physical observables separately.

### Step 2.5 — Preserve backward ray tracing consistently

For rendering, rays originate at the camera and are followed backward into the scene.

Choose one sign convention and use it consistently. One convenient approach is to retain future-directed photon momenta but integrate the Hamiltonian system with the affine evolution reversed. This avoids confusing the physical sign of the photon energy with the direction of numerical tracing.

Document this convention in `docs/theory.md`. Add a test confirming that the null constraint remains satisfied under the chosen convention.

### Step 2.6 — Add event and termination handling

Represent ray outcomes explicitly, for example:

```
enum class RayOutcome {
    Captured,
    Escaped,
    DiskIntersection,
    MaximumSteps,
    NumericalFailure
};
```

At this stage, the disk outcome can be a placeholder that is implemented later.

Use an outer escape boundary and a configurable capture boundary outside the event horizon. Because the standard Schwarzschild and Boyer–Lindquist coordinate systems have coordinate singularities at their horizons, do not blindly integrate through that coordinate singularity. Check that your classification stays stable when the capture cutoff and integration tolerances change.

### Step 2.7 — Test conservation laws

For each integrated ray, monitor:

\[ H=\frac12g^{\mu\nu}p_\mu p_\nu, \]

along with the conserved covariant momenta \(p_t\) and \(p_\phi\) for stationary, axisymmetric metrics.

Measure the normalized drift over a trajectory. Define a scale-aware normalization instead of relying exclusively on absolute error, particularly when the conserved energy is not unity.

Deliverable: a metric-independent Hamiltonian geodesic solver, passing tests for Schwarzschild, including null-constraint and conservation-law checks.

Completion criterion: the same geodesic engine reproduces the earlier Schwarzschild benchmarks, and its errors improve predictably as the solver tolerances are tightened.

## Phase 3 — Build the camera and render the Schwarzschild shadow

Intermediate

Objective: turn individual photon trajectories into an image.

### Step 3.1 — Implement the virtual camera

Create a `Camera` component with:

- Position in spacetime.
- Viewing direction and orientation.
- Image width and height.
- Field of view.
- Near and far scene boundaries.

The camera begins at a large radius compared with \(M\), so the initial observer is far from the black hole's strong-field region.

### Step 3.2 — Construct initial photon directions

For each pixel, calculate its local viewing direction and normalize the photon momentum using a local orthonormal observer frame.

Transform the resulting four-momentum into coordinate components using the observer's tetrad. Then initialize the geodesic state.

This is a crucial step: setting coordinate-space momentum components directly from pixel coordinates generally produces incorrect physical viewing directions.

Begin with a simple camera model and verify the coordinate transformation on known directions before generating a full image.

### Step 3.3 — Render the image

For every pixel:

1. Construct its initial ray.
2. Integrate backward.
3. Detect the ray's outcome.
4. Assign a color based on the outcome.
5. Write the pixel into an image buffer.

For this stage, use simple colors for the capture region, escaping background and numerical failures. Do not add plasma, temperature profiles or realistic lighting.

Write the buffer to a PPM image.

### Step 3.4 — Test image-level convergence

Render at multiple resolutions and integration tolerances. Extract the boundary between captured and escaping rays and compare it across runs.

For an ideal Schwarzschild black hole viewed by a distant observer, the shadow is circular. Its angular size depends on the observer distance; its asymptotic critical impact parameter is \(3\sqrt{3}M\). 

[image](https://www.google.com/s2/favicons?domain=https://www.sciencedirect.com&sz=32)

ScienceDirect



Your code should recover that behavior within its measured numerical error.

Deliverable: a black-hole shadow renderer with configurable camera settings and automated boundary measurements.

Completion criterion: increasing image resolution refines the boundary, and tighter numerical tolerances do not cause uncontrolled changes to the measured shadow.

## Phase 4 — Add the Kerr metric and rotating black holes

Intermediate → Advanced

Objective: generalize the engine to a rotating spacetime and study how the shadow changes with spin and viewing angle.

Do not modify the geodesic integrator to special-case Kerr. Implement Kerr as another metric using the same interface.

### Step 4.1 — Study the Kerr geometry

Use the Kerr metric in Boyer–Lindquist coordinates. Define

\[ \Sigma=r^2+a^2\cos^2\theta, \qquad \Delta=r^2-2Mr+a^2. \]

In these coordinates, the nonzero metric components are

\[ g_{tt}=-\left(1-\frac{2Mr}{\Sigma}\right), \]

\[ g_{t\phi}=g_{\phi t} =-\frac{2Mar\sin^2\theta}{\Sigma}, \]

\[ g_{rr}=\frac{\Sigma}{\Delta}, \qquad g_{\theta\theta}=\Sigma, \]

\[ g_{\phi\phi} =\left(r^2+a^2+ \frac{2Ma^2r\sin^2\theta}{\Sigma}\right) \sin^2\theta. \]

The full line element includes the cross term \(2g_{t\phi}\,dt\,d\phi\).

The spin parameter obeys \(|a|\le M\) for a Kerr black hole. The outer horizon is located at

\[ r_+=M+\sqrt{M^2-a^2}. \]

At \(a=0\), the metric must reduce to Schwarzschild. This limiting case will be one of your most important tests.

### Step 4.2 — Implement `KerrMetric`

Add a new class that evaluates the metric tensor, its inverse and analytical coordinate derivatives.

Implement the inverse metric using either validated closed-form expressions or a tested small-matrix inversion. Verify the results against the covariant metric.

Do not derive the Kerr expressions independently without cross-checking them against a reliable GR reference. Errors in signs, factors of two or coordinate conventions can produce images that look plausible but represent the wrong geometry.

### Step 4.3 — Validate the Kerr implementation before rendering

Write tests for:

1. Metric symmetry.
2. The inverse relationship between \(g_{\mu\nu}\) and \(g^{\mu\nu}\).
3. Agreement with Schwarzschild when \(a=0\).
4. Agreement of analytical derivatives with numerical derivative checks away from coordinate singularities.
5. Conservation of \(p_t\) and \(p_\phi\) along integrated trajectories.
6. Preservation of the null Hamiltonian.

Only proceed once these tests pass.

### Step 4.4 — Run a controlled spin experiment

Set \(M=1\). Begin with this explicit experiment matrix:

| Spin \(a/M\) | Inclinations to test |
| ------------ | -------------------- |
| 0.0          | 0°, 30°, 60°, 90°    |
| 0.5          | 0°, 30°, 60°, 90°    |
| 0.9          | 0°, 30°, 60°, 90°    |
| 0.99         | 0°, 30°, 60°, 90°    |

These are chosen simulation parameters, not claims about the spins of specific observed black holes. The polar view provides a useful symmetry check; generic inclinations reveal the deformation and asymmetry associated with rotation. 

[image](https://www.google.com/s2/favicons?domain=https://www.sciencedirect.com&sz=32)

ScienceDirect



For every combination, save the image and the numerical settings used to produce it.

### Step 4.5 — Measure the shadow rather than relying on appearance

Implement a boundary-extraction tool and calculate at least two quantities:

- Area-equivalent radius: \(R_A=\sqrt{A/\pi}\), where \(A\) is the measured shadow area in the image plane.
- Circularity residual: fit a circle to the measured boundary and calculate the normalized RMS radial residual around that fitted circle.

These are project-defined image measurements. They are not substitutes for a full analysis of the observable shadow in a particular astrophysical system.

Record how both quantities change with spin, inclination, image resolution, numerical tolerance and capture cutoff.

### Step 4.6 — Compare the two metrics

Your output must include:

- Schwarzschild and Kerr images at matched camera settings.
- Plots of boundary measurements against spin.
- A plot or table of numerical convergence.
- A short explanation of the physical source of each observed difference.

Deliverable: a general-purpose C++ Schwarzschild/Kerr ray tracer, with reproducible parameter sweeps and quantitative shadow measurements.

Completion criterion: the \(a=0\) limit reproduces Schwarzschild, conservation checks pass, and the spin-dependent results remain stable under numerical refinement.

At this point, you already have a complete and defensible scientific-computing project. Continue to the next phase only after this version is validated.

## Phase 5 — Add a thin accretion disk

Advanced · Optional extension

Objective: turn a binary black-hole shadow into a synthetic image with light emitted by matter surrounding the black hole.

This is a different level of modeling. A ray tracer predicts where photons travel, but to calculate brightness you must also specify how matter emits radiation.

### Step 5.1 — Implement disk geometry

Create an idealized, geometrically thin disk in the equatorial plane:

\[ \theta=\frac{\pi}{2}. \]

Define its inner and outer radii and provide a function to test whether a ray intersects the disk. Begin with a simple, transparent or uniformly emitting disk.

### Step 5.2 — Implement an emission model

Choose a documented model for how the emitting material's brightness varies with radius. Initially, a prescribed emissivity function is sufficient; you do not need to solve the relativistic fluid equations that generate the disk.

Keep this model separate from the metric and geodesic solver.

### Step 5.3 — Calculate relativistic frequency shifts

For photon four-momentum \(p^\mu\), the frequency measured by an observer with four-velocity \(u^\mu\) is proportional to

\[ \nu\propto -p_\mu u^\mu. \]

Calculate the frequency ratio between emission and observation. This incorporates the relevant gravitational and kinematic frequency shifts, provided the photon momentum and both observer four-velocities use the same conventions.

### Step 5.4 — Render the emitted brightness

For each ray intersecting the disk:

1. Find the emission location.
2. Calculate the local emission properties.
3. Calculate the emitter-to-observer frequency ratio.
4. Apply the corresponding relativistic radiative transformation.
5. Accumulate the contribution into the output image.

For vacuum propagation, the invariant specific intensity is \(I_\nu/\nu^3\). This provides the foundation for transforming emitted spectral intensity to observed spectral intensity along each ray.

### Step 5.5 — Validate the new physics separately

Check limiting cases before looking for visual realism:

- Turning off emission restores the dark shadow and background.
- An appropriate zero-velocity or weak-field limit removes the corresponding kinematic or gravitational shift.
- Disk intersections converge when the integration tolerance is tightened.
- The intensity transformation is consistent with the chosen frequency and observer conventions.

Deliverable: a synthetic Kerr black-hole image with a thin emitting disk and documented physical assumptions.

Completion criterion: geometric lensing, disk emission and relativistic frequency shifts can be enabled, disabled and tested independently.

# 4. Final repository architecture

Use this structure once Phase 2 starts. You do not need all these files on day one.

```
gr-raytracer/
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── configs/
│   ├── schwarzschild.json
│   └── kerr.json
├── include/
│   └── grrt/
│       ├── math/
│       │   ├── Coordinates.hpp
│       │   └── Matrix4.hpp
│       ├── metrics/
│       │   ├── Metric.hpp
│       │   ├── SchwarzschildMetric.hpp
│       │   └── KerrMetric.hpp
│       ├── geodesics/
│       │   ├── GeodesicState.hpp
│       │   ├── HamiltonianSystem.hpp
│       │   └── Integrator.hpp
│       ├── camera/
│       │   ├── Camera.hpp
│       │   └── ObserverFrame.hpp
│       ├── raytracing/
│       │   ├── RayTracer.hpp
│       │   └── RayOutcome.hpp
│       ├── rendering/
│       │   ├── Image.hpp
│       │   └── Renderer.hpp
│       └── objects/
│           └── AccretionDisk.hpp
├── src/
│   ├── metrics/
│   ├── geodesics/
│   ├── camera/
│   ├── raytracing/
│   ├── rendering/
│   └── main.cpp
├── tests/
│   ├── test_ode.cpp
│   ├── test_metrics.cpp
│   ├── test_geodesics.cpp
│   ├── test_conservation.cpp
│   ├── test_convergence.cpp
│   └── test_ray_classification.cpp
├── tools/
│   ├── validate_schwarzschild.py
│   └── analyze_shadow.py
├── results/
│   ├── images/
│   ├── trajectories/
│   └── measurements/
└── docs/
    ├── theory.md
    ├── architecture.md
    ├── validation.md
    └── limitations.md
```

Architecture rule: metrics provide geometry; the Hamiltonian system computes derivatives; the integrator advances the state; the ray tracer handles termination; the camera initializes rays; the renderer converts outcomes into pixels. Keep these responsibilities separate.

# 5. Implementation order and weekly schedule

A practical target is 12–16 weeks at around 6–8 hours per week, assuming you already know C++ fundamentals and can learn the required GR alongside implementation. Treat this as a planning estimate rather than a deadline.

| Weeks | Implementation work                                               | Required output                                      |
| ----- | ----------------------------------------------------------------- | ---------------------------------------------------- |
| 1     | Environment, CMake, tests, RK4 and adaptive integration           | Tested ODE solver                                    |
| 2–3   | Schwarzschild theory and reduced photon orbit                     | Light-deflection benchmark                           |
| 4–5   | Metric interface, Hamiltonian system, full state integration      | Verified Schwarzschild geodesics                     |
| 6–7   | Camera, initial ray conditions, ray termination and PPM rendering | First shadow image                                   |
| 8–10  | Kerr metric, observer frame validation and spin experiments       | Rotating-black-hole shadow images                    |
| 11–12 | Boundary extraction, convergence tests and controlled comparison  | Quantitative results                                 |
| 13–16 | Optional disk emission, advanced testing and report               | Synthetic astrophysical images or research extension |

Do not move to the next phase just because the planned week has ended. Move when the current phase's acceptance tests pass.

## 6. Exact build and test workflow

From the project root, first verify that CMake can build and test a basic executable.

```
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

These commands work with common CMake generators; multi-configuration generators such as Visual Studio use `--config Debug`, while single-configuration generators also commonly use a build-time configuration option.

After the numerical library and ray tracer exist, run the tests before each experiment. Keep Debug builds for correctness and use Release builds for performance measurements.

Every experiment should record:

- Metric and physical parameters.
- Camera position, inclination and image resolution.
- Integrator and numerical tolerances.
- Capture and escape boundaries.
- Number of rays, integration failures and total runtime.
- Output image and measured shadow properties.

Put this information in a configuration file or a metadata file alongside each result. Otherwise, it becomes difficult to determine whether an image changed because of the physics, camera geometry or numerical settings.

## 7. The scientific validation standard

Your final report should contain evidence that the simulation is correct, not only screenshots of its output.

## Required validation checklist

Validation progress

### 0/8

The numerical integrator passes convergence tests on an analytical ODE.

The Schwarzschild weak-field deflection approaches 4M/b.

The Schwarzschild capture threshold approaches 3√3 M.

The Kerr metric reduces correctly to Schwarzschild when a = 0.

The null Hamiltonian and conserved quantities remain within documented error bounds.

The shadow boundary converges with integration tolerance and image resolution.

The Kerr spin and inclination sweep is reproducible from saved configuration files.

Failures are reported separately from captured or escaped rays.

Copy validation checklist

Use an independent Python implementation for selected reference trajectories, and compare the results at matched initial conditions and numerical precision. Do not make Python call the same C++ formulas and then treat agreement as independent verification.

For the shadow itself, report how the measured boundary changes under progressively tighter integration tolerances and finer image resolution. The analytical shadow review by Cunha and Herdeiro's broader research context is a useful entry point for understanding these benchmarks and their limitations. 

[image](https://www.google.com/s2/favicons?domain=https://www.sciencedirect.com&sz=32)

ScienceDirect



## 8. What you should do first

Begin with Phase 0 only. Your first working milestone is a C++ program that integrates a harmonic oscillator, outputs its state to CSV, and demonstrates convergence against the analytical solution.

Then implement the reduced Schwarzschild orbit equation. Do not create the Kerr class, camera, renderer or accretion disk until you have reproduced the light-deflection benchmark.

Keep these references available throughout implementation:

- [Boost.Odeint documentation](https://www.boost.org/doc/libs/latest/libs/numeric/odeint/doc/html/boost_numeric_odeint/getting_started/overview.html?utm_source=chatgpt.com) for numerical integration.
- [SciPy solve_ivp documentation](https://docs.scipy.org/doc/scipy/reference/generated/scipy.integrate.solve_ivp.html?utm_source=chatgpt.com) for the independent Python reference solver.
- [CMake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/?utm_source=chatgpt.com) for the build system.
- [Calculating black hole shadows: Review of analytical studies](https://www.sciencedirect.com/science/article/pii/S0370157321003811?utm_source=chatgpt.com) for the black-hole physics and analytical benchmarks.

The final target is Release 2: a validated C++ engine that traces null geodesics in Schwarzschild and Kerr spacetimes and produces quantitative shadow measurements. That is the core project. Accretion-disk rendering is a valuable advanced extension, not a prerequisite for calling the core project complete.
