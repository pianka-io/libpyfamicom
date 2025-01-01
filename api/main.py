import time

import pygame
import numpy as np
import pytendo
emu = pytendo.create_emu("bin/pals/2C02G.pal", "bin/tests/1.Branch_Basics.nes")


pygame.init()
SCALE = 3
WIDTH, HEIGHT = 256, 240
screen = pygame.display.set_mode((WIDTH * SCALE, HEIGHT * SCALE))

np.set_printoptions(threshold=10**6)

##

# for _ in range(10000):
#     pytendo.tick_emu(emu)
# frame = pytendo.frame_emu(emu)
# for y in range(240):  # 240 rows
#     for x in range(256):
#         row = frame[x, y, :]  # Get all columns (x) for row y
#         if np.array_equal(row, [0, 0, 0]):
#             print("-", end="")
#         else:
#             print("X", end="")
#         # print(row, end="")
#     print()

##

frames = 0
last = time.perf_counter()
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    pytendo.tick_emu(emu)
    frame = pytendo.frame_emu(emu)
    frames += 1
    surface = pygame.surfarray.make_surface(frame)
    scaled_surface = pygame.transform.scale(surface, (WIDTH * SCALE, HEIGHT * SCALE))
    screen.blit(scaled_surface, (0, 0))
    pygame.display.flip()
    if time.perf_counter() - last > 1.0:
        print(f"{frames}fps")
        last = time.perf_counter()
        frames = 0

pygame.quit()
