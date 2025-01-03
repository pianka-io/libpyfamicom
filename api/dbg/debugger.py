import dearpygui.dearpygui as dpg
import numpy as np
import pytendo

from dbg.disassembler import Disassembler
from emu.emulator import Emulator

line_height = 20
scale_factor = 3

class Debugger:
    def __init__(self, emulator: Emulator):
        self.emulator = emulator
        self.init_window()
        self.prg_rom = pytendo.dbg_prg_rom(self.emulator.emu)
        self.lines = Disassembler.disassemble(self.prg_rom)
        self.create_window(self.lines)
        self.create_viewport()
        self.draw_lines(self.lines)
        self.clicked = {}

    def run(self):
        while dpg.is_dearpygui_running():
            if dpg.is_mouse_button_released(dpg.mvMouseButton_Left):
                self.detect_click(self.lines)
            self.tick_frame()
            frame = self.emulator.frame()
            frame = np.transpose(frame, (1, 0, 2))
            frame_with_alpha = np.dstack((frame, np.full((240, 256, 1), 255, dtype=np.uint8)))
            scaled_screen = np.repeat(np.repeat(frame_with_alpha, scale_factor, axis=0), scale_factor, axis=1)
            dpg.set_value("screen_texture", scaled_screen.flatten() / 255)
            dpg.render_dearpygui_frame()
        dpg.destroy_context()

    def tick_frame(self):
        self.emulator.tick()

    def tick_cpu(self):
        pytendo.dbg_cpu_tick(self.emulator.emu)

    def init_window(self):
        dpg.create_context()
        with dpg.texture_registry():
            dpg.add_dynamic_texture(
                width=256 * scale_factor,
                height=240 * scale_factor,
                default_value=(0, 0, 0, 255),
                tag="screen_texture"
            )

    def create_window(self, lines):
        with dpg.window(tag="Main"):
            with dpg.group(horizontal=True):
                dpg.add_image("screen_texture")
                with dpg.child_window(width=200, height=240 * scale_factor, tag="scrollable_window"):
                    with dpg.drawlist(width=170, height=len(lines) * 20, tag="code_drawlist"):
                        pass
                with dpg.group(horizontal=False):
                    dpg.add_text("Disassembly")
                    with dpg.child_window(width=200, height=200 * scale_factor, tag="scrollable_window_2"):
                        with dpg.drawlist(width=170, height=len(lines) * 20, tag="code_drawlist_2"):
                            pass

    def create_viewport(self):
        dpg.create_viewport(title='pytendo', width=256 * scale_factor + 200 + 200 + 32, height=240 * scale_factor + 16,
                            resizable=False)
        dpg.setup_dearpygui()
        dpg.show_viewport()
        dpg.set_primary_window("Main", True)

    def draw_lines(self, lines: list[str]):
        for i, line in enumerate(lines):
            y_start = i * line_height
            y_end = y_start + line_height
            rect_tag = f"line_bg_{i}"
            text_tag = f"line_text_{i}"

            dpg.draw_rectangle([0, y_start], [300, y_end], color=(0, 0, 0, 0), fill=(0, 0, 0, 0), tag=rect_tag,
                               parent="code_drawlist")
            # backgrounds[rect_tag] = (0, 0, 0, 0)
            dpg.draw_text([5, y_start + 5], line, color=(255, 255, 255), tag=text_tag, parent="code_drawlist")

    def detect_click(self, lines: list[str]):
        mouse_pos = dpg.get_mouse_pos(local=True)
        scroll_offset = dpg.get_y_scroll("scrollable_window")
        adjusted_y = mouse_pos[1] + scroll_offset

        for i, line in enumerate(lines):
            y_start = i * line_height
            y_end = y_start + line_height
            if 0 <= mouse_pos[0] <= 300 and y_start <= adjusted_y <= y_end:
                print(f"Clicked line {i}: {line}")
                rect_tag = f"line_bg_{i}"
                if i in self.clicked:
                    del self.clicked[i]
                    dpg.configure_item(rect_tag, fill=None)
                else:
                    self.clicked[i] = True
                    dpg.set_y_scroll("scrollable_window", i * line_height)
                    dpg.configure_item(rect_tag, fill=(50, 50, 200, 255))
                break
