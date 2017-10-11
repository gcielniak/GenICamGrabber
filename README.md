# GenICamGrabber
An OpenCV-based grabber for machine vision cameras supported by the Common Vision Blox library by Stemmer Imaging.
## Prerequisites
- [Boost](http://www.boost.org/users/history/version_1_65_1.html) (tested +1.60)
- [OpenCV](https://opencv.org/releases.html) (tested +3.x)
- [Common Vision Blox](https://www.commonvisionblox.com/en/cvb-download/) (tested +2016 SP1, 12.1)

## Building
The following setup was built using CMake (3.3.0) and Visual Studio 2015 on Windows 10:
- set up the project: `mkdir build & cd build & cmake .. -G "Visual Studio 14 2015 Win64"`;
- to quickly build the program from a command line: `cmake --build . --config release`.

# GeniCam SDK overview
- Basler  - [Pylon](https://www.baslerweb.com/en/products/software/) - only Basler cameras, multi-OS support
- Teledyne Dalsa - [Sapera LT](http://www.teledynedalsa.com/imaging/products/software/sapera/lt/): only Dalsa cameras, Windows/Linux (not-confirmed)
- Stemmer Imaging - [Common Vision Blox](https://www.commonvisionblox.com/en/) - multi-camera support, Windows/Linux(not-tested), licence fee (free with the camera)


