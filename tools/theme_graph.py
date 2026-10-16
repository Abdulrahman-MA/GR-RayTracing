import json
import os
import re
import shutil

COMMUNITY_META = {
    0: {
        "label": "Exact Project Specification & Physics Roadmap",
        "color": "#F59E0B",
        "desc": "General relativistic ray-tracing project blueprint and technological roadmap"
    },
    1: {
        "label": "Phase Space (x^mu, p_mu) & Thin Accretion Disk Physics",
        "color": "#06B6D4",
        "desc": "8-component phase space and equatorial accretion disk crossing"
    },
    2: {
        "label": "Radiative Transfer & Multi-Threaded Pixel Pipeline",
        "color": "#EC4899",
        "desc": "Relativistic intensity transfer (I_nu/nu^3) and multi-threaded scanline rendering"
    },
    3: {
        "label": "Metric Tensors (g_munu, g^munu) & Tetrad Algebra",
        "color": "#8B5CF6",
        "desc": "Spacetime metric tensors, inverse matrices, and analytical coordinate derivatives"
    },
    4: {
        "label": "Schwarzschild & Kerr Spacetime Geometries",
        "color": "#6366F1",
        "desc": "Static Schwarzschild and rotating Boyer-Lindquist Kerr metrics with frame dragging"
    },
    5: {
        "label": "Spacetime 4-Coordinates & Observer Tetrad (e_(a)^mu)",
        "color": "#10B981",
        "desc": "Local orthonormal tetrad satisfying g_munu e_(a)^mu e_(b)^nu = eta_ab"
    },
    6: {
        "label": "Adaptive Dormand-Prince 5(4) Geodesic Stepper",
        "color": "#EAB308",
        "desc": "Embedded 7-stage Runge-Kutta 5(4) with scale-aware local truncation error control"
    },
    7: {
        "label": "Phase-Space Vector Primitives & Dynamic State",
        "color": "#14B8A6",
        "desc": "Flat, cache-aligned stack state vectors for ODE integration"
    },
    8: {
        "label": "Observer Camera & Screen Ray Projection",
        "color": "#3B82F6",
        "desc": "Pinhole camera mapping screen pixels to null coordinate 4-momenta (H = 0)"
    },
    9: {
        "label": "Null Geodesic Ray Tracer & Event Horizon Capture",
        "color": "#EF4444",
        "desc": "Backward ray integration, escape boundaries, and horizon capture cutoff detection"
    },
    10: {
        "label": "System Architecture & GR Layer Contracts",
        "color": "#A855F7",
        "desc": "Layered abstraction contracts across math, metrics, geodesics, and rendering"
    },
    11: {
        "label": "General Relativistic Field Theory & Geodesic Equations",
        "color": "#F97316",
        "desc": "Null curves (ds^2 = 0), conserved Killing quantities (p_t, p_phi), and affine parameter"
    },
    12: {
        "label": "Numerical Geodesic Milestones & Acceptance Criteria",
        "color": "#84CC16",
        "desc": "Quantitative benchmark schedule and scientific acceptance criteria"
    },
    13: {
        "label": "Coordinate Singularities & Physical Horizons",
        "color": "#F43F5E",
        "desc": "Singularity cutoffs r_+ + epsilon preventing Boyer-Lindquist coordinate blowups"
    },
    14: {
        "label": "Empirical Validation & Shadow Analysis Standards",
        "color": "#0EA5E9",
        "desc": "Convergence checks, independent reference models, and shadow measurements"
    },
    15: {
        "label": "Metric Symmetry & Horizon Verification Tests",
        "color": "#D946EF",
        "desc": "Catch2 tests for g_mua g^anu = delta_mu^nu and a->0 Schwarzschild reduction"
    },
    16: {
        "label": "Black Hole Shadow Area & Circularity Residuals",
        "color": "#38BDF8",
        "desc": "Automated boundary extraction, area-equivalent radius R_A, and circularity RMS sigma_R"
    },
    17: {
        "label": "Schwarzschild Geodesic Deflection & Capture Benchmark",
        "color": "#22C55E",
        "desc": "Asymptotic deflection alpha approx 4M/b and critical impact parameter b_crit = 3*sqrt(3)*M"
    },
    18: {
        "label": "Relativistic Engine Development Log",
        "color": "#CA8A04",
        "desc": "Chronological audit of numerical integration and metric implementations"
    },
    19: {
        "label": "Harmonic Oscillator Baseline Verification",
        "color": "#78716C",
        "desc": "SciPy reference solver for 4th-order ODE convergence validation"
    },
    20: {
        "label": "Photon Sphere Radial Equilibrium Benchmark",
        "color": "#FF6D00",
        "desc": "Circular null geodesic equilibrium verification at r = 3M (dp_r/dlambda = 0)"
    },
    21: {
        "label": "Null Ray Outcome & Horizon Capture States",
        "color": "#DC2626",
        "desc": "Discrete ray outcomes: Captured, Escaped, DiskIntersection, NumericalFailure"
    }
}

