# poisson-disc-sampler

A C++ tool to generate equally-separated positions inside a user-defined area using Poisson disc sampling.

## Features
- Reads an input CSV file defining the area.
- Generates random points with a minimum distance between them and from borders.
- Plots the generated points using CImg and X11.

## Requirements
- C++ compiler (C++11 or newer)
- CMake
- [CImg](https://cimg.eu/) (included)
- X11 development package (`libx11-dev`)

## Build
```bash
sudo apt-get install libx11-dev
cd poisson-disc-sampler
mkdir build && cd build
cmake ..
make
```

## Usage
```bash
./bin/poisson-disc-sampler <area.csv> <num_positions> <max_attempts>
```
- `<area.csv>`: CSV file with area definition (see below)
- `<num_positions>`: Number of points to generate
- `<max_attempts>`: Max attempts per point before rejection

Example:
```bash
./bin/poisson-disc-sampler ../input/area.csv 300 500
```

## Input File Format
CSV with lines: `x,y,flag`  
`flag=1` means inside area, `flag=0` means outside.

## License
See `CImg.h` for CImg license. Project code is MIT licensed unless otherwise stated.
