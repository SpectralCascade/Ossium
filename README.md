# About Ossium
Ossium is a bare-bones 2D game engine being built with C++ and SDL 2. The aim of this project is to create a simple, lightweight 2D game engine which is easy to use and simultaneously acts as a nice wrapper for SDL 2.

# Features
* Low number of dependencies (SDL 2 with basic extension libraries)
* Extendable input system with action bindings, states and input claiming
* Flexible Entity Component System
* Game event system with delay-based queuing
* Basic class serialisation support via completely code-based schemas
* Animation system based around timelines and animator clips
* Built-in CSV import/export and JSON parsing
* Basic texture packer with mipmap generator
* Resource controller for simple resource management
* Audio playback and mixing
* Vector maths functions and various intersection tests for primitive shapes
* Simple unit testing framework
* Rendering with layer support

# Pre-requisites
All SDL libraries are listed on the [main SDL 2 webpage](https://www.libsdl.org/download-2.0.php) and the [SDL 2 projects page](https://www.libsdl.org/projects).
* A compiler with the C++17 standard (e.g. GCC 8.1.0)
* SDL 2.0.8 (or greater)
* SDL_Image
* SDL_Mixer
* SDL_TTF

# Additional notes
I'm developing this project using Code::Blocks 17.12 on Windows 10 64-bit currently (and sometimes on (arm) Linux 64-bit)
with the GNU GCC/G++ compiler. The engine is easily portable to Linux or Mac (you may need to change SDL 2 includes by find and replace of initial `<SDL` to `<SDL2/SDL` for Linux builds).

Before building, please make sure you are compiling with C++17 enabled (use compile option `-std=c++1z` or `-std=c++17` and make sure you're using GCC 8 or later) as the project makes use of some of those features.

Check out Lazy Foo's SDL 2 tutorials at www.lazyfoo.net if you're unfamiliar with SDL; if you don't know how to setup SDL 2 to build Ossium, check out the first tutorial.

P.S. if you make anything cool with Ossium, I'd love to hear about it!
