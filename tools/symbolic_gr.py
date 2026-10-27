"""
Symbolic General Relativity Tensor & Geodesic Engine (GREngine)
===============================================================
Provides exact symbolic tensor calculus for curved 4D spacetimes using SymPy:
1. Covariant and Contravariant Metric Tensors (g_uv, g^uv)
2. Analytical Metric Partial Derivatives (dg_uv/dx^a, dg^uv/dx^a) for C++ verification
3. Christoffel Symbols of the Second Kind (Gamma^rho_{mu nu}) exploiting lower-index symmetry
4. Riemann Curvature Tensor (R^rho_{sigma mu nu}) exploiting skew-symmetry
5. Ricci Tensor (R_{mu nu}), Ricci Scalar (R), and Einstein Tensor (G_{mu nu})
6. Kretschmann Curvature Invariant (K = R_{abcd} R^{abcd})
7. Symbolic Derivations of Schwarzschild (Static + Birkhoff's Theorem) & Kerr Spacetimes
"""

import itertools
import sympy as sp


class GREngine:
    """
    A symbolic General Relativity engine for computing spacetime geometry tensors
    and Hamiltonian geodesic derivatives.

    Exploits index symmetries:
      - Christoffel symbols: Gamma^rho_{mu nu} = Gamma^rho_{nu mu}
      - Riemann tensor:      R^rho_{sigma mu nu} = -R^rho_{sigma nu mu}
      - Ricci tensor:        R_{mu nu} = R_{nu mu}
    """

    def __init__(self, coords, metric, metric_inv=None):
        """
        Args:
            coords (list[sp.Symbol]): Coordinate symbols [t, r, theta, phi].
            metric (sp.Matrix): 4x4 covariant metric tensor g_{mu nu}.
            metric_inv (sp.Matrix, optional): Precomputed or block-simplified inverse metric g^{mu nu}.
        """
        self.coords = list(coords)
        self.g = sp.Matrix(metric)
        self.n = len(self.coords)
        if metric_inv is not None:
            self.g_inv = sp.Matrix(metric_inv)
        else:
            self.g_inv = self._compute_fast_inverse(self.g)

        self._dg_cov = None
        self._dg_inv = None
        self._Gamma = None
        self._Riemann = None
        self._Ricci = None
        self._Ricci_scalar = None
        self._Einstein = None
        self._Kretschmann = None

    @staticmethod
    def _compute_fast_inverse(g):
        """
        Fast symbolic matrix inversion tailored for diagonal and stationary axisymmetric (t-phi block) metrics.
        Avoids exponential expression swell in SymPy's generic 4x4 LU solver.
        """
        if g.is_diagonal():
            return sp.diag(*[sp.cancel(1 / g[i, i]) for i in range(g.rows)])
        # Stationary axisymmetric block structure: (t, phi) 2x2 block + diagonal (r, theta)
        if (
            g.rows == 4
            and g[0, 1] == 0 and g[0, 2] == 0
            and g[1, 0] == 0 and g[1, 2] == 0 and g[1, 3] == 0
            and g[2, 0] == 0 and g[2, 1] == 0 and g[2, 3] == 0
            and g[3, 1] == 0 and g[3, 2] == 0
        ):
            det_tph = sp.cancel(g[0, 0] * g[3, 3] - g[0, 3] * g[3, 0])
            inv = sp.Matrix.zeros(4, 4)
            inv[0, 0] = sp.cancel(g[3, 3] / det_tph)
            inv[0, 3] = sp.cancel(-g[0, 3] / det_tph)
            inv[3, 0] = sp.cancel(-g[3, 0] / det_tph)
            inv[3, 3] = sp.cancel(g[0, 0] / det_tph)
            inv[1, 1] = sp.cancel(1 / g[1, 1])
            inv[2, 2] = sp.cancel(1 / g[2, 2])
            return inv
        return sp.simplify(g.inv())

    def metric_derivatives(self):
        """
        Computes partial derivatives of both covariant and inverse metric tensors
        with respect to each coordinate x^sigma:
            d_cov[sigma][mu, nu] = d(g_{mu nu}) / dx^sigma
            d_inv[sigma][mu, nu] = d(g^{mu nu}) / dx^sigma
        Matches the exact structure of `MetricDerivatives` in `include/grrt/metrics/Metric.hpp`.
        """
        if self._dg_cov is not None and self._dg_inv is not None:
            return self._dg_cov, self._dg_inv

        self._dg_cov = [
            self.g.applyfunc(lambda e: sp.cancel(sp.diff(e, x_sig))) for x_sig in self.coords
        ]
        # Identity: d(g^{-1}) = -g^{-1} (dg) g^{-1}
        self._dg_inv = [
            (-self.g_inv * dg * self.g_inv).applyfunc(sp.cancel) for dg in self._dg_cov
        ]
        return self._dg_cov, self._dg_inv

    def christoffel(self):
        """
        Computes Christoffel symbols of the second kind:
            Gamma^rho_{mu nu} = 1/2 g^{rho sigma} (d_nu g_{sigma mu} + d_mu g_{sigma nu} - d_sigma g_{mu nu})
        Exploits symmetry Gamma^rho_{mu nu} == Gamma^rho_{nu mu}.
        """
        if self._Gamma is not None:
            return self._Gamma

        n, coords, g, g_inv = self.n, self.coords, self.g, self.g_inv
        Gamma = sp.MutableDenseNDimArray.zeros(n, n, n)

        for rho in range(n):
            for mu in range(n):
                for nu in range(mu, n):
                    term = sp.Integer(0)
                    for sigma in range(n):
                        if g_inv[rho, sigma] != 0:
                            term += g_inv[rho, sigma] * (
                                sp.diff(g[sigma, mu], coords[nu])
                                + sp.diff(g[sigma, nu], coords[mu])
                                - sp.diff(g[mu, nu], coords[sigma])
                            )
                    if term != 0:
                        val = sp.simplify(term / 2)
                        Gamma[rho, mu, nu] = val
                        if mu != nu:
                            Gamma[rho, nu, mu] = val

        self._Gamma = Gamma
        return Gamma

    def riemann(self):
        """
        Computes the Riemann curvature tensor:
            R^rho_{sigma mu nu} = d_mu Gamma^rho_{nu sigma} - d_nu Gamma^rho_{mu sigma}
                                + Gamma^rho_{mu lam} Gamma^lam_{nu sigma}
                                - Gamma^rho_{nu lam} Gamma^lam_{mu sigma}
        Exploits antisymmetry in the last two indices (mu < nu).
        """
        if self._Riemann is not None:
            return self._Riemann

        Gamma = self.christoffel()
        n, coords = self.n, self.coords
        Riem = sp.MutableDenseNDimArray.zeros(n, n, n, n)

        for rho in range(n):
            for sigma in range(n):
                for mu in range(n):
                    for nu in range(mu + 1, n):
                        term = (
                            sp.diff(Gamma[rho, nu, sigma], coords[mu])
                            - sp.diff(Gamma[rho, mu, sigma], coords[nu])
                        )
                        for lam in range(n):
                            if Gamma[rho, mu, lam] != 0 and Gamma[lam, nu, sigma] != 0:
                                term += Gamma[rho, mu, lam] * Gamma[lam, nu, sigma]
                            if Gamma[rho, nu, lam] != 0 and Gamma[lam, mu, sigma] != 0:
                                term -= Gamma[rho, nu, lam] * Gamma[lam, mu, sigma]
                        if term != 0:
                            val = sp.simplify(term)
                            Riem[rho, sigma, mu, nu] = val
                            Riem[rho, sigma, nu, mu] = -val

        self._Riemann = Riem
        return Riem

    def ricci(self):
        """
        Computes the symmetric Ricci tensor by contracting the Riemann tensor:
            R_{mu nu} = R^rho_{mu rho nu}
        """
        if self._Ricci is not None:
            return self._Ricci

        Riem = self.riemann()
        n = self.n
        Ric = sp.MutableDenseMatrix.zeros(n, n)

        for mu in range(n):
            for nu in range(mu, n):
                term = sum(Riem[rho, mu, rho, nu] for rho in range(n))
                if term != 0:
                    val = sp.simplify(term)
                    Ric[mu, nu] = val
                    if mu != nu:
                        Ric[nu, mu] = val

        self._Ricci = Ric
        return Ric

    def ricci_scalar(self):
        """
        Computes the Ricci curvature scalar:
            R = g^{mu nu} R_{mu nu}
        """
        if self._Ricci_scalar is not None:
            return self._Ricci_scalar

        Ric = self.ricci()
        R_val = sum(
            self.g_inv[mu, nu] * Ric[mu, nu]
            for mu, nu in itertools.product(range(self.n), repeat=2)
            if self.g_inv[mu, nu] != 0 and Ric[mu, nu] != 0
        )
        self._Ricci_scalar = sp.simplify(R_val)
        return self._Ricci_scalar

    def einstein(self):
        """
        Computes the Einstein tensor:
            G_{mu nu} = R_{mu nu} - 1/2 R g_{mu nu}
        """
        if self._Einstein is not None:
            return self._Einstein

        Ric = self.ricci()
        R_scalar = self.ricci_scalar()
        G_mat = sp.MutableDenseMatrix.zeros(self.n, self.n)

        for mu in range(self.n):
            for nu in range(mu, self.n):
                val = sp.simplify(Ric[mu, nu] - sp.Rational(1, 2) * R_scalar * self.g[mu, nu])
                G_mat[mu, nu] = val
                if mu != nu:
                    G_mat[nu, mu] = val

        self._Einstein = G_mat
        return G_mat

    def kretschmann(self):
        """
        Computes the Kretschmann curvature scalar:
            K = R_{abcd} R^{abcd}
        Distinguishes physical curvature singularities from coordinate singularities.
        """
        if self._Kretschmann is not None:
            return self._Kretschmann

        R_uddd = self.riemann()
        n = self.n
        g, g_inv = self.g, self.g_inv

        # Lower first index: R_{a b c d} = g_{a e} R^e_{b c d}
        R_dddd = sp.MutableDenseNDimArray.zeros(n, n, n, n)
        for a, b, c, d in itertools.product(range(n), repeat=4):
            term = sum(g[a, e] * R_uddd[e, b, c, d] for e in range(n) if g[a, e] != 0 and R_uddd[e, b, c, d] != 0)
            if term != 0:
                R_dddd[a, b, c, d] = term

        # Raise all indices sequentially to form R^{a b c d}
        R_1 = sp.MutableDenseNDimArray.zeros(n, n, n, n)
        for a, b, c, d in itertools.product(range(n), repeat=4):
            term = sum(g_inv[a, e] * R_dddd[e, b, c, d] for e in range(n) if g_inv[a, e] != 0 and R_dddd[e, b, c, d] != 0)
            if term != 0:
                R_1[a, b, c, d] = term

        R_2 = sp.MutableDenseNDimArray.zeros(n, n, n, n)
        for a, b, c, d in itertools.product(range(n), repeat=4):
            term = sum(g_inv[b, e] * R_1[a, e, c, d] for e in range(n) if g_inv[b, e] != 0 and R_1[a, e, c, d] != 0)
            if term != 0:
                R_2[a, b, c, d] = term

        R_3 = sp.MutableDenseNDimArray.zeros(n, n, n, n)
        for a, b, c, d in itertools.product(range(n), repeat=4):
            term = sum(g_inv[c, e] * R_2[a, b, e, d] for e in range(n) if g_inv[c, e] != 0 and R_2[a, b, e, d] != 0)
            if term != 0:
                R_3[a, b, c, d] = term

        R_uuuu = sp.MutableDenseNDimArray.zeros(n, n, n, n)
        for a, b, c, d in itertools.product(range(n), repeat=4):
            term = sum(g_inv[d, e] * R_3[a, b, c, e] for e in range(n) if g_inv[d, e] != 0 and R_3[a, b, c, e] != 0)
            if term != 0:
                R_uuuu[a, b, c, d] = term

        K = sum(
            R_dddd[a, b, c, d] * R_uuuu[a, b, c, d]
            for a, b, c, d in itertools.product(range(n), repeat=4)
            if R_dddd[a, b, c, d] != 0 and R_uuuu[a, b, c, d] != 0
        )

        self._Kretschmann = sp.simplify(K)
        return self._Kretschmann

    def hamiltonian_rhs(self, momenta):
        """
        Derives the canonical Hamiltonian equations of motion for null geodesics:
            H(x, p) = 1/2 g^{mu nu} p_mu p_nu = 0
            dx^mu / dlambda = +dH / dp_mu = g^{mu nu} p_nu
            dp_mu / dlambda = -dH / dx^mu = -1/2 (d g^{alpha beta} / dx^mu) p_alpha p_beta
        Matches `HamiltonianSystem::evaluate_derivatives` in `src/geodesics/HamiltonianSystem.cpp`.
        """
        p = sp.Matrix(momenta)
        H = sp.simplify(sp.Rational(1, 2) * (p.T * self.g_inv * p)[0, 0])
        dx_dlam = [sp.simplify(sp.diff(H, p_mu)) for p_mu in momenta]
        dp_dlam = [sp.simplify(-sp.diff(H, x_mu)) for x_mu in self.coords]
        return H, dx_dlam, dp_dlam


