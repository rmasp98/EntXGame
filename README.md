# EntXGame
This game is not even close to being complete yet!!! I am a complete self-taught novice so would appreciate any advice or improvements to the game.

## Intro
This is purely a learning experience in trying to create a game using relatively low level libraries (OpenGL, GLFW, EntityX). The game is based upon the Japanese puzzle game, Sokoban. The game is simply a room with boxes in and the aim of the game is to move all the boxes onto specific targets in the room. An example of this game can be seen in the figure below:


## Install
Currently the game does not come with any of the required libraries so they will need to be installed beforehand. It is C++ code so obviously you will need a C++ compiler (if you choose something other than g++, you will need to alter to makefile to reflect this). The following libraries can be installed generally by the distribution you are running:
* libGL
* libGLU
* libglut
* libx11
* libXxf86vm
* lpthread
* lXrandr
* lXi
* lglfw
* lGLEW
* lSOIL
* lfreetype

There is also an additional library required that manages the Entity Component System (ECS) that the game is based upon. This can be downloaded from: https://github.com/alecthomas/entityx. Follow the instructions in the GitHub page to install the library ensuring that the install directory is /usr rather than the default /usr/local.

Once all the libraries are installed, clone this repository and run make from the cloned directory. Disclaimer, I have not really done any testing so couldn't say how reliably this will work but it is pretty simple so should not have too many issues.
