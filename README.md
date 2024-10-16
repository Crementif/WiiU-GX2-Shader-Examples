# Wii U GX2 Shader Examples

This repository contains a collection of shader samples for the [GX2 graphics API](https://wiiubrew.org/wiki/Hardware/GX2) of the Wii U.
The current examples show you the very basics of how to write a shader for the Wii U and how to use it to draw a triangle or a textured quad, but I might expand them in the future. A basic understanding of OpenGL is required, this just shows the Wii U-isms a bit.

These GX2 samples use the new [CafeGLSL shader compiler](https://github.com/Exzap/CafeGLSL) which finally allows Wii U homebrew developers to use their own shaders without having to use hand-assembled shaders.

## Current examples
 - [Triangle](./source/example_triangle.cpp) - Draws a triangle
 - [Textured Quad](./source/example_texture.cpp) - Draws a texture on a full-screen quad

## External examples
 - ~~[Tank Trap](https://github.com/Crementif/UntitledSandGame)~~(releasing soon!) - A game-jam game made that also uses CafeGLSL!
 - [Immaterial](https://github.com/glastonbridge/immaterial-wiiu-demo) ([Blog Article](http://www.slipstreamdemo.info/?p=137)) - A Wii U demo for Slipstream with 3D graphics rendered using a custom Wii U engine.
 - [We Are Outdated](https://github.com/vurpo/we-are-outdated/) - A follow-up from the previous Wii U demo which additionally has framebuffer readback and other framebuffer effects.
 - [Super Mario 64 Port](https://github.com/aboood40091/sm64-port) - A port of Super Mario 64 to the Wii U showing general GX2 usage.
 - GTA3 Wii U port - A port of GTA3 to the Wii U that unfortunately got DMCA'd, although the source code is probably still somewhere online.
 - [angle](https://github.com/GaryOderNichts/angle/tree/wiiu/gx2) - A work-in-progress port of ANGLE which brings OpenGL ES support to the Wii U. This contains a lot of GX2 code and could help with porting OpenGL code.
 - [rio](https://github.com/aboood40091/rio) - A game development framework for the Wii U, although it doesn't use CafeGLSL yet.
 - ... probably more!

## Building
 - Install [devkitPRO](https://devkitpro.org/wiki/Getting_Started)
 - Install devkitPPC and wut through devkitPro's pacman by using `(dkp-)pacman -S wiiu-dev`
 - Run `make` in the root of the project directory
 - To run these examples on Cemu or your Wii U, you need to manually install CafeGLSL. See the [CafeGLSL repository](https://github.com/Exzap/CafeGLSL?tab=readme-ov-file#usage) for more information.

## License
The code is licensed under [The Unlicense](./LICENSE.md) which basically means you don't have to give credit or anything, just do whatever you want with it.
This excludes the [CafeGLSL binary](./cemu/cafeLibs/glslrecompiler.rpl) which is licensed under its own [license](https://github.com/Exzap/CafeGLSL?tab=readme-ov-file#-license).