# Graph Report - gr_raytracing  (2026-10-17)

## Corpus Check
- 43 files · ~12,798 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 343 nodes · 436 edges · 22 communities (18 shown, 3 thin omitted)
- Extraction: 97% EXTRACTED · 3% INFERRED · 0% AMBIGUOUS · INFERRED: 12 edges (avg confidence: 0.81)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `99fc4557`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- Phase 2 — Build the actual ray-tracing engine
- GeodesicState
- Image
- Matrix4
- Metric
- Coordinates
- IntegratorConfig
- State
- Camera
- RayTracer
- 2. Core Modules and Contracts
- General Relativistic Ray Tracing Theory
- 5. Implementation order and weekly schedule
- Physical and Numerical Limitations
- Scientific Validation Protocol
- test_metrics.cpp
- analyze_shadow.py
- validate_schwarzschild.py
- Development Log
- validate_ode.py
- test_geodesics.cpp

## God Nodes (most connected - your core abstractions)
1. `GeodesicState` - 26 edges
2. `Coordinates` - 24 edges
3. `Metric` - 21 edges
4. `Matrix4` - 14 edges
5. `Camera` - 12 edges
6. `ObserverFrame` - 12 edges
7. `RayResult` - 12 edges
8. `Image` - 12 edges
9. `IntegratorConfig` - 11 edges
10. `RayTracer` - 11 edges

## Surprising Connections (you probably didn't know these)
- `Camera::generate_ray()` --calls--> `GeodesicState`  [EXTRACTED]
  src/camera/Camera.cpp → include/grrt/geodesics/GeodesicState.hpp
- `Renderer::render()` --references--> `Camera`  [EXTRACTED]
  src/rendering/Renderer.cpp → include/grrt/camera/Camera.hpp
- `HamiltonianSystem::evaluate_derivatives()` --references--> `GeodesicState`  [EXTRACTED]
  src/geodesics/HamiltonianSystem.cpp → include/grrt/geodesics/GeodesicState.hpp
- `HamiltonianSystem::evaluate_hamiltonian()` --references--> `GeodesicState`  [EXTRACTED]
  src/geodesics/HamiltonianSystem.cpp → include/grrt/geodesics/GeodesicState.hpp
- `KerrMetric::evaluate()` --references--> `Coordinates`  [EXTRACTED]
  src/metrics/KerrMetric.cpp → include/grrt/math/Coordinates.hpp

## Import Cycles
- None detected.

## Communities (22 total, 3 thin omitted)

### Community 0 - "Exact Project Specification & Physics Roadmap | Exact Project Specification & Physics Roadmap | Phase 2 — Build the actual ray-tracing engine"
Cohesion: 0.04
Nodes (44): 1. Lock the technology stack, 2. Define exactly what you are building, 3. Exact implementation sequence, 4. Final repository architecture, Fix these assumptions for version 1, General-Relativistic Ray Tracer: Exact Project Specification, Phase 0 — Set up the project and prove your numerical toolkit works, Phase 1 — Build a Schwarzschild light-bending simulator (+36 more)

### Community 1 - "Phase Space (x^mu, p_mu) & Thin Accretion Disk Physics | Phase Space (x^mu, p_mu) & Thin Accretion Disk Physics | GeodesicState"
Cohesion: 0.06
Nodes (23): GeodesicState, data, array, size_t, operator+(), AccretionDisk, enabled_, DiskIntersection (+15 more)

### Community 2 - "Radiative Transfer & Multi-Threaded Pixel Pipeline | Radiative Transfer & Multi-Threaded Pixel Pipeline | Image"
Cohesion: 0.07
Nodes (27): ColorRGB, b, g, r, uint8_t, Image, data_, get_pixel (+19 more)

### Community 3 - "Metric Tensors (g_munu, g^munu) & Tetrad Algebra | Metric Tensors (g_munu, g^munu) & Tetrad Algebra | Matrix4"
Cohesion: 0.09
Nodes (17): array, size_t, Matrix4, data, operator+(), array, MetricDerivatives, d_cov (+9 more)

### Community 4 - "Schwarzschild & Kerr Spacetime Geometries | Schwarzschild & Kerr Spacetime Geometries | Metric"
Cohesion: 0.08
Nodes (16): HamiltonianSystem, backward_, evaluate_derivatives, evaluate_hamiltonian, metric_, KerrMetric, evaluate, horizon_radius (+8 more)

### Community 5 - "Spacetime 4-Coordinates & Observer Tetrad (e_(a)^mu) | Spacetime 4-Coordinates & Observer Tetrad (e_(a)^mu) | Coordinates"
Cohesion: 0.10
Nodes (16): array, ObserverFrame, local_to_coordinate_momentum, lower_momentum, position_, sample_, tetrad_, Coordinates (+8 more)

### Community 6 - "Adaptive Dormand-Prince 5(4) Geodesic Stepper | Adaptive Dormand-Prince 5(4) Geodesic Stepper | IntegratorConfig"
Cohesion: 0.10
Nodes (19): size_t, Integrator, config_, IntegratorConfig, abs_tol, initial_step_size, max_step_size, max_steps (+11 more)

