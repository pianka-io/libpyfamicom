import dearpygui.dearpygui as dpg
import numpy as np
import pytendo

from disassembler import Disassembler

emu = pytendo.create_emu("bin/pals/2C02G.pal", "bin/tests/1.Branch_Basics.nes")
prg_rom = pytendo.debug_prg_rom(emu)
lines = Disassembler.disassemble(prg_rom)

scale_factor = 3
dpg.create_context()

with dpg.texture_registry():
    dpg.add_dynamic_texture(
        width=256 * scale_factor,
        height=240 * scale_factor,
        default_value=(0, 0, 0, 255),
        tag="screen_texture"
    )

with dpg.window(tag="Main"):
    with dpg.group(horizontal=True):
        dpg.add_image("screen_texture")
        with dpg.child_window(width=300, height=240 * scale_factor, tag="scrollable_window"):
            with dpg.drawlist(width=270, height=len(lines) * 20, tag="code_drawlist"):
                pass

assembly_lines = lines
line_height = 20
backgrounds = {}

for i, line in enumerate(assembly_lines):
    print(i)
    y_start = i * line_height
    y_end = y_start + line_height
    rect_tag = f"line_bg_{i}"
    text_tag = f"line_text_{i}"

    dpg.draw_rectangle([0, y_start], [300, y_end], color=(0, 0, 0, 0), fill=(0, 0, 0, 0), tag=rect_tag, parent="code_drawlist")
    backgrounds[rect_tag] = (0, 0, 0, 0)
    dpg.draw_text([5, y_start + 5], line, color=(255, 255, 255), tag=text_tag, parent="code_drawlist")

clicked = {}

def detect_click():
    mouse_pos = dpg.get_mouse_pos(local=True)
    scroll_offset = dpg.get_y_scroll("scrollable_window")
    adjusted_y = mouse_pos[1] + scroll_offset

    for i, line in enumerate(assembly_lines):
        y_start = i * line_height
        y_end = y_start + line_height
        if 0 <= mouse_pos[0] <= 300 and y_start <= adjusted_y <= y_end:
            print(f"Clicked line {i}: {line}")
            rect_tag = f"line_bg_{i}"
            if i in clicked:
                del clicked[i]
                dpg.configure_item(rect_tag, fill=None)
            else:
                clicked[i] = True
                dpg.set_y_scroll("scrollable_window", i * line_height)
                dpg.configure_item(rect_tag, fill=(50, 50, 200, 255))
            break

dpg.create_viewport(title='pytendo', width=256 * scale_factor + 300 + 24, height=240 * scale_factor + 16, resizable=False)
dpg.setup_dearpygui()
dpg.show_viewport()
dpg.set_primary_window("Main", True)

while dpg.is_dearpygui_running():
    if dpg.is_mouse_button_released(dpg.mvMouseButton_Left):
        detect_click()

    pytendo.tick_emu(emu)
    frame = pytendo.frame_emu(emu)
    frame = np.transpose(frame, (1, 0, 2))
    frame_with_alpha = np.dstack((frame, np.full((240, 256, 1), 255, dtype=np.uint8)))
    scaled_screen = np.repeat(np.repeat(frame_with_alpha, scale_factor, axis=0), scale_factor, axis=1)
    dpg.set_value("screen_texture", scaled_screen.flatten() / 255)
    dpg.render_dearpygui_frame()

dpg.destroy_context()
