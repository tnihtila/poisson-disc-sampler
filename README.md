# poisson-disc-sampler

Tool to generate equally-separated positions inside an area using Poisson disc sampling methodology.

## Overview

`poisson-disc-sampler` generates a number of randomly picked points inside a given area with a maximum achievable distance from each others and the area borders using Poisson disc sampling.

Tool is written in **C++**.

## Features

- Reads given input CSV file defining the area inside which points are generated.
- Plots the generated points.

## Prerequisities

`poisson-disc-sampler` requires the X11 development package

On Debian/Ubuntu:
   ```bash
   sudo apt-get install libx11-dev
   ```

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/tnihtila/poisson-disc-sampler.git
   ```

2. Build
   ```bash
   cd poisson-disc-sampler
   mkdir build
   cd build
   cmake ..
   make
   ```

3. Test with the example input file and parameters
   ```bash
   # 300 positions with max 500 attempts per generated point
   ./bin/poisson-disc-sampler ../input/area.csv 300 500
   ```
