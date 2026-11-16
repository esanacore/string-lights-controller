"""
Uno R3 + proto shield enclosure for the string light controller.

Prints on the Adventurer 5M Pro (0.6 mm nozzle, PETG):
  - base: open side up, no supports
  - lid:  flat side down, lip up, no supports

Board coordinates follow the standard Uno R3 drawing:
origin = PCB lower-left corner, USB-B and barrel jack on the x=0 edge.
All dimensions in mm. Edit the PARAMETERS block and re-run:
    python uno_case.py   ->  uno_case_base.stl, uno_case_lid.stl
"""

from manifold3d import Manifold, CrossSection
import trimesh
import numpy as np

# ---------------- PARAMETERS ----------------
WALL = 2.4            # 4 perimeters at 0.6 mm
FLOOR = 2.0
PCB_L, PCB_W, PCB_T = 68.6, 53.3, 1.6
STANDOFF_H = 5.0      # floor to bottom of Uno PCB
STANDOFF_D = 6.0
PILOT_D = 2.6         # M3 self-tapping screw into PETG
STACK_H = 38.0        # Uno PCB top to lid: shield + parts (~20 mm) + matrix hanging from lid (~18 mm)
TOP_GAP = 3.0         # air above the tallest part

USB_OVERHANG = 6.4    # USB-B sticks out past the PCB edge
CLEAR = 1.0           # side clearance around the PCB

# Uno R3 mounting holes (x, y)
HOLES = [(14.0, 2.5), (15.3, 50.7), (66.1, 7.6), (66.1, 35.5)]

# Connector cutouts on the x=0 wall: (center_y, width, height), height measured from PCB top.
# Sized for the cable plug overmold, not just the socket, so the cable fits through the wall.
USB_CUT = (44.4, 16.0, 14.0)
JACK_CUT = (7.6, 12.0, 13.0)

WIRE_HOLE_D = 6.0     # string lead exit on the far wall
LID_T = 2.0
LIP_H = 4.0
LIP_T = 1.6
LID_FIT = 0.25        # gap per side between lip and wall (PETG friction fit)
VENT_W, VENT_L, VENT_N = 2.0, 30.0, 6

# S1 button (measured on the shield): center 25.5 mm from the USB-end edge,
# 3.75 mm from the power-header edge
S1_X, S1_Y = 25.5, 3.75
S1_TOP = 19.6          # est. height of the S1 cap above the Uno PCB top
PLUNGER_D = 6.0
TUBE_ID, TUBE_OD = 6.6, 8.8   # OD kept small: S1 sits close to the box wall
TUBE_BOTTOM = 28.0     # tube ends this high above the Uno PCB top (leaves collar travel)

# 8x8 matrix: LED block slides up into a frame under the lid and glues in place
MX_BLOCK = 32.6        # pocket for the 32 mm LED block
MX_WINDOW = 31.0       # visible opening (0.8 mm lip holds the block)
MX_X0, MX_Y0 = 10.0, 17.0   # pocket corner in board coords; header end points toward +x
MX_FRAME_H = 4.0
# --------------------------------------------

def box(x, y, z, sx, sy, sz):
    return Manifold.cube([sx, sy, sz]).translate([x, y, z])

def cyl(x, y, z, d, h, seg=48):
    return Manifold.cylinder(h, d / 2, d / 2, seg).translate([x, y, z])

# Cavity extents in board coordinates
cav_x0 = -USB_OVERHANG - 0.4          # USB face nearly touches the inner wall
cav_x1 = PCB_L + CLEAR
cav_y0 = -CLEAR
cav_y1 = PCB_W + CLEAR
cav_h = STANDOFF_H + PCB_T + STACK_H + TOP_GAP

out_x0, out_y0 = cav_x0 - WALL, cav_y0 - WALL
out_L = (cav_x1 - cav_x0) + 2 * WALL
out_W = (cav_y1 - cav_y0) + 2 * WALL
out_H = FLOOR + cav_h

pcb_bottom = FLOOR + STANDOFF_H
pcb_top = pcb_bottom + PCB_T

# ---------------- base ----------------
base = box(out_x0, out_y0, 0, out_L, out_W, out_H)
base -= box(cav_x0, cav_y0, FLOOR, cav_x1 - cav_x0, cav_y1 - cav_y0, cav_h + 1)

for (hx, hy) in HOLES:
    base += cyl(hx, hy, FLOOR - 0.01, STANDOFF_D, STANDOFF_H + 0.01)
    base -= cyl(hx, hy, FLOOR - 1.0, PILOT_D, STANDOFF_H + 2)

