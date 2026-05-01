# Dark Dungeon 🔥

## Games Programming 2 - Coursework
**Glasgow Caledonian University**

A 3D dungeon exploration application built with OpenGL and C++.

## Demo Video
https://github.com/huseynshahinduran/DarkDungeon/blob/main/demo.mp4

## Features

### Core
- Scene Framework (MainGame, Display, Game Loop)
- 3D Models loaded from OBJ files (torch, chest, barrel, door, key)
- Vertex and Fragment shaders loaded from file
- Camera with mouse look and WASD movement
- Textures loaded with STB library

### Extension - Rendering & Shaders
- **ADS Lighting** - Ambient, Diffuse, Specular with attenuation
- **Rim Lighting** - Edge glow effect on treasure chest
- **Fog** - Distance-based dungeon atmosphere
- **Toon Shading** - Cel-shaded rendering (toggle with T key)
- **Procedural Flame** - Shader-generated fire animation
- **Light Flicker** - Realistic torch light flickering

### Extension - Application Architecture
- Frame Timing with delta time and V-Sync
- Two-room dungeon with wall collision
- Game mechanics: find key, open door, reach treasure

### Extension - User Input
- Mouse camera control (FPS style)
- WASD movement relative to camera direction
- E key interaction (pick up key, open door)
- T key shader toggle (ADS / Toon)
- R key scene reset

## Controls
| Key | Action |
|-----|--------|
| Mouse | Look around |
| WASD | Move |
| E | Pick up key / Open door |
| T | Toggle Toon Shading |
| R | Reset scene |
| ESC | Exit |

## Built With
- C++ / OpenGL / GLSL
- SDL2
- GLEW
- GLM
