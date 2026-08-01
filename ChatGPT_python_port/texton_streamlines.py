#!/usr/bin/env python3
"""Reproduce Texton's transient streamline point-list output from a .clu file.

This is a clean-room Python port of the relevant execution path in Texton:

    CDlgMoreVtx::OnStreamlinesGo
      -> GetClustersOriginalVelocity
      -> CDlgStreamlines::NoLoadInitialisePointslist
      -> TextureIteration3D / CalcDeltaS3D
      -> VtkUtilities::RenderStreamlineList

The output is a static PNG rather than an interactive VTK window.
"""
from __future__ import annotations

import argparse
import math
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence

import numpy as np


@dataclass(frozen=True)
class ClusterRecord:
    parent: int
    child1: int
    child2: int
    level: int
    is_3d: int
    centroid: tuple[float, float, float]
    error: float
    average_velocity: tuple[float, float, float]
    fit_matrix: tuple[float, ...]
    fit_offset: tuple[float, float, float]


@dataclass(frozen=True)
class CluVolume:
    dimensions: tuple[int, int, int]  # x, y, z
    records: tuple[ClusterRecord, ...]
    leaf_count: int


def load_clu(path: str | Path) -> CluVolume:
    """Read Texton's five-lines-per-node .clu serialization."""
    path = Path(path)
    lines = path.read_text(encoding="ascii").splitlines()
    if not lines:
        raise ValueError(f"Empty .clu file: {path}")

    header = lines[0].split()
    if len(header) != 4:
        raise ValueError("Expected header: <node_count> <size_x> <size_y> <size_z>")
    node_count, size_x, size_y, size_z = map(int, header)

    payload = lines[1:]
    if len(payload) != node_count * 5:
        raise ValueError(
            f"Header says {node_count} nodes, but file contains "
            f"{len(payload) // 5} complete records ({len(payload)} payload lines)."
        )

    records: list[ClusterRecord] = []
    for offset in range(0, len(payload), 5):
        relationship = payload[offset].split()
        descriptor = payload[offset + 1].split()
        average_velocity = payload[offset + 2].split()
        fit_matrix = payload[offset + 3].split()
        fit_offset = payload[offset + 4].split()

        if not (
            len(relationship) == 4
            and len(descriptor) == 5
            and len(average_velocity) == 3
            and len(fit_matrix) == 9
            and len(fit_offset) == 3
        ):
            record_number = offset // 5
            raise ValueError(f"Malformed record {record_number}")

        records.append(
            ClusterRecord(
                parent=int(relationship[0]),
                child1=int(relationship[1]),
                child2=int(relationship[2]),
                level=int(relationship[3]),
                is_3d=int(descriptor[0]),
                centroid=tuple(map(float, descriptor[1:4])),
                error=float(descriptor[4]),
                average_velocity=tuple(map(float, average_velocity)),
                fit_matrix=tuple(map(float, fit_matrix)),
                fit_offset=tuple(map(float, fit_offset)),
            )
        )

    # Texton writes all singleton voxel leaves first, then the hierarchy nodes.
    leaf_count = 0
    for record in records:
        if record.child1 == 0 and record.child2 == 0 and record.level == 0:
            leaf_count += 1
        else:
            break

    if leaf_count == 0:
        raise ValueError("No leading leaf records found")
    # A full binary merge hierarchy has 2L-1 total nodes.
    if node_count != (2 * leaf_count - 1):
        raise ValueError(
            f"Unexpected hierarchy shape: {node_count} nodes for {leaf_count} leaves"
        )

    return CluVolume(
        dimensions=(size_x, size_y, size_z),
        records=tuple(records),
        leaf_count=leaf_count,
    )


def texton_round(value: float) -> int:
    """Exact port of Texton's FloatToInt helper, including its negative-value quirk."""
    fraction, _integer = math.modf(value)
    if value > 0.0:
        if fraction == 0.5:
            truncated = int(value)  # C++ cast truncates toward zero
            return math.ceil(value) if truncated % 2 else math.floor(value)
        return math.ceil(value) if fraction > 0.5 else math.floor(value)

    # In the original C++, modf() returns a negative fractional part for a
    # negative input, but the code compares it with +0.5. Consequently this
    # branch rounds negative non-integers toward zero. Reproducing that quirk
    # changes two seed decisions in this dataset, so it is kept deliberately.
    if fraction == 0.5:
        truncated = int(value)
        return math.floor(value) if truncated % 2 else math.ceil(value)
    return math.floor(value) if fraction > 0.5 else math.ceil(value)


