# GenICamGrabber
An OpenCV-based grabber for machine vision cameras supported by the Common Vision Blox library by Stemmer Imaging.
## Prerequisites
- Boost (1.61)
- OpenCV (3.1.0)
- Common Vision Blox (2016 SP1, 12.1)

## Building
The following setup was built using CMake (3.3.0) and Visual Studio 2015 on Windows 10:
- set up the project: `mkdir build & cd build & cmake .. -G "Visual Studio 14 2015 Win64"`;
- to quickly build the program from a command line: `cmake --build . --config release`.

# GeniCam SDK overview
- Basler  - (Pylon)[https://www.baslerweb.com/en/products/software/] - only Basler cameras, multi-OS support
- Teledyne Dalsa - (Sapera LT)[http://www.teledynedalsa.com/imaging/products/software/sapera/lt/]: only Dalsa cameras, Windows/Linux (not-confirmed)
- Stemmer Imaging - (Common Vision Blox)[https://www.commonvisionblox.com/en/] - multi-camera support, Windows/Linux(not-tested), licence fee (free with the camera)


