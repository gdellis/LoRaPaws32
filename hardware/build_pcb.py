#!/usr/bin/env python3
"""
Build tracker PCB programmatically using KiCad pcbnew API
"""

import sys

sys.path.insert(0, "/usr/lib/kicad/plugins")

import pcbnew

BOARD_WIDTH_MM = 60
BOARD_HEIGHT_MM = 40


def from_mm(mm):
    return pcbnew.FromMM(mm)


def create_board_outline(board):
    """Create rectangular board outline"""
    w = from_mm(BOARD_WIDTH_MM)
    h = from_mm(BOARD_HEIGHT_MM)

    corners = [
        (0, 0, w, 0),  # bottom
        (w, 0, w, h),  # right
        (w, h, 0, h),  # top
        (0, h, 0, 0),  # left
    ]

    for x1, y1, x2, y2 in corners:
        shape = pcbnew.PCB_SHAPE(board)
        shape.SetShape(pcbnew.SHAPE_T_SEGMENT)
        shape.SetLayer(pcbnew.Edge_Cuts)
        shape.SetStart(pcbnew.VECTOR2I(x1, y1))
        shape.SetEnd(pcbnew.VECTOR2I(x2, y2))
        board.Add(shape)

    print("Created board outline: {}mm x {}mm".format(BOARD_WIDTH_MM, BOARD_HEIGHT_MM))


def add_text(board, text, x_mm, y_mm, layer=None):
    """Add text to board"""
    if layer is None:
        layer = pcbnew.F_SilkS

    txt = pcbnew.PCB_TEXT(board)
    txt.SetText(text)
    txt.SetPosition(pcbnew.VECTOR2I(from_mm(x_mm), from_mm(y_mm)))
    txt.SetLayer(layer)
    txt.SetTextSize(pcbnew.VECTOR2I(from_mm(1.5), from_mm(1.5)))
    board.Add(txt)
    return txt


def add_mounting_hole(board, x_mm, y_mm):
    """Add a mounting hole at specified location"""
    hole_drill = from_mm(3)  # M3 holes
    via = pcbnew.PCB_VIA(board)
    via.SetPosition(pcbnew.VECTOR2I(from_mm(x_mm), from_mm(y_mm)))
    via.SetDrill(hole_drill)
    via.SetWidth(hole_drill + from_mm(1.2))
    board.Add(via)
    print("Added mounting hole at ({}, {})".format(x_mm, y_mm))


def main():
    board = pcbnew.BOARD()

    create_board_outline(board)

    add_text(board, "Pet Tracker", 20, 35, pcbnew.F_SilkS)
    add_text(board, "ESP32-S3 + LoRa", 20, 33, pcbnew.B_SilkS)

    # Add mounting holes at corners
    corners = [(3, 3), (57, 3), (3, 37), (57, 37)]
    for x_mm, y_mm in corners:
        add_mounting_hole(board, x_mm, y_mm)

    output_path = (
        "/home/glenn/projects/esp32-tracker/hardware/tracker/tracker.kicad_pcb"
    )
    pcbnew.SaveBoard(output_path, board)
    print("Saved PCB to: {}".format(output_path))

    return 0


if __name__ == "__main__":
    sys.exit(main())
