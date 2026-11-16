"""Geometry checks on case v2: printable, closed, aligned, and the connectors line up.

Regenerates the model from hardware/case/uno_case_v2.py into a temp dir and checks it.
Run: python3 tests/test_case_geometry.py   (needs: pip install -r requirements-dev.txt)
"""
import os, sys, tempfile, importlib.util, contextlib, io, pathlib
import numpy as np
import trimesh

ROOT = pathlib.Path(__file__).resolve().parent.parent
SCRIPT = ROOT / "hardware/case/uno_case_v2.py"

def load():
    spec = importlib.util.spec_from_file_location("case_v2", SCRIPT)
    mod = importlib.util.module_from_spec(spec)
    with contextlib.redirect_stdout(io.StringIO()):
        spec.loader.exec_module(mod)   # __main__ guard keeps it from writing files
    return mod

M = load()

def mesh(man):
    m = man.to_mesh()
    return trimesh.Trimesh(np.array(m.vert_properties)[:, :3], np.array(m.tri_verts))

def test_parts_watertight():              # NFR-002
    for name in ("base", "lid", "plunger", "usb_plug", "jack_plug", "slot_plug"):
        assert mesh(getattr(M, name)).is_watertight, name

def test_fits_printer_bed():              # NFR-002: AD5M Pro is 220 x 220 x 220
    for name in ("base", "lid"):
        assert max(mesh(getattr(M, name)).extents) < 215, name

def test_usb_cutout_centered_on_port():   # FR-008 (v1 was 6 mm off)
    assert abs(M.USB_CUT[0] - 38.4) < 0.5

def test_lid_does_not_collide_with_base():   # FR-008
    from manifold3d import Manifold
    lid = M.lid.mirror([0, 0, 1]).translate([0, 0, M.out_H + M.LID_T])          # flip onto the rim
    lid = lid.mirror([0, 1, 0]).translate([0, 2 * M.cyc, 0])                    # undo the Y mirror the flip implies
    assert (M.base ^ lid).volume() < 1e-3

def solid_at(man, x, y, z, e=0.2):
    """True if the part has material at (x, y, z), probed with a tiny cube (no rtree needed)."""
    from manifold3d import Manifold
    probe = Manifold.cube([e, e, e]).translate([x - e / 2, y - e / 2, z - e / 2])
    return (man ^ probe).volume() > 0.5 * e ** 3

def test_lid_holes_align_with_bosses():   # FR-008
    # With the lid flipped onto the base, the screw axis at each boss must be open
    # through the lid, open down the insert hole, and the boss must be solid around it.
    lid = (M.lid.mirror([0, 0, 1]).translate([0, 0, M.out_H + M.LID_T])
           .mirror([0, 1, 0]).translate([0, 2 * M.cyc, 0]))
    for (cx, cy) in M.corner_c:
        assert not solid_at(lid, cx, cy, M.out_H + M.LID_T / 2), f"lid blocks screw at {cx, cy}"
        assert not solid_at(M.base, cx, cy, M.out_H - 2), f"no insert hole at {cx, cy}"
        r = M.M3_INSERT_HOLE / 2 + 0.5
        assert any(solid_at(M.base, cx + dx, cy + dy, M.out_H - 2)
                   for dx, dy in ((r, 0), (-r, 0), (0, r), (0, -r))), f"no boss wall at {cx, cy}"

def test_screw_lengths_fit_insert_holes():   # NFR-003
    # M3x8 flat head through the lid into the boss hole must not bottom out
    assert 8 - M.LID_T < M.LID_INSERT_L + M.EXTRA_DEPTH
    # M3x6 / M2x6 through a 1.6 mm PCB into the standoff hole
    for sz in ("M3", "M2"):
        assert 6 - M.PCB_T < M.UNO_INSERT_L[sz] + M.EXTRA_DEPTH
    # M2x6 through the matrix PCB into the post
    assert 6 - M.PCB_T < M.MX_INSERT_L + M.MX_EXTRA_DEPTH

if __name__ == "__main__":
    fails = 0
    for name, fn in list(globals().items()):
        if name.startswith("test_"):
            try:
                fn(); print(f"PASS  {name}")
            except AssertionError as e:
                fails += 1; print(f"FAIL  {name}: {e}")
    sys.exit(1 if fails else 0)