def build_original_velocity(volume: CluVolume) -> tuple[np.ndarray, np.ndarray]:
    """Create the integer velocity lattice used by GetClustersOriginalVelocity."""
    size_x, size_y, size_z = volume.dimensions
    velocity = np.zeros((size_z, size_y, size_x, 3), dtype=np.int16)
    roi_mask = np.zeros((size_z, size_y, size_x), dtype=bool)

    for record in volume.records[: volume.leaf_count]:
        x, y, z = (int(round(v)) for v in record.centroid)
        if not (0 <= x < size_x and 0 <= y < size_y and 0 <= z < size_z):
            raise ValueError(f"Leaf centroid outside volume: {(x, y, z)}")
        velocity[z, y, x] = [texton_round(v) for v in record.average_velocity]
        roi_mask[z, y, x] = True

    return velocity, roi_mask


def texton_seed_points(velocity: np.ndarray) -> list[tuple[int, int, int]]:
    """Generate the same x+=5, y+=5, z+=4 seed lattice used by Texton."""
    size_z, size_y, size_x, _ = velocity.shape
    seeds: list[tuple[int, int, int]] = []
    for x in range(0, size_x, 5):
        for y in range(0, size_y, 5):
            for z in range(0, size_z, 4):
                vx, vy, vz = (int(v) for v in velocity[z, y, x])
                # This is Texton's exact seed test, not magnitude != 0.
                if abs(vx + vy + vz) > 0:
                    seeds.append((x, y, z))
    return seeds


def _next_face_distance(
    cell_x: int,
    cell_y: int,
    cell_z: int,
    px: float,
    py: float,
    pz: float,
    vx: float,
    vy: float,
    vz: float,
    magnitude: float,
) -> float | None:
    """Port of CalcDeltaS3D: reach the nearest forward voxel face + 0.01."""
    candidates: list[float] = []
    for numerator, component in (
        ((cell_y + 1) - py, vy),
        (cell_y - py, vy),
        ((cell_x + 1) - px, vx),
        (cell_x - px, vx),
        ((cell_z + 1) - pz, vz),
        (cell_z - pz, vz),
    ):
        if component != 0.0:
            delta = numerator * (magnitude / component)
            if delta > 0.0 and math.isfinite(delta):
                candidates.append(delta)

    if not candidates:
        return None
    return min(candidates) + 0.01


def trace_forward(
    seed: tuple[int, int, int],
    velocity: np.ndarray,
    maximum_steps: int = 100,
) -> np.ndarray:
    """Trace the positive side that Texton appends to its streamline point list."""
    size_z, size_y, size_x, _ = velocity.shape
    px, py, pz = (seed[0] + 0.5, seed[1] + 0.5, seed[2] + 0.5)
    points: list[tuple[float, float, float]] = []

    for _ in range(maximum_steps):
        cell_x, cell_y, cell_z = math.floor(px), math.floor(py), math.floor(pz)
        vx, vy, vz = (float(v) for v in velocity[cell_z, cell_y, cell_x])
        magnitude = math.sqrt(vx * vx + vy * vy + vz * vz)
        if magnitude == 0.0:
            # Texton's loop consumes an iteration without moving.
            continue

        delta_s = _next_face_distance(
            cell_x, cell_y, cell_z, px, py, pz, vx, vy, vz, magnitude
        )
        if delta_s is None:
            continue

        nx = px + (vx / magnitude) * delta_s
        ny = py + (vy / magnitude) * delta_s
        nz = pz + (vz / magnitude) * delta_s

        # Texton clamps to the full volume before its rounded boundary test.
        nx = min(max(nx, 0.0), float(size_x - 1))
        ny = min(max(ny, 0.0), float(size_y - 1))
        nz = min(max(nz, 0.0), float(size_z - 1))

        rounded_x, rounded_y, rounded_z = map(texton_round, (nx, ny, nz))
        if (
            rounded_x >= size_x - 1
            or rounded_y >= size_y - 1
            or rounded_z >= size_z - 1
            or rounded_x <= 0
            or rounded_y <= 0
            or rounded_z <= 0
        ):
            break

        points.append((nx, ny, nz))
        px, py, pz = nx, ny, nz

    return np.asarray(points, dtype=np.float64).reshape((-1, 3))


def calculate_streamlines(
    velocity: np.ndarray,
    maximum_steps: int = 100,
) -> tuple[list[tuple[int, int, int]], list[np.ndarray]]:
    seeds = texton_seed_points(velocity)
    streamlines = [trace_forward(seed, velocity, maximum_steps) for seed in seeds]
    return seeds, streamlines


def _set_axes_equal(ax, dimensions: tuple[int, int, int]) -> None:
    size_x, size_y, size_z = dimensions
    centers = np.array([(size_x - 1) / 2, (size_y - 1) / 2, (size_z - 1) / 2])
    radius = max(size_x - 1, size_y - 1, size_z - 1) / 2
    ax.set_xlim(centers[0] - radius, centers[0] + radius)
    ax.set_ylim(centers[1] - radius, centers[1] + radius)
    ax.set_zlim(centers[2] - radius, centers[2] + radius)
    ax.set_box_aspect((1, 1, 1))