def build_schwarzschild_symbolic():
    """Returns coordinates, parameters, and GREngine for the exact Schwarzschild metric."""
    t, r, th, ph = sp.symbols("t r theta phi", real=True)
    M = sp.Symbol("M", positive=True)
    f = 1 - 2 * M / r
    g_schw = sp.diag(-f, 1 / f, r**2, r**2 * sp.sin(th) ** 2)
    return [t, r, th, ph], M, GREngine([t, r, th, ph], g_schw)


def build_kerr_symbolic():
    """Returns coordinates, parameters, and GREngine for the exact Boyer-Lindquist Kerr metric."""
    t, r, th, ph = sp.symbols("t r theta phi", real=True)
    M, a = sp.symbols("M a", real=True)
    Sigma = r**2 + a**2 * sp.cos(th) ** 2
    Delta = r**2 - 2 * M * r + a**2

    g_tt = -(1 - 2 * M * r / Sigma)
    g_tph = -2 * M * a * r * sp.sin(th) ** 2 / Sigma
    g_rr = Sigma / Delta
    g_thth = Sigma
    g_phph = (r**2 + a**2 + 2 * M * a**2 * r * sp.sin(th) ** 2 / Sigma) * sp.sin(th) ** 2

    g_kerr = sp.Matrix([
        [g_tt, 0, 0, g_tph],
        [0, g_rr, 0, 0],
        [0, 0, g_thth, 0],
        [g_tph, 0, 0, g_phph],
    ])
    return [t, r, th, ph], (M, a, Sigma, Delta), GREngine([t, r, th, ph], g_kerr)


