from enum import Enum

import dearpygui.dearpygui as dpg
import numpy as np
import pytendo

from dbg.disassembler import Disassembler
from emu.emulator import Emulator

line_height = 20
scale_factor = 3


class State(Enum):
    PAUSED = 0
    RUNNING = 1
    BREAKPOINT = 2
    STEP = 3


class Debugger:
    def __init__(self, emulator: Emulator):
        self.emulator = emulator
        self.init_window()
        self.prg_rom = pytendo.dbg_prg_rom(self.emulator.emu)
        self.lines = Disassembler.disassemble(self.prg_rom)
        in_order = sorted(self.lines.items())
        self.line_by_address = {key: index for index, (key, value) in enumerate(in_order)}
        self.address_by_line = {index: key for index, (key, value) in enumerate(in_order)}
        self.lines_list = [value for key, value in sorted(self.lines.items())]
        self.create_window(self.lines)
        self.create_viewport()
        self.draw_state()
        self.draw_lines(self.lines_list)
        self.breakpoints = set()
        self.current = 0
        self.broken = 0
        self.state = State.PAUSED

    def run(self):
        while dpg.is_dearpygui_running():
            if dpg.is_mouse_button_released(dpg.mvMouseButton_Left):
                self.detect_click(self.lines_list)

            match self.state:
                case State.PAUSED:
                    pass
                case State.RUNNING:
                    for _ in range(29780):
                        state = pytendo.dbg_state(self.emulator.emu)
                        if state["cpu_register_pc"] in self.breakpoints:
                            self.state = State.BREAKPOINT
                            self.draw_state()
                            i = self.line_by_address[state["cpu_register_pc"]]
                            rect_tag = f"line_bg_{i}"
                            dpg.set_y_scroll("code_window", i * line_height - 100)
                            dpg.configure_item(rect_tag, fill=(50, 50, 200, 255))
                            self.broken = state["cpu_register_pc"]
                            print("breakpoint hit")
                            break
                        self.tick_cpu()
                case State.STEP:
                    state = pytendo.dbg_state(self.emulator.emu)
                    self.tick_cpu()
                    self.draw_state()
                    self.broken = state["cpu_register_pc"]
                    self.state = State.BREAKPOINT

            state = pytendo.dbg_state(self.emulator.emu)
            current = self.current
            self.current = state["cpu_register_pc"]
            if current != self.current:
                if current in self.line_by_address:
                    self.update_highlighted(self.line_by_address[current])
                if self.current in self.line_by_address:
                    self.update_highlighted(self.line_by_address[self.current])
                    dpg.set_y_scroll("code_window", self.line_by_address[self.current] * line_height - 100)
            self.draw_state()

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
                with dpg.child_window(width=200, height=240 * scale_factor, tag="code_window"):
                    with dpg.drawlist(width=170, height=len(lines) * 20, tag="code_drawlist"):
                        pass
                with dpg.group(horizontal=False):
                    with dpg.group(horizontal=True):
                        dpg.add_button(label="Run", callback=self.on_click_run)
                        dpg.add_button(label="Step", callback=self.on_click_step)
                    with dpg.child_window(width=400, height=46 * scale_factor, tag="state_window"):
                        with dpg.drawlist(width=380, height=6 * 20, tag="state_drawlist"):
                            pass
                    with dpg.tab_bar(tag="tab_bar", callback=self.on_tab_change):
                        with dpg.tab(label="CPU", tag="tab_cpu"):
                            pass
                        with dpg.tab(label="PPU", tag="tab_ppu"):
                            pass
                    with dpg.child_window(width=400, height=177 * scale_factor + 1, tag="memory_window"):
                        with dpg.drawlist(width=170, height=len(lines) * 20, tag="memory_drawlist"):
                            pass

    def on_click_run(self, sender, app_data):
        self.state = State.RUNNING
        broken = self.broken
        self.broken = 0
        if broken in self.line_by_address:
            self.update_highlighted(self.line_by_address[broken])

    def on_click_step(self, sender, app_data):
        self.state = State.STEP
        broken = self.broken
        current = self.current
        self.broken = 0
        self.current = 0
        if broken in self.line_by_address:
            self.update_highlighted(self.line_by_address[broken])
        if current in self.line_by_address:
            self.update_highlighted(self.line_by_address[current])

    def on_tab_change(self, sender, app_data):
        # print(f"Tab changed to: {app_data}")
        pass

    def create_viewport(self):
        dpg.create_viewport(title='pytendo', width=256 * scale_factor + 200 + 400 + 32, height=240 * scale_factor + 16,
                            resizable=False)
        dpg.setup_dearpygui()
        dpg.show_viewport()
        dpg.set_primary_window("Main", True)

    def draw_state(self):
        def draw_state_line(i: int, text: str):
            y_start = i * line_height
            y_end = y_start + line_height
            rect_tag = f"line_state_bg_{i}"
            text_tag = f"line_state_text_{i}"

            # Delete existing items if they exist
            if dpg.does_item_exist(rect_tag):
                dpg.delete_item(rect_tag)
            if dpg.does_item_exist(text_tag):
                dpg.delete_item(text_tag)

            # Recreate items
            dpg.draw_rectangle([0, y_start], [300, y_end], color=(0, 0, 0, 0), fill=(0, 0, 0, 0), tag=rect_tag,
                               parent="state_drawlist")
            dpg.draw_text([5, y_start + 5], text, color=(255, 255, 255), tag=text_tag, parent="state_drawlist")

        state = pytendo.dbg_state(self.emulator.emu)
        a = state["cpu_register_a"]
        x = state["cpu_register_x"]
        y = state["cpu_register_y"]
        p = state["cpu_register_p"]
        sp = state["cpu_register_sp"]
        pc = state["cpu_register_pc"]
        n = (p & 0x80) >> 7
        v = (p & 0x40) >> 6
        z = (p & 0x10) >> 4
        c = (p & 0x01) >> 0
        i = (p & 0x20) >> 5
        d = (p & 0x04) >> 2
        b = (p & 0x02) >> 1

        cpu_cycles = state["cycles_cpu"]
        ppu_cycles = state["cycles_ppu"]

        ppuctrl = state["ppu_register_ppuctrl"]
        ppumask = state["ppu_register_ppumask"]
        ppustatus = state["ppu_register_ppustatus"]
        oamaddr = state["ppu_register_oamaddr"]
        oamdata = state["ppu_register_oamdata"]
        ppuscroll = state["ppu_register_ppuscroll"]
        ppuaddr = state["ppu_register_ppuaddr"]
        ppudata = state["ppu_register_ppudata"]

        line_0 = f"A    X    Y    SP   PC     N V Z C I D B   CPU         PPU"
        line_1 = f"${a:02x}  ${x:02x}  ${y:02x}  ${sp:02x}  ${pc:04x}  {n} {v} {z} {c} {i} {d} {b}   {cpu_cycles:<12} {ppu_cycles:<12}"
        line_2 = "PPUCTRL     PPUMASK     PPUSTATUS   OAMADDR"
        line_3 = f"{f'${ppuctrl:04x}':<12}{f'${ppumask:04x}':<12}{f'${ppustatus:04x}':<12}{f'${oamaddr:04x}':<12}"
        line_4 = "OAMDATA     PPUSCROLL   PPUADDR     PPUDATA"
        line_5 = f"{f'${oamdata:04x}':<12}{f'${ppuscroll:04x}':<12}{f'${ppuaddr:04x}':<12}{f'${ppudata:04x}':<12}"
        # print(line_0)
        # print(line_1)
        draw_state_line(0, line_0)
        draw_state_line(1, line_1)
        draw_state_line(2, line_2)
        draw_state_line(3, line_3)
        draw_state_line(4, line_4)
        draw_state_line(5, line_5)

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

    def update_highlighted(self, line: int):
        if line not in self.address_by_line:
            return

        rect_tag = f"line_bg_{line}"
        dpg.configure_item(rect_tag, fill=(50, 50, 200, 255))
        address = self.address_by_line[line]
        if address == self.broken:
            dpg.configure_item(rect_tag, fill=(50, 50, 200, 255))
        elif address == self.current:
            dpg.configure_item(rect_tag, fill=(50, 100, 50, 255))
        elif address in self.breakpoints:
            dpg.configure_item(rect_tag, fill=(200, 50, 50, 255))
        else:
            dpg.configure_item(rect_tag, fill=None)

    def detect_click(self, lines: list[str]):
        mouse_pos = dpg.get_mouse_pos(local=True)
        scroll_offset = dpg.get_y_scroll("code_window")
        adjusted_y = mouse_pos[1] + scroll_offset

        for i, line in enumerate(lines):
            y_start = i * line_height + 4
            y_end = y_start + line_height
            if 0 <= mouse_pos[0] <= 180 and y_start <= adjusted_y <= y_end:
                # rect_tag = f"line_bg_{i}"
                address = self.address_by_line[i]
                if address in self.breakpoints:
                    print("breakpoint removed")
                    self.breakpoints.remove(address)
                else:
                    print("breakpoint added")
                    self.breakpoints.add(address)
                self.update_highlighted(i)
                break
