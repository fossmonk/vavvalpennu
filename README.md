# VavvalPennu

## Introduction

VavvalPennu is my second single player batgirl 2d game in C, this time using Raylib. This is supposed to be a more complex version compared to the first game: [BATGIRL](https://github.com/fossmonk/batgirl-platformer). That one is written with the TIGR library. It was simple enough, but the bitmap renderings were a little limiting.

The name VavvalPennu means "batgirl" in Malayalam. The first game was Batgirl in China. In this 
version, we see Batgirl coming to Kerala, India which has a rich cultural horror lore. More about 
this in the gameplay section.

VavvalPennu draws inspiration from classic 2D sidescroller/platformers like Mario with some 
key differences. Sprites are more realistic, map is technically endless (although there are
some hard limits on world x cooordinates). Since this is a sidescroller we also have a camera
which moves only in the x direction. This utilizes the Camera2D functionality from raylib.

VavvalPennu is currently WIP.

## Supported platforms

VavvalPennu is playable in windows, linux and mac. The make command auto-detects the OS and
builds the appropriate binary. There is experimental support for WASM, but this is currently
broken - and will be developed only after the game is complete for other platforms.

## Gameplay

You play as VavvalPennu or Batgirl. Like Mario, you're supposed to collect the `karikku`s 
(tender coconut), kill the monsters on the way (or jump over them), and increase your score. 
There are also other collectibles coming soon. There are 5 levels planned for this game. 
Levelling up is a function of your score, number of collected `karikku`s and other artifacts. 
In each level you will have to defeat a boss to advance.

The bosses are heavily inspired from the Studio Eksaurus short film "Kandittund!" which talks 
about monsters and demons in the Kerala horror lore.

### Movement and Attacks

| Movement         | Control      |
|:---------------: | :-----------:| 
| Run/Move Left    |      A       |
| Run/Move Right   |      D       |
| Jump             |    Space     |
| Whip Attack      |     LMB      |
| Batarang Throw   |     RMB      |

You can run over/jump over collectibles to pick them up. To throw the batarang to a point, 
simply right-click at a point. The batarang will launch from player to that point and continue
in that straight line. Whip attack is based on proximity and mouse position. To whip to the right, 
keep the mouse pointer to the right of player and left-click. Same for left direction.

### HUD

HUD is currently WIP, but it shows your healthbar, score and collected `karikku`s as of now.

### Demo video

https://github.com/user-attachments/assets/28c5a5f3-ffa1-4701-b0ff-c621f84142ea

## Building and Running VavvalPennu

As of now there are no release packages, you will have to build from source to run it. Release 
packages for different platforms are coming soon.

### Language

The game is written entirely in C. There are helper scripts/tools in python. But the game code 
is fully written in C.

### Library Dependency

Raylib is the only library needed other than libc. In linux and mac I recommend building 
raylib from source. In windows one can download the library and header files (self contained,
not DLL). Please checkout the raylib documentation how to install this. Also go through 
the makefiles to understand where to keep raylib libraries and headers. Feel free to modify
these paths according to your convenience. 

### Toolchain

I am using clang as the C compiler. You're welcome to try others.

### Building

After all dependencies and toolchain reqs are met, to build the game, simply run make.

```bash
$ make
```
This should generate these files according to your OS:

- Windows   : `vavvalpennu.exe`
- Linux/Mac : `vavvalpennu`

Note that the executable is not self contained. It relies on asset files and shader source files
in the repo. So it is recommended not to copy the executable somewhere else. In the immediate 
future, I will create a `package` make target which will create a game zipfile which can be 
extracted and played. In the far future, I will consider resource cramming into executable itself
to make it a self contained executable.

## For the GameDevs and Tinkerers

### Basic Configuration

A lot of the config used in the game - asset file paths, game window dimensions etc... are in 
the top level `vpconfig.h` file. Modify this at your own risk ;). Increasing the number of 
max assets like batarangs or fireflys can lead to game crashes, but still you're welcome to 
play around.

### Structure

Every source and header file is in the `src` folder except for the global `vpconfig.h`
For maintainability and modularity, all game objects have their own `.c` and `.h` files.
The `main.c` is mostly an initial setup and just calls main `game_` apis from `game.c`

`game.c` is the top level application code. This has the start menu, pause menu and main 
game loops. I have tried to be as modular as possible and use functions from other modules 
for activities related to that object as much as possible. But I had to leave out some things 
like collision to stay in `game.c`, because it involves two objects at a time.

### Collision detection

The current collision detection in VavvalPennu is quite rudimentary even by 2D standards. Bounding 
boxes are generated for each animation (not each frame, although I will consider that in future). 
The bounding box information is used to apply basic collisions. As of now only circle and 
rectangle bounding boxes are used. This means we have 3 types of collision detection:

- Circle to Circle (Radial distance check)
- Rectangle to Rectangle (AABB check)
- Circle to Rectangle (We find the side of rectangle closest to the circle and check if the distance from center to the side is less than the radius)

There is a future plan to add generic polygon bounding boxes if it comes to that, and maybe 
different bounding boxes for each frame.

### Game Art

I am a programmer, not an artist. While that is not an excuse, I will shamelessly admit that 
most of the game assets were either sourced from royalty free images off the internet and then 
edited in Paint/Photoshop and in some cases generated with AI based sprite generators. I would 
be more than happy to collaborate with a 2D game artist to improve the game art.

### Philosophy and AI Usage

I have tried to mostly rely on my C expertise and the raylib documentation to write the core 
game code which is in C. Sloppy as it maybe, I wanted to stay away from using an AI agent to 
write this, because rather than this being a product, my aim is to understand the game dev 
programming from first principles. I will keep it that way for the entirety of the project. If 
I just wanted to build the game, I could have done that in GoDot or some engine like that. I want
this to be a learning experience. I have consulted stackoverflow from time to time, to 
check for alternate approaches for a specific problem but have not generated a single line of C code in this project.

That being said, I have extensively used LLMs to code the helper scripts for manipulating the 
sprites. Because game art designing is not my focus area at the moment. Who knows, maybe I will 
go through a phase where I manually do all the art and use an agent for coding the game! But 
VavvalPennu shall remain a handcoded game in C.
