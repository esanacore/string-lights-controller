"""
Uno R3 + proto shield enclosure for the string light controller, v2.

Changes from v2 (see docs/case.md):
  - No heat-set inserts. On the v2 print the four corner-boss insert holes filled with oozed
    PETG, so the inserts went in off-center. v3 drops inserts entirely:
      * lid: press-fit. No corner bosses, no screws; a full-perimeter lip with crush ribs
        friction-fits inside the rim.
      * Uno: screws thread straight into printed pilot holes in the standoffs:
        M3 self-tapping (Phillips pan head) at the two far holes, M2x6 socket heads at the two
        crowded holes next to the barrel jack (an M3 head doesn't fit there).
      * matrix: M2x6 socket heads thread into pilot holes in two posts under the lid.
  - Removing the corner bosses also removes the four tiny islands at the rim that strung badly.
  - Connector cutouts unchanged (v2 plugs still fit); v3 plugs get 0.5 mm top clearance for bridge sag.

Prints on the Adventurer 5M Pro (0.6 mm nozzle, PETG), all without supports:
  base open side up, lid face down, plunger on its collar, plugs flange down.

Board coordinates follow the standard Uno R3 drawing:
origin = PCB lower-left corner, USB-B and barrel jack on the x=0 edge.
All dimensions in mm.  python uno_case_v3.py  -> STLs in ./stl_v3/
"""

import os
from manifold3d import Manifold
import trimesh
import numpy as np

# ---------------- PARAMETERS ----------------
WALL = 2.4            # 4 perimeters at 0.6 mm
FLOOR = 2.0
PCB_L, PCB_W, PCB_T = 68.6, 53.3, 1.6
STANDOFF_H = 5.0      # floor to bottom of Uno PCB
STACK_H = 38.0        # Uno PCB top to lid underside region (shield + matrix module)
TOP_GAP = 3.0
USB_OVERHANG = 6.4
CLEAR = 1.0

HOLES = [(14.0, 2.5), (15.3, 50.7), (66.1, 7.6), (66.1, 35.5)]   # Uno R3 mounting holes
# Holes 1-2 sit next to the barrel jack and the 10-pin header: an M3 head (5.5 mm) doesn't fit
# and a wide standoff lands on solder tails, so those two use M2 (3.8 mm head) on 6 mm standoffs.
HOLE_SIZE = ["M2", "M2", "M3", "M3"]

# Connector cutouts on the x=0 wall: (center_y, width, height above PCB top)
USB_CUT = (38.4, 16.0, 13.5)     # v1 used 44.4 (that's the connector's edge, not its center)
JACK_CUT = (7.6, 12.0, 13.0)     # fit confirmed in v1

# Pilot holes for screws threaded straight into PETG (0.6 nozzle prints holes ~0.2 small)
PILOT = {"M3": 2.7, "M2": 1.8}   # prints ~2.5 (M3 self-tapper) / ~1.6 (M2 machine screw)
PILOT_DEPTH = STANDOFF_H + FLOOR - 0.8   # into the floor, 0.8 mm of floor left under the hole
STANDOFF_D = {"M3": 7.0, "M2": 6.0}
MX_PILOT_DEPTH = 5.0             # M2x6 through a 1.6 mm PCB reaches 4.4 mm into the post

# Press-fit lid
LID_T = 3.0
LIP_H = 5.0
LIP_T = 1.6
LID_FIT = 0.15                   # per side; the crush ribs take up the rest
LIP_RIB = 0.35                   # crush-rib height on the lip's outer faces
LIP_RIB_W = 1.0

WIRE_HOLE_D = 6.0

# S1 plunger (v1 tube fit after a quick drill; widen + chamfer the entry)
S1_X, S1_Y = 25.5, 3.75
S1_TOP = 19.6
PLUNGER_D = 6.0
TUBE_ID, TUBE_OD = 6.8, 9.0
TUBE_ENTRY_CHAMFER = 0.6
TUBE_BOTTOM = 28.0

