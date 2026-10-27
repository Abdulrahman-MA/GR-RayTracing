# General Relativistic Ray Tracing Theory

## 1. Geometric Conventions and Spacetime Interval

The simulation adopts geometric units where the speed of light $c = 1$ and Newton's gravitational constant $G = 1$. The central black hole mass is normalized to $M = 1$.
The metric signature follows the standard general relativity convention:
$(-, +, +, +)$.

The spacetime interval between neighboring events is:
$$ds^2 = g_{\mu\nu} dx^\mu dx^\nu$$

For photon propagation (null geodesics), the interval identically vanishes:
$$ds^2 = 0$$

## 2. Spacetime Metrics

### 2.1 Schwarzschild Spacetime

The line element in standard Schwarzschild spherical coordinates $(t, r, \theta, \phi)$ is:
$$ds^2 = -\left(1-\frac{2M}{r}\right)dt^2 + \left(1-\frac{2M}{r}\right)^{-1}dr^2 + r^2 d\theta^2 + r^2 \sin^2\theta d\phi^2$$

Event horizon:
$$r_h = 2M$$

Photon sphere:
$$r_{\text{ph}} = 3M$$

Critical impact parameter for photon capture from infinity:
$$b_{\text{crit}} = 3\sqrt{3}M \approx 5.196152 M$$

Weak-field light deflection angle for $b \gg M$:
$$\alpha \approx \frac{4M}{b}$$

### 2.2 Kerr Spacetime

In Boyer-Lindquist coordinates $(t, r, \theta, \phi)$ with spin parameter $a = J/M$ ($|a| \le M$):
$$\Sigma = r^2 + a^2 \cos^2\theta$$
$$\Delta = r^2 - 2Mr + a^2$$

The line element is:
$$ds^2 = -\left(1 - \frac{2Mr}{\Sigma}\right)dt^2 - \frac{4Mar\sin^2\theta}{\Sigma} dt d\phi + \frac{\Sigma}{\Delta} dr^2 + \Sigma d\theta^2 + \left(r^2 + a^2 + \frac{2Ma^2r\sin^2\theta}{\Sigma}\right) \sin^2\theta d\phi^2$$

Outer event horizon:
$$r_+ = M + \sqrt{M^2 - a^2}$$

In the limit $a \to 0$, the metric reduces to the Schwarzschild metric.

## 3. Hamiltonian Geodesic Equations

Light rays are parameterized by an affine parameter $\lambda$. The null geodesic Lagrangian corresponds to the Hamiltonian:
$$H(x, p) = \frac{1}{2} g^{\mu\nu}(x) p_\mu p_\nu = 0$$

Hamilton's equations of motion:
$$\frac{dx^\mu}{d\lambda} = \frac{\partial H}{\partial p_\mu} = g^{\mu\nu} p_\nu$$
$$\frac{dp_\mu}{d\lambda} = -\frac{\partial H}{\partial x^\mu} = -\frac{1}{2} \left(\frac{\partial g^{\alpha\beta}}{\partial x^\mu}\right) p_\alpha p_\beta$$

Using the identity $\partial_\mu g^{-1} = -g^{-1} (\partial_\mu g) g^{-1}$, the equations can equivalently be written using covariant metric derivatives:
$$\frac{dp_\mu}{d\lambda} = \frac{1}{2} \left(\frac{\partial g_{\alpha\beta}}{\partial x^\mu}\right) p^\alpha p^\beta$$

### Backward Ray Tracing Convention

Rendering computes light arriving at the observer by integrating backward from the camera into the scene:
$$\frac{dx^\mu}{d\lambda_{\text{back}}} = -\frac{dx^\mu}{d\lambda} = -g^{\mu\nu} p_\nu$$
$$\frac{dp_\mu}{d\lambda_{\text{back}}} = -\frac{dp_\mu}{d\lambda} = \frac{1}{2} \left(\frac{\partial g^{\alpha\beta}}{\partial x^\mu}\right) p_\alpha p_\beta$$

Future-directed photon momenta satisfy $p_t < 0$ (energy $E = -p_t > 0$).

## 4. Observer Orthonormal Tetrad

At the camera position $x_{\text{obs}}$, a local orthonormal tetrad $e_{(a)}^\mu$ ($a = 0, 1, 2, 3$) satisfies:
$$g_{\mu\nu} e_{(a)}^\mu e_{(b)}^\nu = \eta_{ab} = \text{diag}(-1, 1, 1, 1)$$

Given a pixel on the screen corresponding to a unit spatial direction $\hat{n} = (n_1, n_2, n_3)$ in the observer's rest frame, the local 4-momentum is:
$$p^{(a)} = (-1, n_1, n_2, n_3)$$

The coordinate 4-momentum is reconstructed as:
$$p^\mu = p^{(a)} e_{(a)}^\mu$$

The covariant momentum is obtained by lowering indices:
$$p_\mu = g_{\mu\nu} p^\nu$$

This formulation guarantees that the initial ray satisfies the null Hamiltonian condition $H = 0$.

## 5. Symbolic Derivation & C++ Tensor Verification

To bridge first-principles General Relativity with the C++20 numerical engine, the project includes a symbolic SymPy tensor engine (`GREngine`):
- **Reusable Symbolic Module & Validator:** [`tools/symbolic_gr.py`](../tools/symbolic_gr.py)
- **Interactive Derivation Notebook:** [`notebooks/01_symbolic_gr_metrics_and_derivations.ipynb`](../notebooks/01_symbolic_gr_metrics_and_derivations.ipynb)

### Key Symbolic Proofs Included:
1. **Birkhoff's Theorem & Schwarzschild Solution:** Starting from a general spherically symmetric time-dependent metric ansatz $g_{\mu\nu} = \text{diag}(-e^{2\alpha(t,r)}, e^{2\beta(t,r)}, r^2, r^2\sin^2\theta)$, solving $R_{tr} = 0$, $R^t_t - R^r_r = 0$, and $R_{\theta\theta} = 0$ proves staticity and yields $e^{2\alpha(r)} = 1 - \frac{2M}{r}$.
2. **Curvature Invariants:** Computes the Kretschmann scalar $K = R_{abcd}R^{abcd} = \frac{48M^2}{r^6}$, proving that $r = 2M$ is a coordinate singularity while $r = 0$ is a physical curvature singularity.
3. **Kerr Geometry & Frame-Dragging:** Symbolically verifies the Boyer-Lindquist inverse metric $g_{\mu\alpha}g^{\alpha\nu} = \delta_\mu^\nu$, outer/inner horizons $r_\pm = M \pm \sqrt{M^2 - a^2}$, static limit (ergosphere) $r_E(\theta) = M + \sqrt{M^2 - a^2\cos^2\theta}$, and ZAMO frame-dragging angular velocity $\omega = -g_{t\phi}/g_{\phi\phi}$.
4. **Exact C++ Derivative Verification:** Symbolically differentiates both Schwarzschild and Kerr metrics ($\partial_r g_{\mu\nu}, \partial_\theta g_{\mu\nu}, \partial_\mu g^{\alpha\beta}$) and asserts 100% mathematical equivalence with the analytical expressions in `SchwarzschildMetric.cpp` and `KerrMetric.cpp`.