for (cy, w, h) in (USB_CUT, JACK_CUT):
    base -= box(out_x0 - 1, cy - w / 2, pcb_top - 1.5, WALL + 2, w, h + 1.5)

# string lead exit, centered on the far wall at breadboard height
# open-top slot so the wires drop in from above (string can't be threaded through a hole)
wire_z = pcb_top + 14
base -= Manifold.cylinder(WALL + 2, WIRE_HOLE_D / 2, WIRE_HOLE_D / 2, 48) \
    .rotate([0, 90, 0]).translate([cav_x1 - 1, PCB_W / 2, wire_z])
base -= box(cav_x1 - 1, PCB_W / 2 - WIRE_HOLE_D / 2, wire_z, WALL + 2, WIRE_HOLE_D, out_H)

# ---------------- lid ----------------
lid = box(out_x0, out_y0, 0, out_L, out_W, LID_T)
lip_x0, lip_y0 = cav_x0 + LID_FIT, cav_y0 + LID_FIT
lip_L = (cav_x1 - cav_x0) - 2 * LID_FIT
lip_W = (cav_y1 - cav_y0) - 2 * LID_FIT
lip = box(lip_x0, lip_y0, LID_T - 0.01, lip_L, lip_W, LIP_H)
lip -= box(lip_x0 + LIP_T, lip_y0 + LIP_T, LID_T - 1, lip_L - 2 * LIP_T, lip_W - 2 * LIP_T, LIP_H + 2)
lip -= box(cav_x1 - LIP_T - 1, PCB_W / 2 - WIRE_HOLE_D / 2 - 0.5, LID_T - 1, LIP_T + 2, WIRE_HOLE_D + 1, LIP_H + 2)
lid += lip

# lid coordinates: z = 0 is the lid's top face (printed face down), +z goes into the box.
# Height above the Uno PCB top -> distance below the lid top:
lid_top_above_pcb = STACK_H + TOP_GAP + LID_T
def below_lid(h_above_pcb):
    return lid_top_above_pcb - h_above_pcb

# The lid prints face down and gets flipped onto the box, which mirrors Y.
# Mirror every lid feature's Y so it lands in the right place after the flip.
cyc = (cav_y0 + cav_y1) / 2
def my(y):
    return 2 * cyc - y

# S1 plunger guide tube
tube_len = below_lid(TUBE_BOTTOM) - LID_T
lid += cyl(S1_X, my(S1_Y), LID_T - 0.01, TUBE_OD, tube_len + 0.01)
lid -= cyl(S1_X, my(S1_Y), -1, TUBE_ID, tube_len + LID_T + 2)

# matrix frame + window
f = 1.6
MX_Y0m = my(MX_Y0 + MX_BLOCK)
lid += box(MX_X0 - f, MX_Y0m - f, LID_T - 0.01, MX_BLOCK + 2 * f, MX_BLOCK + 2 * f, MX_FRAME_H + 0.01)
lid -= box(MX_X0, MX_Y0m, LID_T - 0.8, MX_BLOCK, MX_BLOCK, MX_FRAME_H + 2)        # pocket (0.8 lip left at top)
lid -= box(MX_X0 + (MX_BLOCK - MX_WINDOW) / 2, MX_Y0m + (MX_BLOCK - MX_WINDOW) / 2, -1,
           MX_WINDOW, MX_WINDOW, LID_T + 2)                                       # window

# vent slots, now over the far end beside the matrix
for i in range(4):
    x = 52 + i * 4.5
    lid -= box(x, my(44), -1, VENT_W, 26, LID_T + 2)

# plunger: rod with a collar at the bottom (collar rests on S1, can't pass up the tube)
rod_len = lid_top_above_pcb + 3.0 - S1_TOP
plunger = cyl(0, 0, 0, PLUNGER_D, rod_len) + cyl(0, 0, 0, TUBE_OD, 1.6)

def export(m, path):
    mesh = m.to_mesh()
    tm = trimesh.Trimesh(vertices=np.array(mesh.vert_properties)[:, :3],
                         faces=np.array(mesh.tri_verts))
    tm.apply_translation(-tm.bounds[0])   # sit on the bed at the origin
    tm.export(path)
    ext = tm.bounds[1] - tm.bounds[0]
    print(f"{path}: {ext[0]:.1f} x {ext[1]:.1f} x {ext[2]:.1f} mm, watertight={tm.is_watertight}")

export(base, "uno_case_base.stl")
export(lid, "uno_case_lid.stl")
export(plunger, "uno_case_plunger.stl")