# 8x8 matrix (HW-109): LED block in a pocket behind the window, board screwed to 2 posts.
MX_BLOCK = 32.6
MX_WINDOW = 31.0
MX_X0, MX_Y0 = 10.0, 17.0        # pocket corner (board coords); board/header end toward +x
MX_FRAME_H = 4.0
MX_LIP = 0.8
# Measure these on the module before printing the lid (see docs/case.md):
MX_FACE_TO_PCB = 10.0            # LED face to the top surface of the green PCB
MX_HOLE_FROM_BLOCK = 14.5        # LED block edge (header side) to the mounting hole centers
MX_HOLE_SPACING = 26.0           # between the two mounting holes (centered on the block)
MX_POST_D = 6.6

PLUG_CLEAR_W = 0.1               # per side across the width
PLUG_CLEAR_TOP = 0.5             # at the bridged top edge (v2: 0.3, barrel plug needed the sag trimmed)
PLUG_CHAMFER = 0.6
RIB = 0.3                        # crush-rib height
SLOT_LIP_CUT = 10.0              # lid lip cut-out over the wire slot
PLUG_FLANGE = 1.5
PLUG_FLANGE_T = 1.2
# --------------------------------------------

def box(x, y, z, sx, sy, sz):
    return Manifold.cube([sx, sy, sz]).translate([x, y, z])

def cyl(x, y, z, d, h, seg=48, d2=None):
    return Manifold.cylinder(h, d / 2, (d2 if d2 is not None else d) / 2, seg).translate([x, y, z])

cav_x0 = -USB_OVERHANG - 0.4
cav_x1 = PCB_L + CLEAR
cav_y0 = -CLEAR
cav_y1 = PCB_W + CLEAR
cav_h = STANDOFF_H + PCB_T + STACK_H + TOP_GAP
out_x0, out_y0 = cav_x0 - WALL, cav_y0 - WALL
out_L = (cav_x1 - cav_x0) + 2 * WALL
out_W = (cav_y1 - cav_y0) + 2 * WALL
out_H = FLOOR + cav_h
pcb_top = FLOOR + STANDOFF_H + PCB_T

# ---------------- base ----------------
base = box(out_x0, out_y0, 0, out_L, out_W, out_H)
base -= box(cav_x0, cav_y0, FLOOR, cav_x1 - cav_x0, cav_y1 - cav_y0, cav_h + 1)

# Uno standoffs with pilot holes (screws thread straight into the plastic)
for (hx, hy), sz in zip(HOLES, HOLE_SIZE):
    base += cyl(hx, hy, FLOOR - 0.01, STANDOFF_D[sz], STANDOFF_H + 0.01)
    base -= cyl(hx, hy, FLOOR + STANDOFF_H - PILOT_DEPTH, PILOT[sz], PILOT_DEPTH + 1)

# Connector cutouts
CUT_Z0 = pcb_top - 1.5
for (cy, w, h) in (USB_CUT, JACK_CUT):
    base -= box(out_x0 - 1, cy - w / 2, CUT_Z0, WALL + 2, w, h + 1.5)

# String lead slot (open top), same as v1
wire_z = pcb_top + 14
base -= Manifold.cylinder(WALL + 2, WIRE_HOLE_D / 2, WIRE_HOLE_D / 2, 48) \
    .rotate([0, 90, 0]).translate([cav_x1 - 1, PCB_W / 2, wire_z])
base -= box(cav_x1 - 1, PCB_W / 2 - WIRE_HOLE_D / 2, wire_z, WALL + 2, WIRE_HOLE_D, out_H)

# ---------------- lid ----------------
# Lid coords: z = 0 is the lid's outer face (printed face down), +z goes into the box.
# The lid flips onto the box, which mirrors Y; mirror every lid feature's Y.
cyc = (cav_y0 + cav_y1) / 2
def my(y):
    return 2 * cyc - y

lid = box(out_x0, out_y0, 0, out_L, out_W, LID_T)

