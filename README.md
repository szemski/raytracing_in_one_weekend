# Ray Tracing in One Weekend

## Overview
Software renderer implemented in C, following [Ray Tracing in One Weekend](https://raytracing.github.io/) book series.

Example render after finishing first book from the series:

![alt text](https://github.com/szemski/raytracing_in_one_weekend/blob/main/render.png?raw=true)

## Build
Code was tested only on Windows.
This project uses [premake5](https://premake.github.io/) as a build system.

For example, in order to generate VS2022 solution, download premake5 and run terminal in this repository directory, then run `premake5 msvc2022`. See premake documentation for more details.
