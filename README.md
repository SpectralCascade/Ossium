# About Ossium
Ossium is a bare-bones, cross-platform game engine built in C++ with only essential dependencies. A number of games made with Ossium can be [found here](https://itch.io/c/1747446/ossium-engine-games).

WIP working towards a proper release.

## Features
* Low number of dependencies (SDL 2 with a few small extension libraries and Box2D for physics)
* Extendable serialisation support via completely code-based schemas
* JSON serialisable Entity-Component System
* Extendable input system with action bindings, states and input claiming
* CSV and JSON parsing
* Audio playback and mixing
* Simple unit testing framework
* Rendering with layer support

## Pre-requisites
* mingw toolchain supporting the latest GCC versions, for Windows this is installed via [MSYS2](https://www.msys2.org). Make sure you setup the system path to point at MSYS2, mingw root directory AND the mingw bin folder.
* Python 3 (must be version 3.8 or newer)
* [soupbuild](https://github.com/SpectralCascade/soupbuild) is required to build Ossium. You can make a quick debug build for Windows by running `python3 soupbuild.py` in the Ossium root directory; this will output `libOssium.a` in the generated `soupbuild/outputs/Windows/debug` directory.

## How to Use

Ossium is currently best used as a static library.

At some stage I intend to write up documentation for Ossium (once I've cleaned up the code a bit). Until then, you can check up on [my website](https://timlanesoftware.com) from time to time for blog posts and articles about Ossium (and other projects I'm working on). Or you can dive into the code to better understand how different parts of the engine work for yourself. Note that the engine is constantly being updated and improved currently, so if you intend to use it for your own games and projects beware of bugs and changes over time.

## Roadmap
I have a rough [todo list](https://github.com/SpectralCascade/Ossium/wiki/Todo) that details what I'm working on currently.

## Additional notes
I'm developing this project using Visual Studio Code and Make with the MinGW64 GCC/G++ compiler toolchain.

Before building, please make sure you are compiling with C++17 enabled (use compile option `-std=c++1z` or `-std=c++17` and make sure you're using GCC 8 or later) as the project makes use of some of those features.

Check out Lazy Foo's SDL 2 tutorials at www.lazyfoo.net if you're unfamiliar with SDL.

P.S. if you make anything cool with Ossium, I'd love to hear about it!