# press-fit lip, cut away only at the wire slot, with vertical crush ribs on its outer faces
lip_x0, lip_y0 = cav_x0 + LID_FIT, cav_y0 + LID_FIT
lip_L = (cav_x1 - cav_x0) - 2 * LID_FIT
lip_W = (cav_y1 - cav_y0) - 2 * LID_FIT
lip = box(lip_x0, lip_y0, LID_T - 0.01, lip_L, lip_W, LIP_H)
lip -= box(lip_x0 + LIP_T, lip_y0 + LIP_T, LID_T - 1, lip_L - 2 * LIP_T, lip_W - 2 * LIP_T, LIP_H + 2)
rib_h = LIP_H - 1.0                    # ribs stop 1 mm short of the lip tip so it starts easily
for fx in (0.2, 0.5, 0.8):             # long sides
    x = lip_x0 + fx * lip_L - LIP_RIB_W / 2
    lip += box(x, lip_y0 - LIP_RIB, LID_T, LIP_RIB_W, LIP_RIB + 0.01, rib_h)
    lip += box(x, lip_y0 + lip_W - 0.01, LID_T, LIP_RIB_W, LIP_RIB + 0.01, rib_h)
for fy in (0.25, 0.75):                # short sides (clear of the wire slot at the middle)
    y = lip_y0 + fy * lip_W - LIP_RIB_W / 2
    lip += box(lip_x0 - LIP_RIB, y, LID_T, LIP_RIB + 0.01, LIP_RIB_W, rib_h)
    lip += box(lip_x0 + lip_L - 0.01, y, LID_T, LIP_RIB + 0.01, LIP_RIB_W, rib_h)
lip -= box(cav_x1 - LIP_T - 1, my(PCB_W / 2) - SLOT_LIP_CUT / 2, LID_T - 1, LIP_T + 2, SLOT_LIP_CUT, LIP_H + 2)
lid += lip

# pry notch on the outer face's edge at the USB end, for a fingernail
lid -= box(out_x0 - 0.01, my(PCB_W / 2) - 5, -0.01, 1.2, 10, 1.0)

lid_top_above_pcb = STACK_H + TOP_GAP + LID_T

# S1 plunger guide tube, wider bore + chamfered entry at the outer face
tube_len = lid_top_above_pcb - TUBE_BOTTOM - LID_T
lid += cyl(S1_X, my(S1_Y), LID_T - 0.01, TUBE_OD, tube_len + 0.01)
lid -= cyl(S1_X, my(S1_Y), -1, TUBE_ID, tube_len + LID_T + 2)
lid -= cyl(S1_X, my(S1_Y), -0.01, TUBE_ID + 2 * TUBE_ENTRY_CHAMFER, TUBE_ENTRY_CHAMFER, d2=TUBE_ID)

# matrix frame + window (fit confirmed in v1)
f = 1.6
MX_Y0m = my(MX_Y0 + MX_BLOCK)
lid += box(MX_X0 - f, MX_Y0m - f, LID_T - 0.01, MX_BLOCK + 2 * f, MX_BLOCK + 2 * f, MX_FRAME_H + 0.01)
lid -= box(MX_X0, MX_Y0m, LID_T - MX_LIP, MX_BLOCK, MX_BLOCK, MX_FRAME_H + 2)
lid -= box(MX_X0 + (MX_BLOCK - MX_WINDOW) / 2, MX_Y0m + (MX_BLOCK - MX_WINDOW) / 2, -1,
           MX_WINDOW, MX_WINDOW, LID_T + 2)

# matrix posts with M2 pilot holes: the board's top face sits on the post ends.
# Posts end 0.3 mm short so tightening the screws seats the LED face against the window lip.
face_z = LID_T - MX_LIP
post_end = face_z + MX_FACE_TO_PCB - 0.3
post_len = post_end - LID_T
hx = MX_X0 + MX_BLOCK + MX_HOLE_FROM_BLOCK
mx_posts = []
for hy in (MX_Y0 + MX_BLOCK / 2 - MX_HOLE_SPACING / 2, MX_Y0 + MX_BLOCK / 2 + MX_HOLE_SPACING / 2):
    mx_posts.append((hx, hy))
    lid += cyl(hx, my(hy), LID_T - 0.01, MX_POST_D, post_len + 0.01)
    lid -= cyl(hx, my(hy), post_end - MX_PILOT_DEPTH, PILOT["M2"], MX_PILOT_DEPTH + 1)

# vents over the USB end (clear of the matrix frame)
for i in range(3):
    x = cav_x0 + LIP_T + 1.5 + i * 4.0
    lid -= box(x, my(36), -1, 2.0, 26, LID_T + 2)

# ---------------- plunger (unchanged from v1) ----------------
rod_len = lid_top_above_pcb + 3.0 - S1_TOP
plunger = cyl(0, 0, 0, PLUNGER_D, rod_len) + cyl(0, 0, 0, 8.8, 1.6)