def verify_schwarzschild_derivation_and_cpp():
    """
    1. Derives the Schwarzschild metric from Einstein's Vacuum Field Equations (Birkhoff's Theorem).
    2. Verifies every tensor component and derivative in `src/metrics/SchwarzschildMetric.cpp`.
    """
    print("=" * 72)
    print("PART 1: SYMBOLIC DERIVATION OF SCHWARZSCHILD & BIRKHOFF'S THEOREM")
    print("=" * 72)

    t, r, th, ph = sp.symbols("t r theta phi", real=True)
    coords = [t, r, th, ph]
    alpha_tr = sp.Function("alpha")(t, r)
    beta_tr = sp.Function("beta")(t, r)

    g_birkhoff = sp.diag(-sp.exp(2 * alpha_tr), sp.exp(2 * beta_tr), r**2, r**2 * sp.sin(th) ** 2)
    birkhoff_engine = GREngine(coords, g_birkhoff)
    Ric_birkhoff = birkhoff_engine.ricci()

    # 1. Birkhoff time-independence constraint: R_{tr} = 2/r * d(beta)/dt = 0
    eq_tr = sp.simplify(Ric_birkhoff[0, 1])
    print(f"[Step 1] Birkhoff R_tr = {eq_tr} = 0  ==>  d(beta)/dt = 0  ==>  beta = beta(r)")

    # 2. Radial constraint: R^t_t - R^r_r = 0
    alpha_r = sp.Function("alpha")(r)
    beta_r = sp.Function("beta")(r)
    g_static_ansatz = sp.diag(-sp.exp(2 * alpha_r), sp.exp(2 * beta_r), r**2, r**2 * sp.sin(th) ** 2)
    static_engine = GREngine(coords, g_static_ansatz)
    Ric_static = static_engine.ricci()

    eq_radial = sp.simplify(static_engine.g_inv[0, 0] * Ric_static[0, 0] - static_engine.g_inv[1, 1] * Ric_static[1, 1])
    print(f"[Step 2] R^t_t - R^r_r = {eq_radial} = 0  ==>  alpha'(r) + beta'(r) = 0  ==>  beta(r) = -alpha(r)")

    # 3. Angular ODE: R_{theta theta} = 0
    y = sp.Function("y")(r)
    ode_expr = sp.simplify(Ric_static[2, 2].subs(beta_r, -alpha_r).subs(alpha_r, sp.log(y) / 2).doit())
    sol = sp.dsolve(sp.Eq(ode_expr, 0), y)
    C1 = list(sol.rhs.free_symbols - {r})[0]
    M = sp.Symbol("M", positive=True)
    y_final = sol.rhs.subs(C1, -2 * M)
    print(f"[Step 3] ODE R_{{theta theta}} = {ode_expr} = 0  ==>  f(r) = e^(2*alpha) = {y_final}")

    # 4. Validate exact Schwarzschild vacuum & Kretschmann scalar
    _, M_sym, schw_engine = build_schwarzschild_symbolic()
    Ric_schw = schw_engine.ricci()
    G_schw = schw_engine.einstein()
    K_schw = schw_engine.kretschmann()

    assert Ric_schw == sp.zeros(4, 4), "Schwarzschild Ricci tensor must vanish!"
    assert G_schw == sp.zeros(4, 4), "Schwarzschild Einstein tensor must vanish!"
    assert sp.simplify(K_schw - 48 * M_sym**2 / r**6) == 0, "Kretschmann scalar mismatch!"
    print(f"[Step 4] Vacuum verified: R_uv = 0, G_uv = 0")
    print(f"[Step 5] Kretschmann Curvature Scalar: K = {K_schw} (proves r=2M is coordinate singularity, r=0 is physical)")

    # 5. Verify C++ analytical derivatives in SchwarzschildMetric.cpp
    dg_cov, dg_inv = schw_engine.metric_derivatives()
    f = 1 - 2 * M_sym / r
    df_dr = 2 * M_sym / r**2
    d_inv_f_dr = -df_dr / f**2

    assert sp.simplify(dg_cov[1][0, 0] - (-df_dr)) == 0
    assert sp.simplify(dg_cov[1][1, 1] - d_inv_f_dr) == 0
    assert sp.simplify(dg_cov[1][2, 2] - 2 * r) == 0
    assert sp.simplify(dg_cov[1][3, 3] - 2 * r * sp.sin(th) ** 2) == 0
    assert sp.simplify(dg_cov[2][3, 3] - 2 * r**2 * sp.sin(th) * sp.cos(th)) == 0

    assert sp.simplify(dg_inv[1][0, 0] - (-d_inv_f_dr)) == 0
    assert sp.simplify(dg_inv[1][1, 1] - df_dr) == 0
    assert sp.simplify(dg_inv[1][2, 2] - (-2 / r**3)) == 0
    assert sp.simplify(dg_inv[1][3, 3] - (-2 / (r**3 * sp.sin(th) ** 2))) == 0
    assert sp.simplify(dg_inv[2][3, 3] - (-2 * sp.cos(th) / (r**2 * sp.sin(th) ** 3))) == 0
    print("[Step 6] C++ `SchwarzschildMetric.cpp` analytical derivatives: 100% EXACT MATCH\n")


