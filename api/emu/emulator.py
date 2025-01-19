from unittest import case

import numpy
import pytendo


class Emulator:
    def __init__(self, name, pal: str, rom: str):
        self.name = name
        self.emu = pytendo.emu_create(pal, rom)

    def tick(self) -> numpy.ndarray:
        pytendo.emu_tick(self.emu)
        return self.frame()

    def frame(self) -> numpy.ndarray:
        return pytendo.emu_frame(self.emu)

    def press(self, button: str) -> None:
        bit = self.__parse_button(button)
        pytendo.emu_controller_press(self.emu, bit)

    def release(self, button: str) -> None:
        bit = self.__parse_button(button)
        pytendo.emu_controller_release(self.emu, bit)

    @staticmethod
    def __parse_button(button: str) -> int:
        bit = 0b0
        match button.lower():
            case "a":
                bit = 0b00000001
            case "b":
                bit = 0b00000010
            case "select":
                bit = 0b00000100
            case "start":
                bit = 0b00001000
            case "up":
                bit = 0b00010000
            case "down":
                bit = 0b00100000
            case "left":
                bit = 0b01000000
            case "right":
                bit = 0b10000000
        return bit
