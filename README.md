# 🐍 Ainz in OpenGL — Ball Python Enclosure Simulation 🐍

This project simulates my ball python named **Ainz** moving inside a glass terrarium using OpenGL and C++. It includes directional shading, shadow casting, scales, a heat lamp, and realistic terrain using randomized grain patterns.

## 🖤 Features 🖤

- Sine-wave slithering or curled ball mode
- Heat lamp toggle with realistic lighting
- Dynamic shadows that adjust with lighting
- Snake scales with alternating color patterns
- Venn diagram-style water bowl
- Glass enclosure with reflective streaks
- Custom textured terracotta sand floor

## 🕸️ Scene Description 🕸️

The animation includes:
- A snake that follows the mouse when in slither mode.
- A warm heat lamp in the top-right that illuminates the environment.
- A sandy enclosure with hundreds of textured grains.
- A stylized water bowl and glass walls with animated highlights.
- The ability to toggle between curled and slithering states.

All visuals are built using raw OpenGL calls and simulated using real-time frame updates.

## ⚰️ Files ⚰️

- `ainz.cpp` – full implementation of the OpenGL animation
- `ainz.mp4` – video animation of Ainz 
- `README.md` – this documentation

## 🩸 How to Run 🩸

```bash
# 1. Compile with GLFW and OpenGL
g++ ainz.cpp -std=c++11 -DGL_SILENCE_DEPRECATION -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -framework OpenGL -o ainz

# 2. Run the program
./ainz
