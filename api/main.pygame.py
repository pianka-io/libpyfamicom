import time
import pygame

from emu.emulator import Emulator

emu = Emulator("pytendo", "bin/pals/2C02G.pal", "bin/roms/SpriteTest.nes")
clock = pygame.time.Clock()
pygame.init()
pygame.display.set_caption("pytendo")
SCALE = 3
WIDTH, HEIGHT = 256, 240
screen = pygame.display.set_mode((WIDTH * SCALE, HEIGHT * SCALE))

key_map = {
    pygame.K_a: "A",
    pygame.K_b: "B",
    pygame.K_RETURN: "Start",
    pygame.K_RSHIFT: "Select",
    pygame.K_UP: "Up",
    pygame.K_DOWN: "Down",
    pygame.K_LEFT: "Left",
    pygame.K_RIGHT: "Right",
}

frames = 0
last = time.perf_counter()
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key in key_map:
                emu.press(key_map[event.key])
        elif event.type == pygame.KEYUP:
            if event.key in key_map:
                emu.release(key_map[event.key])
    emu.tick()
    frame = emu.frame()
    frames += 1
    surface = pygame.surfarray.make_surface(frame)
    scaled_surface = pygame.transform.scale(surface, (WIDTH * SCALE, HEIGHT * SCALE))
    screen.blit(scaled_surface, (0, 0))
    pygame.display.flip()
    if time.perf_counter() - last > 1.0:
        pygame.display.set_caption(f"pytendo @ {frames}fps")
        last = time.perf_counter()
        frames = 0
    clock.tick(60)

pygame.quit()

