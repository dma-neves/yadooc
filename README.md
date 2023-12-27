# Yet Another Doom Clone

![alt text](/other/doom_stairs_render_crop.png)

## Description

- Another 2.5D DOOM inspired software renderer written in C++.
- This renderer sits between a Wolfenstein and a Doom renderer, as it doesn't explicitly cast rays for each pixel column like in Wolfenstein, but rather projects each surface to the projection plane like Doom. However, contrary to Doom, no BSP and sector based rendering technique are used.
- yadooc receives a map data structure as input to render. This data structure consists of a set of prisms that can have any number of edges but must be convex.
- Currently only wall texture mapping is supported (no ground texture mapping).

## Requirements

- cmake
- SFML

## Building and running

- To build run: `mkdir build; cd build; cmake -DCMAKE_BUILD_TYPE=Release ..; make`
- The executable will be located in `/build/bin/release`
