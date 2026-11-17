"""Geometry checks on case v3 (no inserts: pilot holes for the Uno, press-fit lid).

Regenerates the model from hardware/case/uno_case_v3.py in memory and checks it.
Run: python3 tests/test_case_v3_geometry.py   (needs: pip install -r requirements-dev.txt)
"""
import sys, importlib.util, contextlib, io, pathlib
import numpy as np
import trimesh
from manifold3d import Manifold

ROOT = pathlib.Path(__file__).resolve().parent.parent
SCRIPT = ROOT / "hardware/case/uno_case_v3.py"

def load():
    spec = importlib.util.spec_from_file_location("case_v3", SCRIPT)
    mod = importlib.util.module_from_spec(spec)
    with contextlib.redirect_stdout(io.StringIO()):
        spec.loader.exec_module(mod)   # __main__ guard keeps it from writing files
    return mod

M = load()

def mesh(man):
    m = man.to_mesh()
    return trimesh.Trimesh(np.array(m.vert_properties)[:, :3], np.array(m.tri_verts))

def solid_at(man, x, y, z, e=0.2):
    """True if the part has material at (x, y, z), probed with a tiny cube."""
    probe = Manifold.cube([e, e, e]).translate([x - e / 2, y - e / 2, z - e / 2])
    return (man ^ probe).volume() > 0.5 * e ** 3

def lid_on_base():
    """The lid flipped and seated on the rim, in base coordinates."""
    return (M.lid.mirror([0, 0, 1]).translate([0, 0, M.out_H + M.LID_T])
            .mirror([0, 1, 0]).translate([0, 2 * M.cyc, 0]))

def test_v3_parts_watertight():              # NFR-002
    for name in ("base", "lid", "plunger", "usb_plug", "jack_plug", "slot_plug"):
        assert mesh(getattr(M, name)).is_watertight, name

def test_v3_fits_printer_bed():              # NFR-002: AD5M Pro is 220 x 220 x 220
    for name in ("base", "lid"):
        assert max(mesh(getattr(M, name)).extents) < 215, name

def test_v3_cutouts_match_v2_plugs():        # FR-008: the v2 plugs are reused
    assert M.USB_CUT == (38.4, 16.0, 13.5) and M.JACK_CUT == (7.6, 12.0, 13.0)

def test_v3_no_inserts_or_bosses():          # FR-008: nothing at the rim corners but wall
    for name in ("corner_c", "BOSS", "M3_INSERT_HOLE", "M2_INSERT_HOLE"):
        assert not hasattr(M, name), name
    # a point just inside each cavity corner, near the rim, must be open
    for x in (M.cav_x0 + 2, M.cav_x1 - 2):
        for y in (M.cav_y0 + 2, M.cav_y1 - 2):
            assert not solid_at(M.base, x, y, M.out_H - 3), f"material at rim corner {x, y}"

def test_v3_plug_top_clearance_covers_bridge_sag():   # FR-008: v2 barrel plug needed trimming
    assert M.PLUG_CLEAR_TOP >= 0.5

def test_v3_pilot_holes_bite():              # NFR-003: hole smaller than the screw's thread
    assert M.PILOT["M3"] < 3.0 and M.PILOT["M2"] < 2.0
    assert M.PILOT["M3"] > 2.2 and M.PILOT["M2"] > 1.4      # but big enough to drive

def test_v3_pilot_holes_open_and_blind():    # NFR-003
    top = M.FLOOR + M.STANDOFF_H
    for (hx, hy) in M.HOLES:
        assert not solid_at(M.base, hx, hy, top - 0.5), f"pilot not open at {hx, hy}"
        assert not solid_at(M.base, hx, hy, top - 4.4), f"pilot too shallow at {hx, hy}"
        assert solid_at(M.base, hx, hy, 0.3), f"pilot breaks through the bottom at {hx, hy}"
        r = M.PILOT["M3"] / 2 + 0.6
        assert all(solid_at(M.base, hx + dx, hy + dy, top - 2)
                   for dx, dy in ((r, 0), (-r, 0), (0, r), (0, -r))), f"thin standoff at {hx, hy}"

def test_v3_lid_press_fit():                 # FR-008: ribs interfere a little, the lip itself clears
    interference = M.LIP_RIB - M.LID_FIT
    assert 0.1 <= interference <= 0.3, interference
    assert M.LID_FIT > 0

def test_v3_lid_does_not_collide_except_ribs():   # FR-008
    lid = lid_on_base()
    overlap = (M.base ^ lid).volume()
    # Only the crush ribs may overlap the wall: 10 ribs x width x interference x rib height
    rib_budget = 10 * M.LIP_RIB_W * (M.LIP_RIB - M.LID_FIT) * (M.LIP_H - 1.0)
    assert overlap <= rib_budget * 1.05, (overlap, rib_budget)

def test_v3_lid_clears_slot_plug():          # FR-008: lip is cut at the wire slot
    lid = lid_on_base()
    x = M.cav_x1 - M.LIP_T / 2
    assert not solid_at(lid, x, M.PCB_W / 2, M.out_H - 2), "lid lip blocks the wire slot"

if __name__ == "__main__":
    fails = 0
    for name, fn in list(globals().items()):
        if name.startswith("test_"):
            try:
                fn(); print(f"PASS  {name}")
            except AssertionError as e:
                fails += 1; print(f"FAIL  {name}: {e}")
    sys.exit(1 if fails else 0)
