# zedutils
zedutils is a library of utility functions for the ZED and ZED Mini. zedutils is built on the [ZED SDK 3.0](https://www.stereolabs.com/docs/api/).

## Requirements
The requirements of zedutils are:
- ZED SDK 3.X (available for Windows 10 and Ubuntu 16/18)
  - CUDA 9.X/10.X (dependent on the ZED SDK installation)
- C++17/20

## Configuring, building and testing
To configure zedutils:
```
cmake -S . -B build
```
To build zedutils:
```
cmake --build build
```
To test zedutils:
```
cmake --build build --target test
```
To build zedutils docs:
```
cmaek --build build --target docs
```

## Planned features
- [] Error handling functions
- [] ZED camera controller
- [] ZED camera calibration
- [] ZED data recorder