### Community 7 - "Phase-Space Vector Primitives & Dynamic State | Phase-Space Vector Primitives & Dynamic State | State"
Cohesion: 0.11
Nodes (14): array, size_t, operator+(), State, data, N, State2, Oscillator (+6 more)

### Community 8 - "Observer Camera & Screen Ray Projection | Observer Camera & Screen Ray Projection | Camera"
Cohesion: 0.15
Nodes (13): Camera, aspect_ratio_, config_, frame_, generate_ray, tan_half_fov_, CameraConfig, fov_radians (+5 more)

### Community 9 - "Null Geodesic Ray Tracer & Event Horizon Capture | Null Geodesic Ray Tracer & Event Horizon Capture | RayTracer"
Cohesion: 0.17
Nodes (9): RayTracer, config_, trace_ray, RayTracerConfig, capture_margin, disk, escape_radius, integrator_config (+1 more)

### Community 10 - "System Architecture & GR Layer Contracts | System Architecture & GR Layer Contracts | 2. Core Modules and Contracts"
Cohesion: 0.17
Nodes (11): 1. Architectural Philosophy and Layering, 2.1 Math Layer (`grrt/math`), 2.2 Metrics Layer (`grrt/metrics`), 2.3 Geodesics Layer (`grrt/geodesics`), 2.4 Camera Layer (`grrt/camera`), 2.5 Ray Tracing Layer (`grrt/raytracing`), 2.6 Rendering Layer (`grrt/rendering`), 2.7 Objects Layer (`grrt/objects`) (+3 more)

### Community 11 - "General Relativistic Field Theory & Geodesic Equations | General Relativistic Field Theory & Geodesic Equations | General Relativistic Ray Tracing Theory"
Cohesion: 0.22
Nodes (8): 1. Geometric Conventions and Spacetime Interval, 2.1 Schwarzschild Spacetime, 2.2 Kerr Spacetime, 2. Spacetime Metrics, 3. Hamiltonian Geodesic Equations, 4. Observer Orthonormal Tetrad, Backward Ray Tracing Convention, General Relativistic Ray Tracing Theory

### Community 12 - "Numerical Geodesic Milestones & Acceptance Criteria | Numerical Geodesic Milestones & Acceptance Criteria | 5. Implementation order and weekly schedule"
Cohesion: 0.33
Nodes (6): 0/8, 5. Implementation order and weekly schedule, 6. Exact build and test workflow, 7. The scientific validation standard, 8. What you should do first, Required validation checklist

### Community 13 - "Coordinate Singularities & Physical Horizons | Coordinate Singularities & Physical Horizons | Physical and Numerical Limitations"
Cohesion: 0.40
Nodes (4): 1. Coordinate Singularities, 2. Polar Singularities, 3. Geometric Optics Approximation, Physical and Numerical Limitations

### Community 14 - "Empirical Validation & Shadow Analysis Standards | Empirical Validation & Shadow Analysis Standards | Scientific Validation Protocol"
Cohesion: 0.40
Nodes (4): 1. Validation Checklist, 2. Independent Reference Solvers, 3. Quantitative Shadow Measurements, Scientific Validation Protocol

### Community 15 - "Metric Symmetry & Horizon Verification Tests | Metric Symmetry & Horizon Verification Tests | test_metrics.cpp"
Cohesion: 0.50
Nodes (3): "Kerr metric inverse and symmetry with non-zero spin", "Kerr metric reduces to Schwarzschild at zero spin", "Schwarzschild metric inverse and symmetry"

### Community 16 - "Black Hole Shadow Area & Circularity Residuals | Black Hole Shadow Area & Circularity Residuals | analyze_shadow.py"
Cohesion: 0.83
Nodes (3): extract_shadow_metrics(), main(), parse_ppm()

### Community 17 - "Schwarzschild Geodesic Deflection & Capture Benchmark | Schwarzschild Geodesic Deflection & Capture Benchmark | validate_schwarzschild.py"
Cohesion: 0.83
Nodes (3): compute_deflection(), orbit_equations(), run_validation()

## Knowledge Gaps
- **142 isolated node(s):** `position`, `width`, `height`, `fov_radians`, `generate_ray` (+137 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 197 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **3 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Metric` connect `Metric` to `GeodesicState`, `Image`, `Matrix4`, `Coordinates`, `Camera`?**
  _High betweenness centrality (0.184) - this node is a cross-community bridge._
- **Why does `GeodesicState` connect `GeodesicState` to `Camera`, `Matrix4`, `Metric`, `Coordinates`?**
  _High betweenness centrality (0.099) - this node is a cross-community bridge._
- **Why does `Coordinates` connect `Coordinates` to `Camera`, `GeodesicState`, `Matrix4`?**
  _High betweenness centrality (0.076) - this node is a cross-community bridge._
- **What connects `position`, `width`, `height` to the rest of the system?**
  _142 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Phase 2 — Build the actual ray-tracing engine` be split into smaller, more focused modules?**
  _Cohesion score 0.044444444444444446 - nodes in this community are weakly interconnected._
- **Should `GeodesicState` be split into smaller, more focused modules?**
  _Cohesion score 0.06090808416389812 - nodes in this community are weakly interconnected._
- **Should `Image` be split into smaller, more focused modules?**
  _Cohesion score 0.07207207207207207 - nodes in this community are weakly interconnected._