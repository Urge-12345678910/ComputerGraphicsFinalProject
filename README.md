# PDD Driver - 3D Traffic Rules Trainer
OpenGL 3.3 Core arcade game: drive through a city, obey traffic lights and speed limit signs.

## Team
| Member | Module |
|---|---|
| Deniz Akhmedjanov | Renderer: Shader, Mesh, fragment.frag (lighting, fog) |
| Adil Daulet | Gameplay: Game, PlayerCar, Traffic |
| Khaknazar Mursalim  | World/UI: RoadSystem, UI, README/report |

## Requirements
Windows 10/11 x64, Visual Studio 2022 (MSVC v143), GPU with OpenGL 3.3.

## Build and run
1. Clone the repository.
2. Open `OpenGL_Project.sln` in Visual Studio.
3. Select **x64** and Debug or Release.
4. Build (Ctrl+Shift+B), run (F5).
No extra installs: GLFW (static lib) and GLAD are inside `Libraries/`. Shaders are in `Shaders/` and are found automatically.

## Controls
| Key | Action |
|---|---|
| Enter | Start game |
| W / S | Gas / brake |
| A / D | Change lane |
| P | Pause |
| R | Restart |
| F | Lighting ON/OFF (main toggle: Blinn-Phong + headlights vs flat colors) |
| G | Fog ON/OFF |
| N | Dusk / Day / Night |
| Q / E | Rotate the sun |
| C | Free camera (WASD, mouse, Space/Ctrl, Shift) |
| H / F1 | Help |
| Esc | Quit |

## Rules
Red light = 1 penalty. Exceeding the sign speed limit by more than 5 km/h for 2 s = 1 penalty.
Collision = 2 penalties. 10 penalties = Game Over. Score = distance/10 - penalties*20.

## Video Link
https://youtu.be/hWB8buZlr7A?si=sjXWnX52bO4t2uTM

## Presentation 
https://canva.link/f1qgt5wtnd9asj9

## Libraries
GLFW 3 (window/input), GLAD (OpenGL loader). Math, mesh generation, bitmap font and UI are written by the team.

