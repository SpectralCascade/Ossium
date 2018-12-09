# About Ossium
Ossium is a bare-bones 2D game engine being built with C++ and SDL 2. The aim of this project is to create a simple, lightweight 2D game engine which is easy to use and simultaneously acts as a nice wrapper for SDL 2.

# Features
* Rendering with layer support
* Serialisation support
* Basic texture packer with mipmap generator
* Resource controller for simple resource management
* Basic vector maths functions and various intersection tests
* Simple Entity Component System

# Pre-requisites
All these components are listed on the [main SDL 2 webpage](https://www.libsdl.org/download-2.0.php) and the [SDL 2 projects page](https://www.libsdl.org/projects).
* SDL 2.0.8 (or greater)
* SDL_Image
* SDL_Mixer
* SDL_TTF

# Additional notes
I'm developing this project using Code::Blocks 17.12 on 64-bit (arm) Linux
with the GNU GCC mingw compiler. The engine is easily portable to Windows or Mac (you may need to change SDL 2 includes by removing the initial "SDL2/" for Windows builds).
Check out Lazy Foo's SDL 2 tutorials at www.lazyfoo.net if you're unfamiliar with SDL; if you don't know how to setup SDL 2 to build Ossium, check out the first tutorial.
