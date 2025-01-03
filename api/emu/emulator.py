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
