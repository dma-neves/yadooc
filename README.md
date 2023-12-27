# Yet Another Doom Clone

![alt text](/other/doom_stairs_render_crop.png)

## Description

- Another 2.5D DOOM inspired software renderer written in C++.
- This renderer sits between a DOOM and a Duke Nukem 3D renderer, as it iteratively projects vertical walls to a projection plane in a similar fashion to Doom (not allowing true up and down camera rotation), while also allowing sloped surfaces to be rendered similarly to Duke Nukem 3D. However, contrary to Doom and Duke Nukem, no BSP or sector based rendering technique are used.
- yadooc receives a map data structure as input to render. This data structure consists of a set of solids that can have any number of edges but must be convex and constructed via the combination of a set of vertical "walls".
- Note that it is easy to define sloped shapes that break the renderer given that it is actually just projecting the vertical walls and then "connecting the dots". Prism-like solids should render "flawlessly".
- Currently only wall texture mapping is supported (no ground texture mapping).

## Requirements

- cmake
- SFML

## Building and running

- To build run: `mkdir build; cd build; cmake -DCMAKE_BUILD_TYPE=Release ..; make`
- The executable will be located in `/build/bin/release`
