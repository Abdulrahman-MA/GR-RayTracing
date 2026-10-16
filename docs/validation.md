# Scientific Validation Protocol

## 1. Validation Checklist

| Verification Standard | Metric / Target | Status |
| :--- | :--- | :--- |
| 1. ODE Convergence | RK4 achieves 4th-order $O(h^4)$ slope on harmonic oscillator | Verified |
| 2. Weak-Field Deflection | Schwarzschild light bending approaches $4M/b$ for $b \gg M$ | Verified |
| 3. Critical Capture Threshold | Photon capture boundary asymptotically approaches $b_{\text{crit}} = 3\sqrt{3}M \approx 5.196M$ | Verified |
| 4. Kerr Metric Reduction | Kerr metric tensor and inverse match Schwarzschild when spin $a = 0$ | Verified |
| 5. Conservation Laws | Null constraint $|H| < 10^{-6}$ and conserved $p_t, p_\phi$ preserved | Verified |
| 6. Shadow Resolution Convergence | Boundary extraction remains stable under resolution and tolerance tightening | Verified |
| 7. Reproducible Parameter Sweeps | JSON-driven experiment configurations | Verified |
| 8. Failure Segregation | Numerical divergence and step budget exhaustion segregated from physical capture | Verified |

## 2. Independent Reference Solvers

The C++ implementation is benchmarked against independent Python SciPy solvers (`tools/validate_ode.py` and `tools/validate_schwarzschild.py`).
The Python reference solvers run adaptive Runge-Kutta 4(5) with explicit zero-crossing event detection to verify:
1. Exact harmonic oscillator trajectories.
2. Asymptotic deflection angles across sweeps of impact parameters $b \in [20, 200] M$.
3. Analytical capture threshold at $b_{\text{crit}} = 3\sqrt{3} M$.

## 3. Quantitative Shadow Measurements

Following Cunha and Herdeiro (2018), shadow analysis relies on:
1. **Area-Equivalent Radius**:
   $$R_A = \sqrt{\frac{A}{\pi}}$$
   where $A$ is the enclosed shadow area in the image plane.
2. **Circularity Residual**:
   Fit a circle of radius $R_0$ to the detected boundary $(x_k, y_k)$ and calculate the root-mean-square residual:
   $$\sigma_R = \frac{1}{R_0} \sqrt{\frac{1}{N} \sum_{k=1}^N (r_k - R_0)^2}$$
   For Schwarzschild, $\sigma_R \approx 0$ due to spherical symmetry. For Kerr at non-zero inclination, $\sigma_R > 0$ quantifies the characteristic asymmetry resulting from frame-dragging.
