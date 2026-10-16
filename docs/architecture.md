# Software Architecture and System Design

## 1. Architectural Philosophy and Layering

The engine is engineered around modularity, strict separation of concerns, and zero dynamic memory allocation in numerical inner loops.

```
+-------------------------------------------------------------+
|                          main.cpp                           |
+-------------------------------------------------------------+
                              |
                              v
+------------------+  +------------------+  +-----------------+
|     Renderer     |->|      Camera      |->|  ObserverFrame  |
+------------------+  +------------------+  +-----------------+
        |                      |                     |
        v                      v                     v
+------------------+  +------------------+  +-----------------+
|    RayTracer     |->| GeodesicState    |->|  Coordinates    |
+------------------+  +------------------+  +-----------------+
        |                      |                     |
        v                      v                     v
+------------------+  +------------------+  +-----------------+
|    Integrator    |->| HamiltonianSystem|->|     Matrix4     |
+------------------+  +------------------+  +-----------------+
                               |
                               v
                      +------------------+
                      |   Metric Base    |
                      +------------------+
                        /              \
                       v                v
            +--------------------+    +------------+
            |SchwarzschildMetric |    | KerrMetric |
            +--------------------+    +------------+
```

## 2. Core Modules and Contracts

### 2.1 Math Layer (`grrt/math`)
- `Coordinates`: 4-component spacetime coordinate $(t, r, \theta, \phi)$ with arithmetic operators.
- `Matrix4`: Fixed-size $4 \times 4$ double-precision matrix tailored for metric tensors, tetrad transformations, and inverse metric calculations.
- `State<N>`: N-dimensional fixed-size state vector for generic ODE solving.

### 2.2 Metrics Layer (`grrt/metrics`)
- `Metric`: Abstract base class declaring:
  - `evaluate(const Coordinates&)` returning `MetricSample` ($g_{\mu\nu}$, $g^{\mu\nu}$, $\partial_\lambda g_{\mu\nu}$, $\partial_\lambda g^{\mu\nu}$).
  - `horizon_radius()` returning outer event horizon.
  - `mass()` and `spin()`.
- `SchwarzschildMetric`: Exact analytical metric, inverse, and coordinate derivatives in spherical coordinates.
- `KerrMetric`: Exact Boyer-Lindquist formulation with analytical coordinate derivatives with respect to $r$ and $\theta$. Reduces to Schwarzschild when spin $a = 0$.

### 2.3 Geodesics Layer (`grrt/geodesics`)
- `GeodesicState`: Cache-aligned 8-component state vector $(t, r, \theta, \phi, p_t, p_r, p_\theta, p_\phi)$.
- `HamiltonianSystem`: Evaluates $dx^\mu/d\lambda$ and $dp_\mu/d\lambda$ according to the Hamiltonian equations with backward tracing sign handling.
- `Integrator`: Templated numerical solver providing both fixed-step Runge-Kutta 4 (RK4) and adaptive Dormand-Prince 5(4) with step-size control.

### 2.4 Camera Layer (`grrt/camera`)
- `ObserverFrame`: Computes the local orthonormal tetrad $e_{(a)}^\mu$ at the camera location.
- `Camera`: Maps pixel coordinates $(x, y)$ to screen rays, constructs unit directional vectors in the observer frame, and transforms them to covariant coordinate momentum $p_\mu$.

### 2.5 Ray Tracing Layer (`grrt/raytracing`)
- `RayOutcome`: Strongly-typed enum (`Captured`, `Escaped`, `DiskIntersection`, `MaximumSteps`, `NumericalFailure`).
- `RayTracer`: Integrates rays backward from the observer, monitors termination boundaries and conservation drift, and records final ray outcomes.

### 2.6 Rendering Layer (`grrt/rendering`)
- `Image`: Contiguous RGB byte buffer writing binary P6 Netpbm format.
- `Renderer`: Multi-threaded scanline dispatcher distributing image chunks across available hardware concurrency.

### 2.7 Objects Layer (`grrt/objects`)
- `AccretionDisk`: Models an equatorial geometrically thin emission disk, checking ray intersections and calculating gravitational/Doppler frequency shifts.

## 3. Performance Optimizations

1. **Stack Allocation**: All coordinates, matrices, and state vectors are flat `std::array` types allocated on the stack. No heap allocation occurs during ray integration.
2. **Analytical Derivatives**: Spacetime metric derivatives are computed analytically rather than using finite differences.
3. **Multi-Threaded Rendering**: Parallel chunking across scanlines achieves linear scaling with core count.
4. **Adaptive Integration**: Dormand-Prince 5(4) automatically enlarges steps in weak gravitational fields while shrinking near the photon sphere.