def render_static(
    streamlines: Sequence[np.ndarray],
    roi_mask: np.ndarray,
    output_path: str | Path,
    *,
    elevation: float = 8.0,
    azimuth: float = -150.0,
    roll: float = 0.0,
    show_roi_shell: bool = True,
    dpi: int = 220,
) -> None:
    """Render the point lists as a static orthographic PNG."""
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    fig = plt.figure(figsize=(8, 8), facecolor="white")
    ax = fig.add_subplot(111, projection="3d", computed_zorder=False)
    ax.set_facecolor("white")
    ax.set_proj_type("ortho")

    if show_roi_shell:
        try:
            from scipy.ndimage import gaussian_filter
            from skimage.measure import marching_cubes

            # A faint anatomical context surface, derived solely from the ROI leaves.
            smooth = gaussian_filter(roi_mask.astype(np.float32), sigma=1.15)
            verts_zyx, faces, _normals, _values = marching_cubes(smooth, level=0.13)
            z, y, x = verts_zyx.T
            ax.plot_trisurf(
                x,
                y,
                z,
                triangles=faces,
                color=(0.80, 0.80, 0.80),
                alpha=0.055,
                linewidth=0,
                antialiased=False,
                shade=False,
                zorder=0,
            )
        except (ImportError, ValueError):
            pass

    # VtkUtilities::RenderStreamlineList ignores lists with <= 2 points, creates
    # vtkPolyLine objects, then wraps them in vtkTubeFilter(radius=.2, sides=12).
    drawable = [line for line in streamlines if len(line) > 2]
    # Draw long paths first so short central structures remain legible.
    for line in sorted(drawable, key=len, reverse=True):
        ax.plot(
            line[:, 0],
            line[:, 1],
            line[:, 2],
            color=(0.24, 0.24, 0.24),
            linewidth=1.15,
            alpha=0.70,
            solid_capstyle="round",
            zorder=5,
        )

    dimensions = (roi_mask.shape[2], roi_mask.shape[1], roi_mask.shape[0])
    _set_axes_equal(ax, dimensions)
    ax.view_init(elev=elevation, azim=azimuth, roll=roll)
    ax.set_axis_off()
    fig.subplots_adjust(left=0, right=1, bottom=0, top=1)
    output_path = Path(output_path)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=dpi, facecolor="white", bbox_inches="tight", pad_inches=0.02)
    plt.close(fig)


def build_argument_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Port Texton's transient streamline point-list algorithm to Python."
    )
    parser.add_argument("clu_file", type=Path, help="Texton .clu input file")
    parser.add_argument("-o", "--output", type=Path, default=Path("texton_streamlines.png"))
    parser.add_argument("--steps", type=int, default=100, help="Texton's RangeAlpha")
    parser.add_argument("--elevation", type=float, default=8.0)
    parser.add_argument("--azimuth", type=float, default=-150.0)
    parser.add_argument("--roll", type=float, default=0.0)
    parser.add_argument("--no-shell", action="store_true", help="Omit the faint ROI context surface")
    parser.add_argument("--dpi", type=int, default=220)
    return parser


def main(argv: Iterable[str] | None = None) -> int:
    args = build_argument_parser().parse_args(argv)
    started = time.perf_counter()
    volume = load_clu(args.clu_file)
    velocity, roi_mask = build_original_velocity(volume)
    integration_started = time.perf_counter()
    seeds, streamlines = calculate_streamlines(velocity, maximum_steps=args.steps)
    integration_seconds = time.perf_counter() - integration_started
    render_static(
        streamlines,
        roi_mask,
        args.output,
        elevation=args.elevation,
        azimuth=args.azimuth,
        roll=args.roll,
        show_roi_shell=not args.no_shell,
        dpi=args.dpi,
    )
    total_seconds = time.perf_counter() - started

    drawable = [line for line in streamlines if len(line) > 2]
    point_count = sum(len(line) for line in drawable)
    print(f"Input:              {args.clu_file}")
    print(f"Dimensions:         {volume.dimensions[0]} x {volume.dimensions[1]} x {volume.dimensions[2]}")
    print(f"ROI leaf voxels:    {volume.leaf_count:,}")
    print(f"Hierarchy nodes:    {len(volume.records):,}")
    print(f"Seed points:        {len(seeds):,}")
    print(f"Rendered paths:     {len(drawable):,}")
    print(f"Rendered points:    {point_count:,}")
    print(f"Integration time:   {integration_seconds:.6f} s")
    print(f"Total time:         {total_seconds:.3f} s")
    print(f"Output:             {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
