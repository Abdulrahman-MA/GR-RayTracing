import sys
import numpy as np
from scipy.integrate import solve_ivp

def harmonic_oscillator(t, y):
    return [y[1], -y[0]]

def run_validation():
    t_span = (0.0, 10.0)
    y0 = [1.0, 0.0]
    t_eval = np.linspace(0.0, 10.0, 500)

    sol = solve_ivp(
        harmonic_oscillator,
        t_span,
        y0,
        method="RK45",
        t_eval=t_eval,
        rtol=1e-10,
        atol=1e-10
    )

    exact_x = np.cos(t_eval)
    exact_v = -np.sin(t_eval)

    max_err_x = np.max(np.abs(sol.y[0] - exact_x))
    max_err_v = np.max(np.abs(sol.y[1] - exact_v))

    print(f"SciPy RK45 Max Position Error: {max_err_x:.3e}")
    print(f"SciPy RK45 Max Velocity Error: {max_err_v:.3e}")

    if max_err_x < 1e-6 and max_err_v < 1e-6:
        print("ODE validation passed.")
        return 0
    else:
        print("ODE validation failed.")
        return 1

if __name__ == "__main__":
    sys.exit(run_validation())
