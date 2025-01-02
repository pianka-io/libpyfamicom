import time

import pygame
import pytendo
# passed: emu = pytendo.create_emu("bin/pals/2C02G.pal", "bin/tests/1.Branch_Basics.nes")
# passed: emu = pytendo.create_emu("bin/pals/2C02G.pal", "bin/tests/2.Backward_branch.nes")
# passed: emu = pytendo.create_emu("bin/pals/2C02G.pal", "bin/tests/3.Forward_branch.nes")
emu = pytendo.create_emu("bin/pals/2C02G.pal", "bin/tests/cpu_dummy_reads.nes")


pygame.init()
SCALE = 3
WIDTH, HEIGHT = 256, 240
screen = pygame.display.set_mode((WIDTH * SCALE, HEIGHT * SCALE))

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
