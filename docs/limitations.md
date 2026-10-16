# Physical and Numerical Limitations

## 1. Coordinate Singularities

The engine utilizes standard Schwarzschild coordinates $(t, r, \theta, \phi)$ for static black holes and Boyer-Lindquist coordinates for rotating Kerr black holes.

Both coordinate systems possess coordinate singularities at their respective event horizons:
- Schwarzschild: $r = 2M$
- Kerr: $r = r_+ = M + \sqrt{M^2 - a^2}$

To prevent numerical divergence caused by coordinate singularities, rays are evaluated against a capture cutoff boundary:
$$r_{\text{cutoff}} = r_+ + \epsilon$$
Rays penetrating this boundary are classified as captured. Future versions may adopt horizon-penetrating coordinates (e.g. Kerr-Schild coordinates) to trace trajectories through horizons.

## 2. Polar Singularities

In spherical and Boyer-Lindquist coordinates, the coordinate transformation becomes degenerate at the poles $\theta = 0$ and $\theta = \pi$ where $\sin\theta \to 0$. Observers and cameras are positioned at small non-zero inclinations away from exact coordinate poles, or regularized.

## 3. Geometric Optics Approximation

Light propagation is modeled strictly under geometric optics as null geodesics in vacuum spacetime. The model does not simulate:
1. Wave optics, diffraction, and polarization rotation.
2. Dynamic spacetime evolution (Einstein field equations are static background solutions).
3. General relativistic magnetohydrodynamics (GRMHD) of turbulent plasma.
4. Optical depth and scattering within accretion media.
