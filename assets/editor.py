#!/bin/python3

import pygame
import sys
import os

# Initialize Pygame
pygame.init()

# Constants
SCREEN_WIDTH = 1280
SCREEN_HEIGHT = 960
GRID_SIZE = 32
GRID_WIDTH = 30
GRID_HEIGHT = 30
TILESET_SIZE = 320
TILE_SIZE = 32

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
SEMI_TRANSPARENT_WHITE = (255, 255, 255, 128)

# Set up the screen
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Tilemap Editor")

# Load the tileset image
tileset = pygame.image.load("tileset.png").convert()

# The main grid where tiles will be placed
grid = [[None for _ in range(GRID_WIDTH)] for _ in range(GRID_HEIGHT)]

# Selected tile position
selected_tile = None

# Button
button_font = pygame.font.SysFont(None, 36)
export_button_text = button_font.render('Export', True, WHITE)
export_button_rect = export_button_text.get_rect(
    bottomright=(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10))
load_button_text = button_font.render('Load', True, WHITE)
load_button_rect = load_button_text.get_rect(
    bottomright=(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 50))

# Function to draw the grid


def draw_grid():
    grid_surface = pygame.Surface(
        (GRID_WIDTH * GRID_SIZE, GRID_HEIGHT * GRID_SIZE), pygame.SRCALPHA)
    for y in range(GRID_HEIGHT):
        for x in range(GRID_WIDTH):
            rect = pygame.Rect(x * GRID_SIZE, y * GRID_SIZE,
                               GRID_SIZE, GRID_SIZE)
            pygame.draw.rect(grid_surface, SEMI_TRANSPARENT_WHITE, rect, 1)
            if grid[y][x] is not None:
                tile_x, tile_y = grid[y][x]
                screen.blit(tileset, (x * GRID_SIZE, y * GRID_SIZE),
                            (tile_x * TILE_SIZE, tile_y * TILE_SIZE, TILE_SIZE, TILE_SIZE))
    screen.blit(grid_surface, (0, 0))

# Function to draw the tileset


def draw_tileset():
    for y in range(TILESET_SIZE // TILE_SIZE):
        for x in range(TILESET_SIZE // TILE_SIZE):
            screen.blit(tileset, (GRID_WIDTH * GRID_SIZE + x * TILE_SIZE, y *
                        TILE_SIZE), (x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE))
            rect = pygame.Rect(GRID_WIDTH * GRID_SIZE + x *
                               TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE)
            pygame.draw.rect(screen, WHITE, rect, 1)

# Function to export the map


def export_map():
    with open('map', 'w') as f:
        for y in range(GRID_HEIGHT):
            row = []
            for x in range(GRID_WIDTH):
                if grid[y][x] is None:
                    row.append("00")
                else:
                    tile_x, tile_y = grid[y][x]
                    index = tile_y * (TILESET_SIZE // TILE_SIZE) + tile_x
                    row.append(f"{index:02}")
            f.write(" ".join(row) + "\n")

# Function to load the map


def load_map():
    if os.path.exists('map'):
        with open('map', 'r') as f:
            for y, line in enumerate(f.readlines()):
                indices = line.strip().split()
                for x, index in enumerate(indices):
                    index = int(index)
                    if index == 0:
                        grid[y][x] = None
                    else:
                        tile_x = index % (TILESET_SIZE // TILE_SIZE)
                        tile_y = index // (TILESET_SIZE // TILE_SIZE)
                        grid[y][x] = (tile_x, tile_y)


# Main loop
running = True
left_mouse_held = False
right_mouse_held = False

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            mouse_x, mouse_y = event.pos
            if event.button == 1:  # Left mouse button
                left_mouse_held = True
                if export_button_rect.collidepoint(mouse_x, mouse_y):
                    export_map()
                    print("exported")
                elif load_button_rect.collidepoint(mouse_x, mouse_y):
                    load_map()
                    print("loaded")
                elif mouse_x < GRID_WIDTH * GRID_SIZE:
                    # Clicked on the main grid
                    grid_x = mouse_x // GRID_SIZE
                    grid_y = mouse_y // GRID_SIZE
                    if selected_tile is not None:
                        grid[grid_y][grid_x] = selected_tile
                elif mouse_x >= GRID_WIDTH * GRID_SIZE and mouse_x < GRID_WIDTH * GRID_SIZE + TILESET_SIZE:
                    # Clicked on the tileset
                    tile_x = (mouse_x - GRID_WIDTH * GRID_SIZE) // TILE_SIZE
                    tile_y = mouse_y // TILE_SIZE
                    selected_tile = (tile_x, tile_y)
            elif event.button == 3:  # Right mouse button
                right_mouse_held = True
                if mouse_x < GRID_WIDTH * GRID_SIZE:
                    # Clicked on the main grid
                    grid_x = mouse_x // GRID_SIZE
                    grid_y = mouse_y // GRID_SIZE
                    # Remove tile from the grid cell
                    grid[grid_y][grid_x] = None
        elif event.type == pygame.MOUSEBUTTONUP:
            if event.button == 1:  # Left mouse button
                left_mouse_held = False
            elif event.button == 3:  # Right mouse button
                right_mouse_held = False

    # Handle continuous tile placement/removal
    if left_mouse_held or right_mouse_held:
        mouse_x, mouse_y = pygame.mouse.get_pos()
        if mouse_x < GRID_WIDTH * GRID_SIZE:
            grid_x = mouse_x // GRID_SIZE
            grid_y = mouse_y // GRID_SIZE
            if left_mouse_held and selected_tile is not None:
                grid[grid_y][grid_x] = selected_tile
            elif right_mouse_held:
                grid[grid_y][grid_x] = None

    # Draw everything
    screen.fill(BLACK)
    draw_grid()
    draw_tileset()

    # Draw the buttons
    pygame.draw.rect(screen, BLACK, export_button_rect)
    screen.blit(export_button_text, export_button_rect)
    pygame.draw.rect(screen, BLACK, load_button_rect)
    screen.blit(load_button_text, load_button_rect)

    pygame.display.flip()

# Quit Pygame
pygame.quit()
sys.exit()
