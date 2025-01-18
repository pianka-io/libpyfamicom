import time
import pygame
import pytendo

from emu.emulator import Emulator

# emu = Emulator("pytendo", "bin/pals/2C02G.pal", "bin/tests/1.Branch_Basics.nes")
# emu = Emulator("pytendo", "bin/pals/2C02G.pal", "bin/tests/cpu_dummy_writes_ppumem.nes")
emu = Emulator("pytendo", "bin/pals/2C02G.pal", "bin/tests/nestest.nes")

clock = pygame.time.Clock()
pygame.init()
pygame.display.set_caption("pytendo")
SCALE = 3
WIDTH, HEIGHT = 256, 240
screen = pygame.display.set_mode((WIDTH * SCALE, HEIGHT * SCALE))

def print_frame(frame):
    print("start")
    for y in range(240):
        for x in range(256):
            r, g, b = frame[x, y, :]
            mean = (r + g + b) / 3
            print(f"{int(mean):02x}", end="")
        print()
    print("end")

frames = 0
last = time.perf_counter()
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    emu.tick()
    frame = emu.frame()
    # print_frame(frame)
    frames += 1
    surface = pygame.surfarray.make_surface(frame)
    scaled_surface = pygame.transform.scale(surface, (WIDTH * SCALE, HEIGHT * SCALE))
    screen.blit(scaled_surface, (0, 0))
    pygame.display.flip()
    if time.perf_counter() - last > 1.0:
        print(f"{frames}fps")
        pygame.display.set_caption(f"pytendo @ {frames}fps")
        last = time.perf_counter()
        frames = 0
    clock.tick(60)

pygame.quit()