def verify_kerr_and_cpp():
    """
    Verifies the rotating Kerr metric in Boyer-Lindquist coordinates:
    1. Reduction to Schwarzschild at a = 0.
    2. Exact inverse metric formula matching `src/metrics/KerrMetric.cpp`.
    3. Horizons, Ergosphere, and ZAMO frame-dragging angular velocity (`ObserverFrame.cpp`).
    4. Analytical partial derivatives d_r g_{uv} and d_theta g_{uv} matching `KerrMetric.cpp`.
    """
    print("=" * 72)
    print("PART 2: SYMBOLIC VERIFICATION OF ROTATING KERR METRIC & C++ ENGINE")
    print("=" * 72)

    coords, (M, a, Sigma, Delta), kerr_engine = build_kerr_symbolic()
    t, r, th, ph = coords

    # 1. Zero-spin reduction to Schwarzschild
    f_schw = 1 - 2 * M / r
    g_schw = sp.diag(-f_schw, 1 / f_schw, r**2, r**2 * sp.sin(th) ** 2)
    diff_a0 = sp.simplify(kerr_engine.g.subs(a, 0) - g_schw)
    assert diff_a0 == sp.zeros(4, 4), "Kerr(a=0) must equal Schwarzschild!"
    print("[Step 1] Zero-spin limit a -> 0: Identically reduces to Schwarzschild metric")

    # 2. Verify C++ closed-form inverse metric in KerrMetric.cpp
    sin2 = sp.sin(th) ** 2
    g_inv_cpp = sp.Matrix.zeros(4, 4)
    g_inv_cpp[0, 0] = -((r**2 + a**2) ** 2 - a**2 * Delta * sin2) / (Sigma * Delta)
    g_inv_cpp[0, 3] = -(2 * M * a * r) / (Sigma * Delta)
    g_inv_cpp[3, 0] = g_inv_cpp[0, 3]
    g_inv_cpp[1, 1] = Delta / Sigma
    g_inv_cpp[2, 2] = 1 / Sigma
    g_inv_cpp[3, 3] = (Delta - a**2 * sin2) / (Sigma * Delta * sin2)

    identity_check = (kerr_engine.g * g_inv_cpp).applyfunc(
        lambda e: sp.cancel(sp.expand(e).subs(sp.cos(th) ** 2, 1 - sp.sin(th) ** 2))
    )
    assert identity_check == sp.eye(4), "Kerr C++ inverse metric formula does not multiply to Identity!"
    print("[Step 2] C++ `KerrMetric.cpp` closed-form inverse g^{uv}: g_{ua} g^{av} = delta_u^v VERIFIED")

    # 3. Event horizons and Ergosphere
    horizons = sp.solve(sp.Eq(Delta, 0), r)
    ergo = sp.solve(sp.Eq(Sigma - 2 * M * r, 0), r)
    omega_zamo = sp.cancel(-kerr_engine.g[0, 3] / kerr_engine.g[3, 3])
    print(f"[Step 3] Event Horizons (Delta = 0): r_+/- = {horizons}")
    print(f"[Step 4] Ergosphere Boundary (g_tt = 0): r_E(theta) = {ergo}")
    print(f"[Step 5] ZAMO Frame-Dragging Angular Velocity (omega = -g_03/g_33): {omega_zamo}")

    # 4. Verify C++ analytical partial derivatives in KerrMetric.cpp
    dg_dr_sym = kerr_engine.g.diff(r)
    dg_dth_sym = kerr_engine.g.diff(th)

    inv_sigma = 1 / Sigma
    inv_delta = 1 / Delta
    d_sigma_dr = 2 * r
    d_sigma_dth = -2 * a**2 * sp.sin(th) * sp.cos(th)
    d_delta_dr = 2 * r - 2 * M
    d_inv_sigma_dr = -d_sigma_dr * inv_sigma**2
    d_inv_sigma_dth = -d_sigma_dth * inv_sigma**2

    # C++ expressions from src/metrics/KerrMetric.cpp lines 53-69
    cpp_dg_dr_00 = 2 * M * (inv_sigma + r * d_inv_sigma_dr)
    cpp_dg_dr_03 = -2 * M * a * sin2 * (inv_sigma + r * d_inv_sigma_dr)
    cpp_dg_dr_11 = (d_sigma_dr * Delta - Sigma * d_delta_dr) * inv_delta**2
    cpp_dg_dr_22 = d_sigma_dr
    cpp_dg_dr_33 = 2 * r * sin2 + 2 * M * a**2 * sin2**2 * (inv_sigma + r * d_inv_sigma_dr)

    assert sp.cancel(dg_dr_sym[0, 0] - cpp_dg_dr_00) == 0
    assert sp.cancel(dg_dr_sym[0, 3] - cpp_dg_dr_03) == 0
    assert sp.cancel(dg_dr_sym[1, 1] - cpp_dg_dr_11) == 0
    assert sp.cancel(dg_dr_sym[2, 2] - cpp_dg_dr_22) == 0
    assert sp.cancel(dg_dr_sym[3, 3] - cpp_dg_dr_33) == 0

    cpp_dg_dth_00 = 2 * M * r * d_inv_sigma_dth
    cpp_dg_dth_03 = -2 * M * a * r * (2 * sp.sin(th) * sp.cos(th) * inv_sigma + sin2 * d_inv_sigma_dth)
    cpp_dg_dth_11 = d_sigma_dth * inv_delta
    cpp_dg_dth_22 = d_sigma_dth
    cpp_dg_dth_33 = (
        2 * (r**2 + a**2) * sp.sin(th) * sp.cos(th)
        + 2 * M * a**2 * r * (4 * sin2 * sp.sin(th) * sp.cos(th) * inv_sigma + sin2**2 * d_inv_sigma_dth)
    )

    assert sp.cancel(dg_dth_sym[0, 0] - cpp_dg_dth_00) == 0
    assert sp.cancel(dg_dth_sym[0, 3] - cpp_dg_dth_03) == 0
    assert sp.cancel(dg_dth_sym[1, 1] - cpp_dg_dth_11) == 0
    assert sp.cancel(dg_dth_sym[2, 2] - cpp_dg_dth_22) == 0
    assert sp.cancel(dg_dth_sym[3, 3] - cpp_dg_dth_33) == 0

    print("[Step 6] C++ `KerrMetric.cpp` analytical partial derivatives (dg/dr, dg/dtheta): 100% EXACT MATCH\n")


if __name__ == "__main__":
    verify_schwarzschild_derivation_and_cpp()
    verify_kerr_and_cpp()
    print("All symbolic General Relativity derivations and C++ tensor verifications PASSED.")

