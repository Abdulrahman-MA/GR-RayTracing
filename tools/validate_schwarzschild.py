import sys
import numpy as np
from scipy.integrate import solve_ivp

def orbit_equations(phi, state, M=1.0):
    u, v = state
    return [v, 3.0 * M * u * u - u]

def compute_deflection(b, M=1.0):
    u_initial = 1e-4
    if b * u_initial >= 1.0:
        return None

    v_initial = np.sqrt(np.maximum(1e-12, (1.0 / (b * b)) - (u_initial * u_initial) * (1.0 - 2.0 * M * u_initial)))
    phi_0 = np.arcsin(b * u_initial)
    phi_span = (phi_0, 4.0 * np.pi)

    def event_escape(phi, state):
        return state[0] - u_initial
    event_escape.terminal = True
    event_escape.direction = -1

    def event_horizon(phi, state):
        return state[0] - (1.0 / (2.0 * M))
    event_horizon.terminal = True
    event_horizon.direction = 1

    sol = solve_ivp(
        lambda phi, y: orbit_equations(phi, y, M),
        phi_span,
        [u_initial, v_initial],
        events=[event_escape, event_horizon],
        rtol=1e-10,
        atol=1e-12,
        max_step=0.01
    )

    if len(sol.t_events[1]) > 0:
        return None

    if len(sol.t_events[0]) > 0:
        phi_final = sol.t_events[0][0]
        delta_phi = phi_final - phi_0
        alpha = delta_phi - np.pi + 2.0 * phi_0
        return alpha

    return None

def run_validation():
    b_values = [50.0, 100.0, 200.0]
    passed = True

    print("Impact parameter sweep vs weak-field analytical 4M/b:")
    for b in b_values:
        measured = compute_deflection(b)
        expected = 4.0 / b
        rel_diff = np.abs(measured - expected) / expected
        print(f"b = {b:6.1f} | Measured: {measured:.5f} rad | Expected: {expected:.5f} rad | Rel Diff: {rel_diff:.2e}")
        if rel_diff > 0.08:
            passed = False

    b_sub = 5.0
    b_super = 5.5
    res_sub = compute_deflection(b_sub)
    res_super = compute_deflection(b_super)

    print(f"b = {b_sub} (sub-critical) -> Captured: {res_sub is None}")
    print(f"b = {b_super} (super-critical) -> Escaped: {res_super is not None}")

    if res_sub is not None or res_super is None:
        passed = False

    if passed:
        print("Schwarzschild light bending validation passed.")
        return 0
    else:
        print("Schwarzschild light bending validation failed.")
        return 1

if __name__ == "__main__":
    sys.exit(run_validation())
