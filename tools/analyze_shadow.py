import sys
import numpy as np

def parse_ppm(file_path):
    with open(file_path, "rb") as f:
        header = f.readline().decode().strip()
        if header != "P6":
            raise ValueError(f"Unsupported PPM format: {header}")

        line = f.readline().decode().strip()
        while line.startswith("#"):
            line = f.readline().decode().strip()

        dims = line.split()
        while len(dims) < 2:
            dims.extend(f.readline().decode().strip().split())
        width, height = int(dims[0]), int(dims[1])

        max_val = int(f.readline().decode().strip())
        pixel_data = np.frombuffer(f.read(), dtype=np.uint8).reshape((height, width, 3))
        return pixel_data

def extract_shadow_metrics(pixel_data):
    gray = np.mean(pixel_data, axis=2)
    shadow_mask = gray < 10

    area_pixels = np.sum(shadow_mask)
    area_eq_radius = np.sqrt(area_pixels / np.pi)

    diff_y = np.diff(shadow_mask.astype(int), axis=0)
    diff_x = np.diff(shadow_mask.astype(int), axis=1)

    y_edges, x_edges = np.where((diff_y[:, :-1] != 0) | (diff_x[:-1, :] != 0))

    if len(x_edges) == 0:
        return area_eq_radius, 0.0

    center_x = np.mean(x_edges)
    center_y = np.mean(y_edges)

    radii = np.sqrt((x_edges - center_x) ** 2 + (y_edges - center_y) ** 2)
    mean_radius = np.mean(radii)
    rms_residual = np.sqrt(np.mean((radii - mean_radius) ** 2)) / (mean_radius + 1e-12)

    return area_eq_radius, rms_residual

def main():
    if len(sys.argv) < 2:
        print("Usage: python analyze_shadow.py <image.ppm>")
        return 1

    img_path = sys.argv[1]
    pixels = parse_ppm(img_path)
    r_a, rms = extract_shadow_metrics(pixels)

    print(f"Shadow Analysis for {img_path}:")
    print(f"Area-equivalent radius (pixels): {r_a:.3f}")
    print(f"Circularity residual RMS: {rms:.4e}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