def clean_comments_from_html(html_text):
    lines = []
    in_block_comment = False
    for line in html_text.splitlines():
        trimmed = line.strip()
        if in_block_comment:
            if "*/" in trimmed:
                in_block_comment = False
            continue
        if trimmed.startswith("/*"):
            if "*/" not in trimmed:
                in_block_comment = True
            continue
        if trimmed.startswith("//"):
            continue
        if trimmed.startswith("<!--") and trimmed.endswith("-->"):
            continue
        lines.append(line)
    return "\n".join(lines)

def apply_theme():
    labels_path = os.path.join("graphify-out", ".graphify_labels.json")
    labels = {str(k): v["label"] for k, v in COMMUNITY_META.items()}
    with open(labels_path, "w", encoding="utf-8") as f:
        json.dump(labels, f, indent=2)

    graph_json_path = os.path.join("graphify-out", "graph.json")
    if os.path.exists(graph_json_path):
        with open(graph_json_path, "r", encoding="utf-8") as f:
            graph_data = json.load(f)

        for node in graph_data.get("nodes", []):
            cid = node.get("community", 0)
            meta = COMMUNITY_META.get(cid, {"label": "General Relativity", "color": "#8B5CF6"})
            node["community_name"] = meta["label"]
            node["color"] = meta["color"]

        with open(graph_json_path, "w", encoding="utf-8") as f:
            json.dump(graph_data, f, indent=2)

    html_path = os.path.join("graphify-out", "graph.html")
    with open(html_path, "r", encoding="utf-8") as f:
        html = f.read()

    script_tag_pattern = r'<script\s+src="https://unpkg\.com/vis-network@[^"]*"\s+integrity="[^"]*"\s+crossorigin="[^"]*"></script>'
    script_tag_replacement = """<script src="vis-network.min.js"></script>
<script>
if (typeof vis === 'undefined') {
  document.write('<script src="https://unpkg.com/vis-network@9.1.6/standalone/umd/vis-network.min.js"><\\/script>');
}
</script>"""
    html = re.sub(script_tag_pattern, script_tag_replacement, html)

    nodes_match = re.search(r"const RAW_NODES = (\[.*?\]);\s*const RAW_EDGES", html)
    edges_match = re.search(r"const RAW_EDGES = (\[.*?\]);\s*const LEGEND", html)
    legend_match = re.search(r"const LEGEND = (\[.*?\]);", html)

    if not (nodes_match and edges_match and legend_match):
        print("Data markers missing")
        return

    nodes = json.loads(nodes_match.group(1))
    edges = json.loads(edges_match.group(1))
    legend = json.loads(legend_match.group(1))

    for node in nodes:
        cid = node.get("community", 0)
        meta = COMMUNITY_META.get(cid, {"label": "General Relativity", "color": "#8B5CF6"})
        node["community_name"] = meta["label"]
        node["color"] = {
            "background": meta["color"],
            "border": "#FFFFFF",
            "highlight": {
                "background": "#FFE082",
                "border": "#00E5FF"
            }
        }
        node["font"] = {"size": 13, "color": "#F8FAFC", "face": "Plus Jakarta Sans"}

    for edge in edges:
        edge["color"] = {
            "color": "rgba(129, 140, 248, 0.35)",
            "highlight": "#00E5FF",
            "hover": "#FFB300",
            "opacity": 0.5
        }
        edge["width"] = 1.5

    for item in legend:
        cid = item.get("cid", 0)
        meta = COMMUNITY_META.get(cid, {"label": item.get("label", ""), "color": "#8B5CF6"})
        item["label"] = meta["label"]
        item["color"] = meta["color"]

    new_nodes_json = json.dumps(nodes)
    new_edges_json = json.dumps(edges)
    new_legend_json = json.dumps(legend)

    html = html[:nodes_match.start(1)] + new_nodes_json + html[nodes_match.end(1):]

    edges_match = re.search(r"const RAW_EDGES = (\[.*?\]);\s*const LEGEND", html)
    html = html[:edges_match.start(1)] + new_edges_json + html[edges_match.end(1):]

    legend_match = re.search(r"const LEGEND = (\[.*?\]);", html)
    html = html[:legend_match.start(1)] + new_legend_json + html[legend_match.end(1):]

    black_hole_css = """
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    background: #020108;
    color: #F1F5F9;
    font-family: 'Plus Jakarta Sans', -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
    display: flex;
    height: 100vh;
    overflow: hidden;
  }
  #graph {
    flex: 1;
    position: relative;
    background: radial-gradient(circle at 50% 50%, #060412 0%, #030209 50%, #000002 100%);
  }
  #sidebar {
    width: 360px;
    background: #06040F;
    border-left: 1px solid rgba(129, 140, 248, 0.25);
    display: flex;
    flex-direction: column;
    overflow: hidden;
    box-shadow: -8px 0 35px rgba(0, 0, 0, 0.85);
  }
  #brand-header {
    padding: 16px;
    border-bottom: 1px solid rgba(129, 140, 248, 0.2);
    display: flex;
    align-items: center;
    justify-content: space-between;
    background: linear-gradient(180deg, #0B081A 0%, #06040F 100%);
  }
  .brand-title {
    font-size: 17px;
    font-weight: 800;
    color: #F8FAFC;
    letter-spacing: 0.05em;
    text-transform: uppercase;
    display: flex;
    align-items: center;
    gap: 8px;
  }
  .brand-sub {
    font-size: 11px;
    color: #94A3B8;
    margin-top: 2px;
    font-weight: 500;
  }
  .brand-badge {
    font-size: 10px;
    font-weight: 700;
    text-transform: uppercase;
    letter-spacing: 0.06em;
    padding: 3px 8px;
    border-radius: 999px;
    background: rgba(99, 102, 241, 0.15);
    color: #A5B4FC;
    border: 1px solid rgba(99, 102, 241, 0.35);
  }
  #physics-hud {
    padding: 12px 16px;
    border-bottom: 1px solid rgba(129, 140, 248, 0.15);
    background: rgba(15, 11, 35, 0.5);
  }
  .hud-title {
    font-size: 10px;
    font-weight: 700;
    text-transform: uppercase;
    letter-spacing: 0.08em;
    color: #818CF8;
    margin-bottom: 8px;
    display: flex;
    justify-content: space-between;
  }
  .hud-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 6px;
  }
  .hud-item {
    font-size: 11px;
    background: rgba(2, 1, 8, 0.7);
    padding: 5px 8px;
    border-radius: 6px;
    border: 1px solid rgba(129, 140, 248, 0.12);
  }
  .hud-sym {
    color: #F59E0B;
    font-weight: 700;
    font-family: monospace;
  }
  .hud-val {
    color: #CBD5E1;
    font-size: 10.5px;
  }
  #search-wrap {
    padding: 12px 16px;
    border-bottom: 1px solid rgba(129, 140, 248, 0.15);
  }
  #search {
    width: 100%;
    background: #0F0C20;
    border: 1px solid rgba(129, 140, 248, 0.25);
    color: #F8FAFC;
    padding: 9px 12px;
    border-radius: 8px;
    font-size: 12px;
    font-family: inherit;
    outline: none;
    transition: all 0.2s ease;
  }
  #search:focus {
    border-color: #00E5FF;
    box-shadow: 0 0 12px rgba(0, 229, 255, 0.25);
  }
  #search-results {
    max-height: 150px;
    overflow-y: auto;
    padding: 6px 16px;
    border-bottom: 1px solid rgba(129, 140, 248, 0.15);
    display: none;
  }
  .search-item {
    padding: 6px 8px;
    cursor: pointer;
    border-radius: 6px;
    font-size: 12px;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
    color: #CBD5E1;
    transition: background 0.15s ease;
  }
  .search-item:hover {
    background: rgba(99, 102, 241, 0.25);
    color: #F8FAFC;
  }
  #info-panel {
    padding: 14px 16px;
    border-bottom: 1px solid rgba(129, 140, 248, 0.15);
    min-height: 130px;
    background: #090616;
  }
  #info-panel h3 {
    font-size: 10.5px;
    color: #818CF8;
    margin-bottom: 8px;
    text-transform: uppercase;
    letter-spacing: 0.08em;
    font-weight: 700;
  }
  #info-content {
    font-size: 12.5px;
    color: #CBD5E1;
    line-height: 1.5;
  }
  #info-content .field {
    margin-bottom: 5px;
  }
  #info-content .field b {
    color: #F8FAFC;
    font-size: 14px;
    font-weight: 700;
    display: block;
    margin-bottom: 2px;
  }
  #info-content .empty {
    color: #64748B;
    font-style: italic;
  }
  .neighbor-link {
    display: block;
    padding: 5px 8px;
    margin: 3px 0;
    border-radius: 6px;
    cursor: pointer;
    font-size: 11.5px;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
    border-left: 3px solid #6366F1;
    background: #0F0C22;
    color: #CBD5E1;
    transition: all 0.15s ease;
  }
  .neighbor-link:hover {
    background: rgba(99, 102, 241, 0.3);
    color: #FFFFFF;
    transform: translateX(2px);
  }
  #neighbors-list {
    max-height: 140px;
    overflow-y: auto;
    margin-top: 6px;
  }
  #legend-wrap {
    flex: 1;
    overflow-y: auto;
    padding: 14px 16px;
  }
  #legend-wrap h3 {
    font-size: 10.5px;
    color: #818CF8;
    margin-bottom: 10px;
    text-transform: uppercase;
    letter-spacing: 0.08em;
    font-weight: 700;
  }
  .legend-item {
    display: flex;
    align-items: center;
    gap: 8px;
    padding: 4px 6px;
    cursor: pointer;
    border-radius: 6px;
    font-size: 11.5px;
    color: #CBD5E1;
    transition: background 0.12s ease;
  }
  .legend-item:hover {
    background: rgba(99, 102, 241, 0.2);
    color: #F8FAFC;
  }
  .legend-item.dimmed {
    opacity: 0.25;
  }
  .legend-dot {
    width: 10px;
    height: 10px;
    border-radius: 50%;
    flex-shrink: 0;
    box-shadow: 0 0 8px currentColor;
  }
  .legend-label {
    flex: 1;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
  }
  .legend-count {
    color: #64748B;
    font-size: 10.5px;
  }
  #stats {
    padding: 10px 16px;
    border-top: 1px solid rgba(129, 140, 248, 0.15);
    font-size: 10.5px;
    color: #64748B;
    background: #05030C;
    display: flex;
    justify-content: space-between;
  }
  #legend-controls {
    display: flex;
    align-items: center;
    gap: 8px;
    margin-bottom: 10px;
  }
  #legend-controls label {
    display: flex;
    align-items: center;
    gap: 8px;
    cursor: pointer;
    font-size: 11.5px;
    color: #94A3B8;
  }
  .legend-cb, #select-all-cb {
    appearance: none;
    -webkit-appearance: none;
    width: 14px;
    height: 14px;
    border: 1.5px solid rgba(129, 140, 248, 0.4);
    border-radius: 4px;
    background: #0B081A;
    cursor: pointer;
    position: relative;
    flex-shrink: 0;
  }
  .legend-cb:checked, #select-all-cb:checked {
    background: #00E5FF;
    border-color: #00E5FF;
  }
  .legend-cb:checked::after, #select-all-cb:checked::after {
    content: '';
    position: absolute;
    left: 4px;
    top: 1.5px;
    width: 3px;
    height: 6px;
    border: solid #020108;
    border-width: 0 2px 2px 0;
    transform: rotate(45deg);
  }
  ::-webkit-scrollbar { width: 5px; }
  ::-webkit-scrollbar-track { background: transparent; }
  ::-webkit-scrollbar-thumb { background: rgba(99, 102, 241, 0.3); border-radius: 4px; }
  ::-webkit-scrollbar-thumb:hover { background: rgba(99, 102, 241, 0.6); }
"""

    html = re.sub(r"<style>.*?</style>", f"<style>{black_hole_css}</style>", html, flags=re.DOTALL)

    sidebar_replacement = """<div id="sidebar">
  <div id="brand-header">
    <div>
      <div class="brand-title">
        <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="#F59E0B" stroke-width="2.5">
          <circle cx="12" cy="12" r="10"/>
          <path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"/>
          <circle cx="12" cy="12" r="3" fill="#000000"/>
        </svg>
        GR-RAYTRACER
      </div>
      <div class="brand-sub">General Relativistic Spacetime Graph</div>
    </div>
    <span class="brand-badge">M=1 · Kerr & Schwarzschild</span>
  </div>
  <div id="physics-hud">
    <div class="hud-title">
      <span>Relativistic Invariants</span>
      <span>G = c = 1</span>
    </div>
    <div class="hud-grid">
      <div class="hud-item"><span class="hud-sym">ds²</span> <span class="hud-val">= 0 (Null Rays)</span></div>
      <div class="hud-item"><span class="hud-sym">H</span> <span class="hud-val">= ½g^μν p_μ p_ν = 0</span></div>
      <div class="hud-item"><span class="hud-sym">b_crit</span> <span class="hud-val">3√3 M ≈ 5.196M</span></div>
      <div class="hud-item"><span class="hud-sym">r_ph</span> <span class="hud-val">3.00 M (Photon Ring)</span></div>
      <div class="hud-item"><span class="hud-sym">r_+</span> <span class="hud-val">M + √(M² - a²)</span></div>
      <div class="hud-item"><span class="hud-sym">E, L</span> <span class="hud-val">-p_t, p_φ (Conserved)</span></div>
    </div>
  </div>
  <div id="search-wrap">
    <input id="search" type="text" placeholder="Search geodesics, metrics, modules..." autocomplete="off">
    <div id="search-results"></div>
  </div>
  <div id="info-panel">
    <h3>Spacetime Entity Telemetry</h3>
    <div id="info-content"><span class="empty">Click any node to inspect relativistic properties</span></div>
  </div>
  <div id="legend-wrap">
    <h3>Astrophysical Community Clusters</h3>
    <div id="legend-controls">
      <label><input type="checkbox" id="select-all-cb" checked onchange="toggleAllCommunities(!this.checked)">Select All Invariants</label>
    </div>
    <div id="legend"></div>
  </div>
  <div id="stats">
    <span>343 nodes · 436 geodesic links</span>
    <span>Signature (-+++)</span>
  </div>
</div>"""

    sidebar_match = re.search(r'<div id="sidebar">.*?</div>\s*<script>', html, flags=re.DOTALL)
    if sidebar_match:
        html = html[:sidebar_match.start()] + sidebar_replacement + "\n<script>" + html[sidebar_match.end():]

    black_hole_canvas_code = """
network.on('beforeDrawing', function(ctx) {
  ctx.save();
  const cx = 0;
  const cy = 0;
  const r_disk_outer = 480;
  const r_disk_inner = 130;
  const r_photon_sphere = 110;
  const r_event_horizon = 85;

  ctx.save();
  ctx.translate(cx, cy);
  ctx.rotate(-0.15);
  ctx.scale(1.0, 0.42);

  const accretionGrad = ctx.createRadialGradient(0, 0, r_disk_inner, 0, 0, r_disk_outer);
  accretionGrad.addColorStop(0.0, 'rgba(255, 240, 180, 0.85)');
  accretionGrad.addColorStop(0.1, 'rgba(0, 229, 255, 0.7)');
  accretionGrad.addColorStop(0.3, 'rgba(245, 158, 11, 0.55)');
  accretionGrad.addColorStop(0.6, 'rgba(239, 68, 68, 0.3)');
  accretionGrad.addColorStop(1.0, 'rgba(2, 1, 8, 0.0)');

  ctx.fillStyle = accretionGrad;
  ctx.beginPath();
  ctx.arc(0, 0, r_disk_outer, 0, Math.PI * 2);
  ctx.fill();

  const beamGrad = ctx.createLinearGradient(-r_disk_outer, 0, r_disk_outer, 0);
  beamGrad.addColorStop(0.0, 'rgba(0, 229, 255, 0.35)');
  beamGrad.addColorStop(0.45, 'rgba(255, 180, 0, 0.25)');
  beamGrad.addColorStop(0.8, 'rgba(220, 38, 38, 0.0)');
  ctx.fillStyle = beamGrad;
  ctx.beginPath();
  ctx.arc(0, 0, r_disk_outer, 0, Math.PI * 2);
  ctx.fill();

  ctx.restore();

  ctx.save();
  ctx.translate(cx, cy);

  ctx.strokeStyle = 'rgba(129, 140, 248, 0.08)';
  ctx.lineWidth = 1;
  for (let r = 80; r <= 600; r += 70) {
    ctx.beginPath();
    ctx.arc(0, 0, r, 0, Math.PI * 2);
    ctx.stroke();
  }
  for (let angle = 0; angle < Math.PI * 2; angle += Math.PI / 6) {
    ctx.beginPath();
    ctx.moveTo(Math.cos(angle) * r_event_horizon, Math.sin(angle) * r_event_horizon);
    ctx.lineTo(Math.cos(angle) * 600, Math.sin(angle) * 600);
    ctx.stroke();
  }

  ctx.shadowColor = '#00E5FF';
  ctx.shadowBlur = 35;
  ctx.strokeStyle = 'rgba(0, 229, 255, 0.45)';
  ctx.lineWidth = 2.5;
  ctx.beginPath();
  ctx.arc(0, 0, r_photon_sphere * 1.25, 0, Math.PI * 2);
  ctx.stroke();

  ctx.shadowColor = '#F59E0B';
  ctx.shadowBlur = 40;
  ctx.strokeStyle = 'rgba(255, 215, 0, 0.95)';
  ctx.lineWidth = 4;
  ctx.beginPath();
  ctx.arc(0, 0, r_photon_sphere, 0, Math.PI * 2);
  ctx.stroke();

  ctx.shadowBlur = 0;
  ctx.fillStyle = '#000000';
  ctx.beginPath();
  ctx.arc(0, 0, r_event_horizon, 0, Math.PI * 2);
  ctx.fill();

  ctx.strokeStyle = 'rgba(139, 92, 246, 0.7)';
  ctx.lineWidth = 1.5;
  ctx.stroke();

  ctx.restore();
  ctx.restore();
});
"""

    if "network.on('beforeDrawing'" not in html:
        network_init_match = re.search(r"const network = new vis\.Network.*?\);", html, flags=re.DOTALL)
        if network_init_match:
            insert_pos = network_init_match.end()
            html = html[:insert_pos] + "\n" + black_hole_canvas_code + "\n" + html[insert_pos:]

    html = clean_comments_from_html(html)

    with open(html_path, "w", encoding="utf-8") as f:
        f.write(html)

    docs_dir = "docs"
    os.makedirs(docs_dir, exist_ok=True)
    doc_graph_path = os.path.join(docs_dir, "spacetime-graph.html")
    with open(doc_graph_path, "w", encoding="utf-8") as f:
        f.write(html)

    src_vis = os.path.join("graphify-out", "vis-network.min.js")
    dst_vis = os.path.join(docs_dir, "vis-network.min.js")
    if os.path.exists(src_vis):
        shutil.copyfile(src_vis, dst_vis)

    tree_path = os.path.join("graphify-out", "GRAPH_TREE.html")
    if os.path.exists(tree_path):
        with open(tree_path, "r", encoding="utf-8") as f:
            tree_html = f.read()
        tree_html = tree_html.replace("background: #f9f9f9;\n      color: #333;", "background: #020108;\n      color: #F1F5F9;")
        tree_html = tree_html.replace("color: #1e3a56;", "color: #F8FAFC;")
        tree_html = tree_html.replace("background: #007bff;", "background: #6366F1;")
        tree_html = tree_html.replace("background: #28a745;", "background: #10B981;")
        tree_html = tree_html.replace("background: #6c757d;", "background: #334155;")
        tree_html = tree_html.replace("stroke: #ccc;", "stroke: rgba(129, 140, 248, 0.3);")
        tree_html = tree_html.replace("fill: #333;", "fill: #F1F5F9;")
        tree_html = clean_comments_from_html(tree_html)
        with open(tree_path, "w", encoding="utf-8") as f:
            f.write(tree_html)

    callflow_path = os.path.join("graphify-out", "gr_raytracing-callflow.html")
    if os.path.exists(callflow_path):
        with open(callflow_path, "r", encoding="utf-8") as f:
            cf_html = f.read()
        cf_html = cf_html.replace("--bg: #0f172a; --surface: #1e293b; --border: #334155;", "--bg: #020108; --surface: #0A071A; --border: rgba(129, 140, 248, 0.25);")
        cf_html = cf_html.replace("--accent: #38bdf8;", "--accent: #00E5FF;")
        cf_html = clean_comments_from_html(cf_html)
        with open(callflow_path, "w", encoding="utf-8") as f:
            f.write(cf_html)

    report_path = os.path.join("graphify-out", "GRAPH_REPORT.md")
    if os.path.exists(report_path):
        with open(report_path, "r", encoding="utf-8") as f:
            rep = f.read()
        for cid, meta in COMMUNITY_META.items():
            rep = rep.replace(f'Community {cid} - "', f'Community {cid} - "{meta["label"]} | ')
        with open(report_path, "w", encoding="utf-8") as f:
            f.write(rep)

    print("Successfully themed all graph visualizations with GR & Black Hole aesthetic")

if __name__ == "__main__":
    apply_theme()