# ---------------- plugs ----------------
def chamfered_box(x, y, z, sx, sy, sz, c):
    """Box with its 4 vertical edges chamfered by c (eases press fits, beats elephant's foot)."""
    from manifold3d import CrossSection
    pts = [(c, 0), (sx - c, 0), (sx, c), (sx, sy - c), (sx - c, sy), (c, sy), (0, sy - c), (0, c)]
    return Manifold.extrude(CrossSection([pts]), sz).translate([x, y, z])

def rect_plug(w, h):
    """Press-fit plug for a rectangular wall cutout (w across, h tall). Printed flange down.
    Plug y axis = cutout height; y=0 is the cutout's bottom edge."""
    bw, bh = w - 2 * PLUG_CLEAR_W, h - PLUG_CLEAR_W - PLUG_CLEAR_TOP
    fl = box(0, 0, 0, w + 2 * PLUG_FLANGE, h + 2 * PLUG_FLANGE, PLUG_FLANGE_T)
    bx, by = PLUG_FLANGE + PLUG_CLEAR_W, PLUG_FLANGE + PLUG_CLEAR_W
    body = chamfered_box(bx, by, PLUG_FLANGE_T - 0.01, bw, bh, WALL + 0.01, PLUG_CHAMFER)
    # two crush ribs on each long side (they deform on insertion)
    for fx in (0.3, 0.7):
        y = by + fx * bh
        body += box(bx - RIB, y - 0.4, PLUG_FLANGE_T, RIB + 0.01, 0.8, WALL * 0.8)
        body += box(bx + bw - 0.01, y - 0.4, PLUG_FLANGE_T, RIB + 0.01, 0.8, WALL * 0.8)
    nail = box((w + 2 * PLUG_FLANGE) / 2 - 3, -0.5, PLUG_FLANGE_T - 0.5, 6, 1.2, 1)
    return fl + body - nail

usb_plug = rect_plug(USB_CUT[1], USB_CUT[2] + 1.5)
jack_plug = rect_plug(JACK_CUT[1], JACK_CUT[2] + 1.5)

# Wire-slot plug: H-section that slides down into the slot from the top, with flanges
# inside and outside the wall. It stops on the leads' half-round channel and the lid holds it down.
slot_h = out_H - wire_z - 0.3
web_w = WIRE_HOLE_D - 2 * PLUG_CLEAR_W
fw = WIRE_HOLE_D + 2 * PLUG_FLANGE
slot_plug = (box(0, 0, 0, fw, slot_h, PLUG_FLANGE_T)                                         # outer flange
             + box((fw - web_w) / 2, 0, PLUG_FLANGE_T - 0.01, web_w, slot_h, WALL + 0.3 + 0.02)  # web through the wall
             + box(0, 0, PLUG_FLANGE_T + WALL + 0.3, fw, slot_h, PLUG_FLANGE_T))               # inner flange

# ---------------- export ----------------
def export(m, path):
    mesh = m.to_mesh()
    tm = trimesh.Trimesh(vertices=np.array(mesh.vert_properties)[:, :3], faces=np.array(mesh.tri_verts))
    tm.apply_translation(-tm.bounds[0])
    tm.export(path)
    ext = tm.bounds[1] - tm.bounds[0]
    print(f"{os.path.basename(path)}: {ext[0]:.1f} x {ext[1]:.1f} x {ext[2]:.1f} mm, watertight={tm.is_watertight}")
    return tm

if __name__ == "__main__":
    os.makedirs("stl_v3", exist_ok=True)
    export(base, "stl_v3/v3_case_base.stl")
    export(lid, "stl_v3/v3_case_lid.stl")
    export(plunger, "stl_v3/v3_case_plunger.stl")
    print(f"outer: {out_L:.1f} x {out_W:.1f} x {out_H:.1f} mm, lid {LID_T} mm")
    print(f"matrix posts at {mx_posts}, post length {post_len:.1f} mm")
    export(usb_plug, "stl_v3/v3_plug_usb.stl")
    export(jack_plug, "stl_v3/v3_plug_barrel.stl")
    export(slot_plug.rotate([90, 0, 0]), "stl_v3/v3_plug_slot.stl")
